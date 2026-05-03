import numpy as np
import pandas as pd
import torch
from torch import nn
from torch.utils.data import DataLoader, Dataset
from torchvision import datasets, transforms
from torchvision.transforms import ToTensor

import os
import re

PIECE_MAP = {
    'p': 0, 'n': 1, 'b': 2, 'r': 3, 'q': 4, 'k': 5,
    'P': 6, 'N': 7, 'B': 8, 'R': 9, 'Q': 10, 'K': 11
}

def fen_to_bitarray(fen_string):
    bitarray = np.zeros(773, dtype=np.float32)
    
    fen_parts = fen_string.strip().split(' ')
    board_part = fen_parts[0]
    
    board_rows = re.split(r'[/\s]+', board_part)
    for row_idx in range(8):
        cur_col = 0
        for c in board_rows[row_idx]:
            if c.isdigit():
                cur_col += int(c)
            else:
                piece_offset = PIECE_MAP[c] * 64
                square_idx = row_idx * 8 + cur_col
                bitarray[piece_offset + square_idx] = 1
                cur_col += 1
                
    if len(fen_parts) >= 3:
        turn_part = fen_parts[1]
        castling_part = fen_parts[2]
        
        if turn_part == 'b':
            bitarray[768] = 1
            
        if 'K' in castling_part: bitarray[769] = 1
        if 'Q' in castling_part: bitarray[770] = 1
        if 'k' in castling_part: bitarray[771] = 1
        if 'q' in castling_part: bitarray[772] = 1
            
    return bitarray

def parse_fen_with_perspective(fen_string, score_value):
    fen_parts = fen_string.strip().split(' ')
    board_part = fen_parts[0]
    turn_part = fen_parts[1] if len(fen_parts) > 1 else 'w'
    castling_part = fen_parts[2] if len(fen_parts) > 2 else '-'
    
    if turn_part == 'b':
        score_value = -score_value
        
        board_part = board_part.swapcase()
        
        board_rows = board_part.split('/')
        board_rows.reverse()
        board_part = '/'.join(board_rows)
        
        castling_part = castling_part.swapcase()
        
    bitarray = np.zeros(772, dtype=np.float32)
    
    board_rows = re.split(r'[/\s]+', board_part)
    for row_idx in range(8):
        cur_col = 0
        for c in board_rows[row_idx]:
            if c.isdigit():
                cur_col += int(c)
            else:
                piece_offset = PIECE_MAP[c] * 64
                square_idx = row_idx * 8 + cur_col
                bitarray[piece_offset + square_idx] = 1
                cur_col += 1
                
    if 'K' in castling_part: bitarray[768] = 1 
    if 'Q' in castling_part: bitarray[769] = 1 
    if 'k' in castling_part: bitarray[770] = 1 
    if 'q' in castling_part: bitarray[771] = 1 
        
    return bitarray, score_value

class ChessEvaluator(nn.Module):
    def __init__(self):
        super().__init__()
        self.network = nn.Sequential(
            nn.Linear(772, 1024),
            
            nn.ReLU(),
            
            nn.Linear(1024, 1)
        )

    def forward(self, x):
        evaluation = self.network(x)
        return evaluation
    
class FENDataset(Dataset):
    def __init__(self, fen_file, transform=None, target_transform=None):
        self.fens = []
        self.scores = []
        
        with open(fen_file, 'r') as f:
            for line in f:
                parts = line.strip().split(',')
                if len(parts) == 2:
                    self.fens.append(parts[0])
                    self.scores.append(float(parts[1]))
                    
        self.scores = np.array(self.scores, dtype=np.float32)
        print(f"Loaded {len(self.fens)} positions successfully!")

    def __len__(self):
        return len(self.fens)

    def __getitem__(self, idx):
        fen_string = self.fens[idx]
        original_score = self.scores[idx]
        
        bit_array, adjusted_score = parse_fen_with_perspective(fen_string, original_score)
        
        features = torch.from_numpy(bit_array).float()
        label = torch.tensor([adjusted_score], dtype=torch.float32)
        
        return features, label

def main():
    BATCH_SIZE = 2048
    LEARNING_RATE = 0.001
    EPOCHS = 20

    device = "cuda" if torch.cuda.is_available() else "cpu"
    print(f"Training on device: {device}")

    dataset = FENDataset(fen_file="data/fen_to_stockfish_evaluation.csv")

    dataloader = DataLoader(
        dataset, 
        batch_size=BATCH_SIZE, 
        shuffle=True, 
        num_workers=4,
        pin_memory=True
    )

    model = ChessEvaluator().to(device)
    
    #loss_fn = nn.MSELoss()
    loss_fn = nn.HuberLoss(delta=1.0)

    optimizer = torch.optim.Adam(model.parameters(), lr=LEARNING_RATE)
    
    for epoch in range(EPOCHS):
        print(f"--- Epoch {epoch+1}/{EPOCHS} ---")
        
        running_loss = 0.0
        
        for batch_idx, (features, labels) in enumerate(dataloader):
            
            features = features.to(device)
            labels = labels.to(device)
            
            labels = labels / 100.0
            
            optimizer.zero_grad()
            
            predictions = model(features)
            
            loss = loss_fn(predictions, labels)
            
            loss.backward()
            
            optimizer.step()
            
            
            running_loss += loss.item()
            
            if batch_idx % 500 == 0:
                print(f"Batch {batch_idx}/{len(dataloader)}, Loss: {loss.item():.4f}")
                
        avg_loss = running_loss / len(dataloader)
        print(f"Epoch {epoch+1} finished! Average Loss: {avg_loss:.4f}\n")

    torch.save(model.state_dict(), "chess_evaluator_weights_1024.pth")
    print("Training complete and model saved!")

if __name__ == "__main__":
    main()