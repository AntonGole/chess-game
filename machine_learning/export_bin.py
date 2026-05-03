import torch
import numpy as np

def export_weights_to_binary():
    state_dict = torch.load("chess_evaluator_weights_1024.pth", map_location='cpu', weights_only=True)

    l1_weights = state_dict['network.0.weight'].numpy()
    l1_biases  = state_dict['network.0.bias'].numpy()
    l2_weights = state_dict['network.2.weight'].numpy()
    l2_biases  = state_dict['network.2.bias'].numpy()

    print(f"Layer 1 Weights shape: {l1_weights.shape} (768 inputs connected to 256 hidden nodes)")
    print(f"Layer 1 Biases shape:  {l1_biases.shape}  (256 hidden node biases)")
    print(f"Layer 2 Weights shape: {l2_weights.shape}   (256 hidden nodes connected to 1 output)")
    print(f"Layer 2 Biases shape:  {l2_biases.shape}    (1 final output bias)")

    print("\nExporting to raw binary format...")
    
    with open("nnue_weights.bin", "wb") as f:
        l1_weights.astype(np.float32).tofile(f)
        l1_biases.astype(np.float32).tofile(f)
        l2_weights.astype(np.float32).tofile(f)
        l2_biases.astype(np.float32).tofile(f)

if __name__ == "__main__":
    export_weights_to_binary()