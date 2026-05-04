#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>
#include <array>
#include <vector>
#include <string>

using namespace std;

#define CHESS_WHITE 0
#define CHESS_BLACK 1

const float INF = 1000000.0f;

enum PieceName {
    EMPTY, 

    WHITE_PAWN,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_ROOK,
    WHITE_QUEEN,
    WHITE_KING,

    BLACK_PAWN,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_ROOK,
    BLACK_QUEEN,
    BLACK_KING
};

inline int get_piece_color(PieceName p) {
    if (p >= WHITE_PAWN && p <= WHITE_KING) return CHESS_WHITE;
    if (p >= BLACK_PAWN && p <= BLACK_KING) return CHESS_BLACK;
    return -1; // EMPTY
}

inline char getPieceSymbol(PieceName p) {
    switch (p) {
        case WHITE_PAWN:     return 'p';
        case WHITE_KNIGHT:   return 'n';
        case WHITE_BISHOP:   return 'b';
        case WHITE_ROOK:     return 'r';
        case WHITE_QUEEN:    return 'q';
        case WHITE_KING:     return 'k';
        case BLACK_PAWN:     return 'P';
        case BLACK_KNIGHT:   return 'N';
        case BLACK_BISHOP:   return 'B';
        case BLACK_ROOK:     return 'R';
        case BLACK_QUEEN:    return 'Q';
        case BLACK_KING:     return 'K';
        default:             return '.';
    }
}

struct Square {
    int row; // 0 to 7
    int col; // 0 to 7

    bool operator==(const Square& other) const {
        return row == other.row && col == other.col;
    }
};

struct Move {
    Square from;
    Square to;
    PieceName captured_piece = EMPTY; // The piece captured in this move
    PieceName promotion = EMPTY; // This move is a promotion
    bool castle_kingside = false; // This move was a kingside castle
    bool castle_queenside = false; // This move was a queenside castle
    bool first_move_king = false; // For tracking ability to castle
    bool first_move_a_rook = false; // For tracking ability to castle queenside
    bool first_move_h_rook = false; // For tracking ability to castle kingside
    bool double_pawn_push = false; // For tracking en passant
    bool en_passant = false; // This move is an en passant
    bool en_passant_was_possible = false; // Prior to making this move, en passant was possible
    int previous_en_passant_col = 0;
    bool captured_a_rook = false; // Capturing this move disabled queenside castling
    bool captured_h_rook = false; // Capturing this move disabled kingside castling
    
    bool operator==(const Move& other) const {
        return from == other.from && to == other.to;
    }
};

inline Move translate_move(string& move) {
    Square from{ (move[1] - '1'), (move[0] - 'a') };
    Square to{ (move[3] - '1'), (move[2] - 'a') };

    return Move{from, to};
}

struct MoveList {
    array<Move, 256> moves;
    int count = 0;

    void push(const Move& move) {
        moves[count++] = move;
    }

    int size() const {
        return count;
    }

    Move* begin() { return &moves[0]; }
    Move* end() { return &moves[count]; }

    bool contains(const Move& target) {
        for (int i = 0; i < count; i++) {
            if (moves[i] == target) return true;
        }
        return false;
    }

    Move find_match(const Move& user_move) {
        for (int i = 0; i < count; i++) {
            if (moves[i] == user_move) {
                return moves[i];
            }
        }

        return Move{}; 
    }

    // Get a list of square indexes (0-63) where a specific piece can go to
    vector<int> get_possible_piece_moves(const Square& piece_square) const {
        vector<int> possible_squares;

        for (int i = 0; i < count; i++) {
            if (moves[i].from == piece_square) {
                possible_squares.push_back(moves[i].to.row * 8 + moves[i].to.col);
            }
        }
        return possible_squares;
    }
};

inline void print_move(Move move) {
    cout << (char)('a' + move.from.col) << move.from.row + 1 << " " << (char)('a' + move.to.col) << move.to.row + 1 << endl;
}

class Board 
{
public:
    array<PieceName, 64> board;

    Board(array<PieceName, 64> starting_board) : board(starting_board) {
    }

    PieceName get_piece(Square square) {
        return board[square.row * 8 + square.col];
    }

    void place_piece(Square square, PieceName piece) {
        board[square.row * 8 + square.col] = piece;
    }

    void remove_piece(Square square) {
        board[square.row * 8 + square.col] = EMPTY;
    }

    void move_piece(Square from_square, Square to_square, PieceName piece) {
        remove_piece(from_square);
        place_piece(to_square, piece);
    }

    void display(int player_color = CHESS_WHITE) {
            if (player_color == CHESS_WHITE) {
                for (int row = 7; row >= 0; row--) {
                    cout << row + 1 << " "; 
                    for (int col = 0; col < 8; col++) {
                        cout << "[" << getPieceSymbol(board[row * 8 + col]) << "]";
                    }
                    cout << endl;
                }
                cout << "   a  b  c  d  e  f  g  h" << endl;
            } 
            else {
                for (int row = 0; row <= 7; row++) {
                    cout << row + 1 << " "; 
                    for (int col = 7; col >= 0; col--) {
                        cout << "[" << getPieceSymbol(board[row * 8 + col]) << "]";
                    }
                    cout << endl;
                }
                cout << "   h  g  f  e  d  c  b  a" << endl;
            }
    }
};

class Chessgame
{
public:
    Board board;
    bool turn;

    bool black_castled;
    bool white_castled;

    bool white_king_moved;
    bool black_king_moved;

    bool white_a_rook_moved;
    bool black_a_rook_moved;

    bool white_h_rook_moved;
    bool black_h_rook_moved;

    Square white_king_pos{0, 4};
    Square black_king_pos{7, 4};

    bool en_passant_possible;
    int en_passant_col;

    Chessgame(array<PieceName, 64> starting_board) : board(starting_board) {
        turn          = CHESS_WHITE;

        black_castled = false;
        white_castled = false;

        white_king_moved = false;
        black_king_moved = false;

        white_a_rook_moved = false;
        black_a_rook_moved = false;

        white_h_rook_moved = false;
        black_h_rook_moved = false;

        en_passant_possible = false;

        white_king_pos = {0, 4};
        black_king_pos = {7, 4};


        for (int i = 0; i < 64; i++) {
            PieceName piece = board.board[i];
            
            if (piece == EMPTY) continue;

            int row = i / 8;
            int col = i % 8;
            Square current_sq{row, col};
        }
    }

    void swap_turn() {
        turn = turn == CHESS_WHITE ? CHESS_BLACK : CHESS_WHITE;
    }
};

#endif // HELPERS_H