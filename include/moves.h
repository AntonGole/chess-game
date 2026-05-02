#ifndef MOVES_H
#define MOVES_H

#include <iostream>
#include <vector>

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

#endif // MOVES_H