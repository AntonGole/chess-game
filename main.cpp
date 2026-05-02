#include <iostream>
#include <random>
#include <chrono>

#include "helpers.h"
#include "moves.h"

using namespace std;

array<PieceName, 64> DEFAULT_BOARD = {
        WHITE_ROOK,   WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN,  WHITE_KING,   WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK,
        WHITE_PAWN,   WHITE_PAWN,   WHITE_PAWN,   WHITE_PAWN,   WHITE_PAWN,   WHITE_PAWN,   WHITE_PAWN,   WHITE_PAWN,
        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,
        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,
        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,
        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,        EMPTY,
        BLACK_PAWN,   BLACK_PAWN,   BLACK_PAWN,   BLACK_PAWN,   BLACK_PAWN,   BLACK_PAWN,   BLACK_PAWN,   BLACK_PAWN,
        BLACK_ROOK,   BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN,  BLACK_KING,   BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK
    };

array<PieceName, 64> TEST_BOARD_1 = {
        WHITE_ROOK, EMPTY, EMPTY, EMPTY, WHITE_KING, EMPTY, EMPTY, WHITE_ROOK,
        EMPTY,      EMPTY, EMPTY, EMPTY, EMPTY,      EMPTY, EMPTY, EMPTY,
        EMPTY,      EMPTY, EMPTY, EMPTY, EMPTY,      EMPTY, EMPTY, EMPTY,
        EMPTY,      EMPTY, EMPTY, EMPTY, EMPTY,      EMPTY, EMPTY, EMPTY,
        EMPTY,      EMPTY, EMPTY, EMPTY, EMPTY,      EMPTY, EMPTY, EMPTY,
        EMPTY,      EMPTY, EMPTY, EMPTY, EMPTY,      EMPTY, EMPTY, EMPTY,
        EMPTY,      EMPTY, EMPTY, EMPTY, EMPTY,      EMPTY, EMPTY, EMPTY,
        BLACK_ROOK, EMPTY, EMPTY, EMPTY, BLACK_KING, EMPTY, EMPTY, BLACK_ROOK
    };

int main() {
    Chessgame game(DEFAULT_BOARD);

    int depth = 6;
    cout << "Running Perft at depth " << depth << "..." << endl;

    auto start_time = chrono::high_resolution_clock::now();
    uint64_t total_nodes = perft(game, depth);
    auto end_time = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end_time - start_time;

    cout << "Nodes generated: " << total_nodes << endl;
    cout << "Time taken: " << elapsed.count() << " seconds" << endl;
    
    return 0;
}