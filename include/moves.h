#ifndef MOVES_H
#define MOVES_H

#include <iostream>
#include <vector>

#include "helpers.h"

using namespace std;

vector<Move> get_psuedo_moves(Chessgame& game, bool generate_castling = true);

void get_legal_moves_white(Chessgame& game, vector<Move>& legal_moves, bool generate_castling);

void get_legal_moves_black(Chessgame& game,  vector<Move>& legal_moves, bool generate_castling);

void get_legal_moves_white_pawn(Chessgame& game, Square square, vector<Move>& legal_moves);

void get_legal_moves_black_pawn(Chessgame& game, Square square, vector<Move>& legal_moves);

void get_king_moves(Chessgame& game, Square square, vector<Move>& legal_moves, 
                    const vector<Square>& own_side, bool generate_castling);

void get_legal_moves_white_king(Chessgame& game, Square square, vector<Move>& legal_moves, bool generate_castling);

void get_legal_moves_black_king(Chessgame& game, Square square, vector<Move>& legal_moves, bool generate_castling);

void get_knight_moves(Chessgame& game, Square square, vector<Move>& legal_moves, 
                      const vector<Square>& own_side);

void get_legal_moves_white_knight(Chessgame& game, Square square, vector<Move>& legal_moves);

void get_legal_moves_black_knight(Chessgame& game, Square square, vector<Move>& legal_moves);

void get_sliding_moves(Chessgame& game, Square square, vector<Move>& legal_moves, 
                       const vector<int>& df, const vector<int>& dr, 
                       const vector<Square>& own_side, const vector<Square>& enemy_side);

void get_legal_moves_white_rook(Chessgame& game, Square square, vector<Move>& legal_moves);

void get_legal_moves_black_rook(Chessgame& game, Square square, vector<Move>& legal_moves);

void get_legal_moves_white_bishop(Chessgame& game, Square square, vector<Move>& legal_moves);

void get_legal_moves_black_bishop(Chessgame& game, Square square, vector<Move>& legal_moves);

void get_legal_moves_white_queen(Chessgame& game, Square square, vector<Move>& legal_moves);

void get_legal_moves_black_queen(Chessgame& game, Square square, vector<Move>& legal_moves);


void make_move(Chessgame& game, Move& move);

void unmake_move(Chessgame& game, Move& move);

void filter_legal_moves(Chessgame& game, vector<Move>& psuedo_moves);

bool is_in_check(Chessgame& game);

uint64_t perft(Chessgame& game, int depth);

#endif // MOVES_H