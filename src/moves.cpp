#include <algorithm>
#include <map>

#include "moves.h"

using namespace std;

void get_psuedo_moves(Chessgame& game, MoveList& moves, bool generate_castling) {
    if (game.turn == WHITE) get_legal_moves_white(game, moves, generate_castling);
    else get_legal_moves_black(game, moves, generate_castling);
}

void get_legal_moves_white(Chessgame& game, MoveList& legal_moves, bool generate_castling) {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Square sq{r, c};
            PieceName piece = game.board.get_piece(sq);

            if (piece == EMPTY || get_piece_color(piece) != WHITE) continue;

            switch (piece) {
                case WHITE_PAWN: 
                    get_legal_moves_white_pawn(game, sq, legal_moves);
                    break;
                case WHITE_KNIGHT: 
                    get_legal_moves_white_knight(game, sq, legal_moves);
                    break;
                case WHITE_BISHOP: 
                    get_legal_moves_white_bishop(game, sq, legal_moves);
                    break;
                case WHITE_ROOK: 
                    get_legal_moves_white_rook(game, sq, legal_moves);
                    break;
                case WHITE_QUEEN: 
                    get_legal_moves_white_queen(game, sq, legal_moves);
                    break;
                case WHITE_KING: 
                    get_legal_moves_white_king(game, sq, legal_moves, generate_castling);
                    break;
                default:
                    break;
            }
        }

    }
}

void get_legal_moves_black(Chessgame& game,  MoveList& legal_moves, bool generate_castling) {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Square sq{r, c};
            PieceName piece = game.board.get_piece(sq);

            if (piece == EMPTY || get_piece_color(piece) != BLACK) continue;

            switch (game.board.get_piece(sq)) {
                case BLACK_PAWN: 
                    get_legal_moves_black_pawn(game, sq, legal_moves);
                    break;
                case BLACK_KNIGHT: 
                    get_legal_moves_black_knight(game, sq, legal_moves);
                    break;
                case BLACK_BISHOP: 
                    get_legal_moves_black_bishop(game, sq, legal_moves);
                    break;
                case BLACK_ROOK: 
                    get_legal_moves_black_rook(game, sq, legal_moves);
                    break;
                case BLACK_QUEEN: 
                    get_legal_moves_black_queen(game, sq, legal_moves);
                    break;
                case BLACK_KING: 
                    get_legal_moves_black_king(game, sq, legal_moves, generate_castling);
                    break;
                default:
                    break;
            }
        }
    }
}

void get_legal_moves_white_pawn(Chessgame& game, Square square, MoveList& legal_moves) {
    auto on_board = [](int f, int r) { return f >= 0 && f < 8 && r >= 0 && r < 8; };

    auto add_pawn_move = [&](Square target, PieceName captured_piece) {
        if (target.row == 7) {
            legal_moves.push(Move{square, target, captured_piece, WHITE_QUEEN});
            legal_moves.push(Move{square, target, captured_piece, WHITE_ROOK});
            legal_moves.push(Move{square, target, captured_piece, WHITE_BISHOP});
            legal_moves.push(Move{square, target, captured_piece, WHITE_KNIGHT});
        } else {
            legal_moves.push(Move{square, target, captured_piece, EMPTY});
        }
    };

    Square push_single{square.row + 1, square.col};
    Square push_double{square.row + 2, square.col};
    Square diagonal_left{square.row + 1, square.col + 1};
    Square diagonal_right{square.row + 1, square.col - 1};

    if (on_board(push_single.row, push_single.col) && game.board.get_piece(push_single) == EMPTY) {
        add_pawn_move(push_single, EMPTY);

        if (square.row == 1 && game.board.get_piece(push_double) == EMPTY) {
        legal_moves.push(Move{square, push_double, EMPTY, EMPTY, 
                                   false, false, false, false, false, true});
        }
    }

    if (on_board(diagonal_left.row, diagonal_left.col)) {
        PieceName target_piece = game.board.get_piece(diagonal_left);
        if (get_piece_color(target_piece) == BLACK) {
            add_pawn_move(diagonal_left, target_piece);
        }
        // En passant
        else if (game.en_passant_possible && square.row == 4 && game.en_passant_col == square.col + 1) {
            legal_moves.push(Move{square, diagonal_left, BLACK_PAWN, EMPTY, false, false, false, false, false, false, true});
        }
    }

    if (on_board(diagonal_right.row, diagonal_right.col)) {
        PieceName target_piece = game.board.get_piece(diagonal_right);
        if (get_piece_color(target_piece) == BLACK) {
            add_pawn_move(diagonal_right, target_piece);
        }
        // En passant
        else if (game.en_passant_possible && square.row == 4 && game.en_passant_col == square.col - 1) {
            legal_moves.push(Move{square, diagonal_right, BLACK_PAWN, EMPTY, false, false, false, false, false, false, true});
        }
    }
}

void get_legal_moves_black_pawn(Chessgame& game, Square square, MoveList& legal_moves) {
    auto on_board = [](int f, int r) { return f >= 0 && f < 8 && r >= 0 && r < 8; };

    auto add_pawn_move = [&](Square target, PieceName captured_piece) {
        if (target.row == 0) {
            legal_moves.push(Move{square, target, captured_piece, BLACK_QUEEN});
            legal_moves.push(Move{square, target, captured_piece, BLACK_ROOK});
            legal_moves.push(Move{square, target, captured_piece, BLACK_BISHOP});
            legal_moves.push(Move{square, target, captured_piece, BLACK_KNIGHT});
        } else {
            legal_moves.push(Move{square, target, captured_piece, EMPTY});
        }
    };

    Square push_single{square.row - 1, square.col};
    Square push_double{square.row - 2, square.col};
    Square diagonal_left{square.row - 1, square.col - 1};
    Square diagonal_right{square.row - 1, square.col + 1};

    if (on_board(push_single.row, push_single.col) && game.board.get_piece(push_single) == EMPTY) {
        add_pawn_move(push_single, EMPTY);

        if (square.row == 6 && game.board.get_piece(push_double) == EMPTY) {
            legal_moves.push(Move{square, push_double, EMPTY, EMPTY, 
                                       false, false, false, false, false, true});
        }
    }

    if (on_board(diagonal_left.row, diagonal_left.col)) {
        PieceName target_piece = game.board.get_piece(diagonal_left);
        if (get_piece_color(target_piece) == WHITE) {
            add_pawn_move(diagonal_left, target_piece);
        }
        // En passant
        // En passant
        else if (game.en_passant_possible && square.row == 3 && game.en_passant_col == square.col - 1) {
            legal_moves.push(Move{square, diagonal_left, WHITE_PAWN, EMPTY, false, false, false, false, false, false, true});
        }
    }

    if (on_board(diagonal_right.row, diagonal_right.col)) {
        PieceName target_piece = game.board.get_piece(diagonal_right);
        if (get_piece_color(target_piece) == WHITE) {
            add_pawn_move(diagonal_right, target_piece);
        }
        // En passant
        else if (game.en_passant_possible && square.row == 3 && game.en_passant_col == square.col + 1) {
            legal_moves.push(Move{square, diagonal_right, WHITE_PAWN, EMPTY, false, false, false, false, false, false, true});
        }
    }
}

void get_king_moves(Chessgame& game, Square square, MoveList& legal_moves, 
                    int own_color, bool generate_castling) {
    auto on_board = [](int f, int r) { return f >= 0 && f < 8 && r >= 0 && r < 8; };

    int f_offset[] = {1, 1, 1, 0, 0, -1, -1, -1};
    int r_offset[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    
    for (size_t i = 0; i < 8; i++) {
        int target_row = square.row + f_offset[i];
        int target_col = square.col + r_offset[i];

        if (!on_board(target_row, target_col)) continue;

        Square target{target_row, target_col};

        if (get_piece_color(game.board.get_piece(target)) != own_color) {
            legal_moves.push({square, target});
        }
    }

    bool own_king_moved = game.turn == WHITE ? game.white_king_moved : game.black_king_moved;

    if (generate_castling && !own_king_moved) {
        int own_row = (own_color == WHITE) ? 0 : 7;
        Square my_king_square = {own_row, 4};

        // Queen side castling
        bool own_a_rook_moved = game.turn == WHITE ? game.white_a_rook_moved : game.black_a_rook_moved;
        if (!own_a_rook_moved) {

            if (game.board.get_piece({own_row, 1}) == EMPTY && 
                game.board.get_piece({own_row, 2}) == EMPTY && 
                game.board.get_piece({own_row, 3}) == EMPTY) {
                
                // Check if the king is currently in check or if the squares it moves to are attacked
                if (!is_square_attacked(game, {own_row, 4}, own_color) && 
                    !is_square_attacked(game, {own_row, 3}, own_color) && 
                    !is_square_attacked(game, {own_row, 2}, own_color)) {
                    
                    legal_moves.push({my_king_square, Square{own_row, 2}, EMPTY, EMPTY, false, true});
                }
            }
        }

        // King side castling
        bool own_h_rook_moved = (own_color == WHITE) ? game.white_h_rook_moved : game.black_h_rook_moved;
        if (!own_h_rook_moved) {
            // Check if path is empty
            if (game.board.get_piece({own_row, 6}) == EMPTY && 
                game.board.get_piece({own_row, 5}) == EMPTY) {
                
                // Check if the king is currently in check, or if the squares it moves to are attacked
                if (!is_square_attacked(game, {own_row, 4}, own_color) &&
                    !is_square_attacked(game, {own_row, 5}, own_color) && 
                    !is_square_attacked(game, {own_row, 6}, own_color)) {
                    
                    legal_moves.push({my_king_square, Square{own_row, 6}, EMPTY, EMPTY, true, false});
                }
            }
        }
    }
}

void get_legal_moves_white_king(Chessgame& game, Square square, MoveList& legal_moves, bool generate_castling) {
    get_king_moves(game, square, legal_moves, WHITE, generate_castling);
}

void get_legal_moves_black_king(Chessgame& game, Square square, MoveList& legal_moves, bool generate_castling) {
    get_king_moves(game, square, legal_moves, BLACK, generate_castling);
}

void get_knight_moves(Chessgame& game, Square square, MoveList& legal_moves, int own_color) {
    auto on_board = [](int f, int r) { return f >= 0 && f < 8 && r >= 0 && r < 8; };

    int f_offset[] = {2, 2, 1, 1, -1, -1, -2, -2};
    int r_offset[] = {1, -1, 2, -2, 2, -2, 1, -1};
    
    for (size_t i = 0; i < 8; i++) {
        int target_row = square.row + f_offset[i];
        int target_col = square.col + r_offset[i];

        if (!on_board(target_row, target_col)) continue;

        Square target{target_row, target_col};

        if (get_piece_color(game.board.get_piece(target)) != own_color) {
            legal_moves.push({square, target});
        }
    }
}

void get_legal_moves_white_knight(Chessgame& game, Square square, MoveList& legal_moves) {
    get_knight_moves(game, square, legal_moves, WHITE);
}

void get_legal_moves_black_knight(Chessgame& game, Square square, MoveList& legal_moves) {
    get_knight_moves(game, square, legal_moves, BLACK);
}

void get_sliding_moves(Chessgame& game, Square square, MoveList& legal_moves, 
                       const int dr[], const int dc[], int count, int own_color) {
    auto on_board = [](int f, int r) { return f >= 0 && f < 8 && r >= 0 && r < 8; };

    for (size_t i = 0; i < count; i++) {
        int cur_f = square.row + dr[i];
        int cur_r = square.col + dc[i];

        while (on_board(cur_f, cur_r)) {
            Square target{cur_f, cur_r};
            PieceName target_piece = game.board.get_piece(target);

            if (target_piece != EMPTY) {
                if (get_piece_color(target_piece) != own_color) {
                    legal_moves.push({square, target});
                }
                break;
            }

            legal_moves.push({square, target}); // Empty square
            cur_f += dr[i];
            cur_r += dc[i];
        }
    }
}

void get_legal_moves_white_rook(Chessgame& game, Square square, MoveList& legal_moves) {
    int dr[] = {1, -1, 0, 0};
    int dc[] = {0, 0, 1, -1};
    get_sliding_moves(game, square, legal_moves, dr, dc, 4, WHITE);
}

void get_legal_moves_black_rook(Chessgame& game, Square square, MoveList& legal_moves) {
    int dr[] = {1, -1, 0, 0};
    int dc[] = {0, 0, 1, -1};
    get_sliding_moves(game, square, legal_moves, dr, dc, 4, BLACK);
}

void get_legal_moves_white_bishop(Chessgame& game, Square square, MoveList& legal_moves) {
    int dr[] = {1, -1, 1, -1};
    int dc[] = {1, 1, -1, -1};
    get_sliding_moves(game, square, legal_moves, dr, dc, 4, WHITE);
}

void get_legal_moves_black_bishop(Chessgame& game, Square square, MoveList& legal_moves) {
    int dr[] = {1, -1, 1, -1};
    int dc[] = {1, 1, -1, -1};
    get_sliding_moves(game, square, legal_moves, dr, dc, 4, BLACK);
}

void get_legal_moves_white_queen(Chessgame& game, Square square, MoveList& legal_moves) {
    int dr[] = {1, -1, 0, 0, 1, -1, 1, -1};
    int dc[] = {0, 0, 1, -1, 1, 1, -1, -1};
    get_sliding_moves(game, square, legal_moves, dr, dc, 8, WHITE);
}

void get_legal_moves_black_queen(Chessgame& game, Square square, MoveList& legal_moves) {
    int dr[] = {1, -1, 0, 0, 1, -1, 1, -1};
    int dc[] = {0, 0, 1, -1, 1, 1, -1, -1};
    get_sliding_moves(game, square, legal_moves, dr, dc, 8, BLACK);
}

void make_move(Chessgame& game, Move& move) {
    PieceName moving_piece = game.board.get_piece(move.from);
    PieceName target_piece = game.board.get_piece(move.to);

    if (!move.en_passant) {
        move.captured_piece = target_piece;
    }

    if (game.en_passant_possible) {
        move.en_passant_was_possible = game.en_passant_possible;
        move.previous_en_passant_col = game.en_passant_col;
    }

    if (target_piece == WHITE_ROOK) {
        if (move.to == Square{0, 0} && !game.white_a_rook_moved) {
            game.white_a_rook_moved = true;
            move.captured_a_rook = true;
        }
        else if (move.to == Square{0, 7} && !game.white_h_rook_moved) {
            game.white_h_rook_moved = true;
            move.captured_h_rook = true;
        }
    }
    else if (target_piece == BLACK_ROOK) {
        if (move.to == Square{7, 0} && !game.black_a_rook_moved) {
            game.black_a_rook_moved = true;
            move.captured_a_rook = true;
        }
        else if (move.to == Square{7, 7} && !game.black_h_rook_moved) {
            game.black_h_rook_moved = true;
            move.captured_h_rook = true;
        }
    }

    game.board.move_piece(move.from, move.to, moving_piece);

    // Promotion
    if (move.promotion != EMPTY) {
        game.board.place_piece(move.to, move.promotion);
    }

    if (moving_piece == WHITE_KING) {
        game.white_king_pos = move.to;

        if (!game.white_king_moved) {
            game.white_king_moved = true;
            move.first_move_king = true;
        }

        if (move.castle_queenside) {
            game.white_castled = true;
            move.first_move_king = true;
            game.board.move_piece(Square{0, 0}, Square{0, 3}, WHITE_ROOK);
        }
        else if (move.castle_kingside) {
            game.white_castled = true;
            move.first_move_king = true;
            game.board.move_piece(Square{0, 7}, Square{0, 5}, WHITE_ROOK);
        }
    }

    else if (moving_piece == BLACK_KING) {
        game.black_king_pos = move.to;

        if (!game.black_king_moved) {
            game.black_king_moved = true;
            move.first_move_king = true;
        }

        if (move.castle_queenside) {
            game.black_castled = true;
            move.first_move_king = true;
            game.board.move_piece(Square{7, 0}, Square{7, 3}, BLACK_ROOK);
        }
        else if (move.castle_kingside) {
            game.black_castled = true;
            move.first_move_king = true;
            game.board.move_piece(Square{7, 7}, Square{7, 5}, BLACK_ROOK);
        }
    }

    else if (moving_piece == WHITE_ROOK) {
        if (!game.white_a_rook_moved && move.from == Square{0, 0}) {
            game.white_a_rook_moved = true;
            move.first_move_a_rook = true;
        }
        else if (!game.white_h_rook_moved && move.from == Square{0, 7}) {
            game.white_h_rook_moved = true;
            move.first_move_h_rook = true;
        }
    }

    else if (moving_piece == BLACK_ROOK) {
        if (!game.black_a_rook_moved && move.from == Square{7, 0}) {
            game.black_a_rook_moved = true;
            move.first_move_a_rook = true;
        }
        else if (!game.black_h_rook_moved && move.from == Square{7, 7}) {
            game.black_h_rook_moved = true;
            move.first_move_h_rook = true;
        }
    }

    // En passant
    if (move.en_passant) {
        Square en_passant_square = moving_piece == WHITE_PAWN ? Square{move.to.row - 1, move.to.col} : 
                                                                Square{move.to.row + 1, move.to.col};
        game.board.remove_piece(en_passant_square);
    }

    // Set en passant state for next turn
    game.en_passant_possible = move.double_pawn_push;
    if (move.double_pawn_push) {
        game.en_passant_col = move.from.col;
    }

    game.swap_turn();
}

void unmake_move(Chessgame& game, Move& move) {
    game.swap_turn();

    PieceName moving_piece = game.board.get_piece(move.to);

    // Promotion reversal
    if (move.promotion != EMPTY) {
        moving_piece = (game.turn == WHITE) ? WHITE_PAWN : BLACK_PAWN; 
    }

    PieceName target_piece = move.captured_piece;

    game.board.move_piece(move.to, move.from, moving_piece);

    if (moving_piece == WHITE_KING) {
        game.white_king_pos = move.from;
        if (move.first_move_king) {
            game.white_king_moved = false;
        }
        if (move.castle_queenside) {
            game.board.move_piece(Square{0, 3}, Square{0, 0}, WHITE_ROOK);
        }

        else if (move.castle_kingside) {
            game.board.move_piece(Square{0, 5}, Square{0, 7}, WHITE_ROOK);
        }
    }

    else if (moving_piece == BLACK_KING) {
        game.black_king_pos = move.from;
        if (move.first_move_king) {
            game.black_king_moved = false;
        }
        if (move.castle_queenside) {
            game.board.move_piece(Square{7, 3}, Square{7, 0}, BLACK_ROOK);
        }

        else if (move.castle_kingside) {
            game.board.move_piece(Square{7, 5}, Square{7, 7}, BLACK_ROOK);
        }
    }

    else if (moving_piece == WHITE_ROOK) {
        if (move.first_move_a_rook) {
            game.white_a_rook_moved = false;
        }
        else if (move.first_move_h_rook) {
            game.white_h_rook_moved = false;
        }
    }

    else if (moving_piece == BLACK_ROOK) {
        if (move.first_move_a_rook) {
            game.black_a_rook_moved = false;
        }
        else if (move.first_move_h_rook) {
            game.black_h_rook_moved = false;
        }
    }

    // En passant
    if (move.en_passant) {
        Square en_passant_square = moving_piece == WHITE_PAWN ? Square{move.to.row - 1, move.to.col} : 
                                                                Square{move.to.row + 1, move.to.col};
        PieceName en_passant_piece = moving_piece == WHITE_PAWN ? BLACK_PAWN : WHITE_PAWN;

        game.board.place_piece(en_passant_square, en_passant_piece);
    }

    else if (target_piece != EMPTY) {
        game.board.place_piece(move.to, target_piece);

        if (move.captured_a_rook) {
            if (game.turn == WHITE) game.black_a_rook_moved = false;
            else game.white_a_rook_moved = false;
        }
        else if (move.captured_h_rook) {
            if (game.turn == WHITE) game.black_h_rook_moved = false;
            else game.white_h_rook_moved = false;
        }
    }

    game.en_passant_possible = move.en_passant_was_possible;
    game.en_passant_col = move.previous_en_passant_col;
}

void filter_legal_moves(Chessgame& game, MoveList& psuedo_moves) {
    MoveList strictly_legal_moves;

    for (Move& move : psuedo_moves) {

        int my_color = game.turn;

        make_move(game, move);
        
        Square my_king_square = my_color == WHITE ? game.white_king_pos : game.black_king_pos;

        bool in_check = is_square_attacked(game, my_king_square, my_color);

        // Keep move if king is safe
        if (!in_check) {
            strictly_legal_moves.push(move);
        }

        unmake_move(game, move);

    }
    psuedo_moves = strictly_legal_moves;
}

bool is_square_attacked(Chessgame& game, Square square, int own_color) {
    auto on_board = [](int f, int r) { return f >= 0 && f < 8 && r >= 0 && r < 8; };

    PieceName enemy_king   = (own_color == WHITE) ? BLACK_KING : WHITE_KING;
    PieceName enemy_pawn   = (own_color == WHITE) ? BLACK_PAWN : WHITE_PAWN;
    PieceName enemy_knight = (own_color == WHITE) ? BLACK_KNIGHT : WHITE_KNIGHT;
    PieceName enemy_bishop = (own_color == WHITE) ? BLACK_BISHOP : WHITE_BISHOP;
    PieceName enemy_rook   = (own_color == WHITE) ? BLACK_ROOK : WHITE_ROOK;
    PieceName enemy_queen  = (own_color == WHITE) ? BLACK_QUEEN : WHITE_QUEEN;

    // Check if opponent king attacks square
    int f_offset[] = {1, 1, 1, 0, 0, -1, -1, -1};
    int r_offset[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    for (size_t i = 0; i < 8; i++) {
        int target_row = square.row + f_offset[i];
        int target_col = square.col + r_offset[i];

        if (on_board(target_row, target_col) && 
            game.board.get_piece({target_row, target_col}) == enemy_king) return true;
    }

    // Check if pawn attacks square
    int pawn_row_offset = (own_color == WHITE) ? 1 : -1;
    if (on_board(square.row + pawn_row_offset, square.col - 1) && 
        game.board.get_piece({square.row + pawn_row_offset, square.col - 1}) == enemy_pawn) return true;
        
    if (on_board(square.row + pawn_row_offset, square.col + 1) && 
        game.board.get_piece({square.row + pawn_row_offset, square.col + 1}) == enemy_pawn) return true;

    // Check if knight attacks square
    int fn_offset[] = {2, 2, 1, 1, -1, -1, -2, -2};
    int rn_offset[] = {1, -1, 2, -2, 2, -2, 1, -1};

    for (size_t i = 0; i < 8; i++) {
        int target_row = square.row + fn_offset[i];
        int target_col = square.col + rn_offset[i];

        if (on_board(target_row, target_col) && 
            game.board.get_piece({target_row, target_col}) == enemy_knight) return true;
    }

    // Check for sliding pieces
    int dr[] = {1, -1, 0, 0, 1, -1, 1, -1};
    int dc[] = {0, 0, 1, -1, 1, 1, -1, -1};

    for (size_t i = 0; i < 8; i++) {
        int cur_f = square.row + dr[i];
        int cur_r = square.col + dc[i];

        while (on_board(cur_f, cur_r)) {
            PieceName target_piece = game.board.get_piece({cur_f, cur_r});

            if (target_piece != EMPTY) {
                if (i <= 3) {
                    // Straight lines: attacking piece must be Rook or Queen
                    if (target_piece == enemy_rook || target_piece == enemy_queen) return true;
                } else {
                    // Diagonal lines: attacking piece must be Bishop or Queen
                    if (target_piece == enemy_bishop || target_piece == enemy_queen) return true;
                }
                
                // If we reach here it means we hit a friendly piece or an enemy piece that 
                // doesn't attack in this direction. Break the while loop
                break; 
            }

            cur_f += dr[i];
            cur_r += dc[i];
        }
    }
    
    return false;
}

uint64_t perft(Chessgame& game, int depth) {
    if (depth == 0) return 1ULL;

    MoveList psudeo_moves;
    get_psuedo_moves(game, psudeo_moves);
    filter_legal_moves(game, psudeo_moves);

    if (depth == 1) return psudeo_moves.size();

    uint64_t nodes = 0;

    for (Move& move : psudeo_moves) {
        make_move(game, move);
        nodes += perft(game, depth - 1);
        unmake_move(game, move);
    }

    return nodes;
}