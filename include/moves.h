#ifndef MOVES_H
#define MOVES_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "helpers.h"

using namespace std;

void get_psuedo_moves(Chessgame& game, MoveList& moves, bool generate_castling = true);

void get_legal_moves_white(Chessgame& game, MoveList& legal_moves, bool generate_castling);

void get_legal_moves_black(Chessgame& game,  MoveList& legal_moves, bool generate_castling);

void get_legal_moves_white_pawn(Chessgame& game, Square square, MoveList& legal_moves);

void get_legal_moves_black_pawn(Chessgame& game, Square square, MoveList& legal_moves);

void get_king_moves(Chessgame& game, Square square, MoveList& legal_moves, 
                    int own_color, bool generate_castling);

void get_legal_moves_white_king(Chessgame& game, Square square, MoveList& legal_moves, bool generate_castling);

void get_legal_moves_black_king(Chessgame& game, Square square, MoveList& legal_moves, bool generate_castling);

void get_knight_moves(Chessgame& game, Square square, MoveList& legal_moves, int own_color);

void get_legal_moves_white_knight(Chessgame& game, Square square, MoveList& legal_moves);

void get_legal_moves_black_knight(Chessgame& game, Square square, MoveList& legal_moves);

void get_sliding_moves(Chessgame& game, Square square, MoveList& legal_moves, 
                       const int dr[], const int dc[], int count, int own_color);

void get_legal_moves_white_rook(Chessgame& game, Square square, MoveList& legal_moves);

void get_legal_moves_black_rook(Chessgame& game, Square square, MoveList& legal_moves);

void get_legal_moves_white_bishop(Chessgame& game, Square square, MoveList& legal_moves);

void get_legal_moves_black_bishop(Chessgame& game, Square square, MoveList& legal_moves);

void get_legal_moves_white_queen(Chessgame& game, Square square, MoveList& legal_moves);

void get_legal_moves_black_queen(Chessgame& game, Square square, MoveList& legal_moves);


void make_move(Chessgame& game, Move& move);

void unmake_move(Chessgame& game, Move& move);

void filter_legal_moves(Chessgame& game, MoveList& psuedo_moves);

uint64_t perft(Chessgame& game, int depth);

bool is_square_attacked(Chessgame& game, Square square, int own_color);

class NNUE {
private:
    vector<float> l1_weights;
    vector<float> l1_biases;
    vector<float> l2_weights;
    vector<float> l2_biases;

    size_t input_size;
    size_t hidden_size;

public:
    NNUE(size_t input_layer_size, size_t hidden_layer_size) {
        input_size = input_layer_size;
        hidden_size = hidden_layer_size;

        l1_weights.resize(input_size * hidden_size);
        l1_biases.resize(hidden_size);
        l2_weights.resize(hidden_size);
        l2_biases.resize(1);
    }

    bool load_weights(const string& filepath) {
        ifstream file(filepath, ios::binary);

        if (!file.is_open()) {
            cout << "Error: Could not open NNUE file: " << filepath << endl;
            return false;
        }

        file.read(reinterpret_cast<char*>(l1_weights.data()), l1_weights.size() * sizeof(float));
        file.read(reinterpret_cast<char*>(l1_biases.data()), l1_biases.size() * sizeof(float));
        file.read(reinterpret_cast<char*>(l2_weights.data()), l2_weights.size() * sizeof(float));
        file.read(reinterpret_cast<char*>(l2_biases.data()), l2_biases.size() * sizeof(float));

        cout << "NNUE Weights loaded successfully!" << endl;
        cout << "Sanity Check - Weight 0: " << l1_weights[0] << endl;
        cout << "Sanity Check - Weight 500: " << l1_weights[500] << endl;
        return true;
    }

    float evaluate(const vector<int>& active_features) {
        
        // Hidden layer
        vector<float> hidden_layer_output(hidden_size, 0.0f);
        for (size_t hidden_idx = 0; hidden_idx < hidden_size; hidden_idx++) {
            float sum = l1_biases[hidden_idx];

            for (int active_idx : active_features) {
                sum += l1_weights[(input_size * hidden_idx) + active_idx];
            }

            if (sum < 0.0f) sum = 0.0f;

            hidden_layer_output[hidden_idx] = sum;
        }

        float final_score = l2_biases[0];
        for (size_t hidden_idx = 0; hidden_idx < hidden_size; hidden_idx++) {
            final_score += (hidden_layer_output[hidden_idx] * l2_weights[hidden_idx]);
        }
        
        return final_score * 100;
    }

    vector<int> get_active_features(const Chessgame& game) {
        vector<int> active_features;
        active_features.reserve(36);

        bool is_white_turn = (game.turn == CHESS_WHITE);

        for (int i = 0; i < 64; i++) {
            PieceName piece = game.board.board[i];

            if (piece == EMPTY) continue;

            int row = i / 8;
            int col = i % 8;
            int fen_index;

            if (is_white_turn) {
            // Invert the row, FEN strings start at A8
            fen_index = ((7 - row) * 8) + col;
            } else {
                fen_index = (row * 8) + col;
            }

            bool is_white_piece = (piece >= WHITE_PAWN && piece <= WHITE_KING);
            bool is_friendly = (is_white_turn == is_white_piece);

            // Convert enum to same id usage as FEN
            int base_type = 0;
            switch (piece) {
                case WHITE_PAWN:   case BLACK_PAWN:   base_type = 0; break;
                case WHITE_KNIGHT: case BLACK_KNIGHT: base_type = 1; break;
                case WHITE_BISHOP: case BLACK_BISHOP: base_type = 2; break;
                case WHITE_ROOK:   case BLACK_ROOK:   base_type = 3; break;
                case WHITE_QUEEN:  case BLACK_QUEEN:  base_type = 4; break;
                case WHITE_KING:   case BLACK_KING:   base_type = 5; break;
                default: continue; 
            }

            // If friendly piece shift ID up by 6 (to 6-11)
            int piece_id = is_friendly ? (base_type + 6) : base_type;

            // Push calculated index
            active_features.push_back((piece_id * 64) + fen_index);
        }

        bool white_kingside = !game.white_king_moved && !game.white_h_rook_moved;
        bool white_queenside = !game.white_king_moved && !game.white_a_rook_moved;
        bool black_kingside = !game.black_king_moved && !game.black_h_rook_moved;
        bool black_queenside = !game.black_king_moved && !game.black_a_rook_moved;

        if (is_white_turn) {
            // White is friendly
            if (white_kingside)  active_features.push_back(768);
            if (white_queenside) active_features.push_back(769);
            if (black_kingside)  active_features.push_back(770);
            if (black_queenside) active_features.push_back(771);
        } else {
            // Black is friendly (Indeces 768/769 belong to active player)
            if (black_kingside)  active_features.push_back(768);
            if (black_queenside) active_features.push_back(769);
            if (white_kingside)  active_features.push_back(770);
            if (white_queenside) active_features.push_back(771);
        }

        return active_features;
    }
};

float alpha_beta(Chessgame& game, int depth, float alpha, float beta, NNUE& evaluator);

Move get_best_move(Chessgame& game, int depth, NNUE& evaluator);

#endif // MOVES_H