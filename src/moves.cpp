#include <algorithm>
#include <map>

#include "moves.h"

using namespace std;

vector<Move> get_psuedo_moves(Chessgame& game, bool generate_castling) {
    vector<Move> legal_moves;
    if (game.turn == WHITE) get_legal_moves_white(game, legal_moves, generate_castling);
    else get_legal_moves_black(game, legal_moves, generate_castling);

    return legal_moves;
}

void get_legal_moves_white(Chessgame& game, vector<Move>& legal_moves, bool generate_castling) {
    for (Square square : game.white_pieces) {
        switch (game.board.get_piece(square)) {
            case WHITE_PAWN: 
                get_legal_moves_white_pawn(game, square, legal_moves);
                break;
            case WHITE_KNIGHT: 
                get_legal_moves_white_knight(game, square, legal_moves);
                break;
            case WHITE_BISHOP: 
                get_legal_moves_white_bishop(game, square, legal_moves);
                break;
            case WHITE_ROOK: 
                get_legal_moves_white_rook(game, square, legal_moves);
                break;
            case WHITE_QUEEN: 
                get_legal_moves_white_queen(game, square, legal_moves);
                break;
            case WHITE_KING: 
                get_legal_moves_white_king(game, square, legal_moves, generate_castling);
                break;
            default:
                break;
        }
    }
}

void get_legal_moves_black(Chessgame& game,  vector<Move>& legal_moves, bool generate_castling) {
        for (Square square : game.black_pieces) {
        switch (game.board.get_piece(square)) {
            case BLACK_PAWN: 
                get_legal_moves_black_pawn(game, square, legal_moves);
                break;
            case BLACK_KNIGHT: 
                get_legal_moves_black_knight(game, square, legal_moves);
                break;
            case BLACK_BISHOP: 
                get_legal_moves_black_bishop(game, square, legal_moves);
                break;
            case BLACK_ROOK: 
                get_legal_moves_black_rook(game, square, legal_moves);
                break;
            case BLACK_QUEEN: 
                get_legal_moves_black_queen(game, square, legal_moves);
                break;
            case BLACK_KING: 
                get_legal_moves_black_king(game, square, legal_moves, generate_castling);
                break;
            default:
                break;
        }
    }
}

void get_legal_moves_white_pawn(Chessgame& game, Square square, vector<Move>& legal_moves) {
    auto on_board = [](int f, int r) { return f >= 0 && f < 8 && r >= 0 && r < 8; };

    auto add_pawn_move = [&](Square target, PieceName captured_piece) {
        if (target.row == 7) {
            legal_moves.push_back(Move{square, target, captured_piece, WHITE_QUEEN});
            legal_moves.push_back(Move{square, target, captured_piece, WHITE_ROOK});
            legal_moves.push_back(Move{square, target, captured_piece, WHITE_BISHOP});
            legal_moves.push_back(Move{square, target, captured_piece, WHITE_KNIGHT});
        } else {
            legal_moves.push_back(Move{square, target, captured_piece, EMPTY});
        }
    };

    Square push_single{square.row + 1, square.col};
    Square push_double{square.row + 2, square.col};
    Square diagonal_left{square.row + 1, square.col + 1};
    Square diagonal_right{square.row + 1, square.col - 1};

    if (on_board(push_single.row, push_single.col) && game.board.get_piece(push_single) == EMPTY) {
        add_pawn_move(push_single, EMPTY);

        if (square.row == 1 && game.board.get_piece(push_double) == EMPTY) {
        legal_moves.push_back(Move{square, push_double, EMPTY, EMPTY, 
                                   false, false, false, false, false, true});
        }
    }

    if (on_board(diagonal_left.row, diagonal_left.col)) {
        if (find(game.black_pieces.begin(), game.black_pieces.end(), diagonal_left) != game.black_pieces.end()) {
            PieceName captured = game.board.get_piece(diagonal_left);
            add_pawn_move(diagonal_left, captured);
        }
        // En passant
        else if (game.en_passant_possible && square.row == 4 && game.en_passant_col == square.col + 1) {
            legal_moves.push_back(Move{square, diagonal_left, BLACK_PAWN, EMPTY, false, false, false, false, false, false, true});
        }
    }

    if (on_board(diagonal_right.row, diagonal_right.col)) {
        if (find(game.black_pieces.begin(), game.black_pieces.end(), diagonal_right) != game.black_pieces.end()) {
            PieceName captured = game.board.get_piece(diagonal_right);
            add_pawn_move(diagonal_right, captured);
        }
        // En passant
        else if (game.en_passant_possible && square.row == 4 && game.en_passant_col == square.col - 1) {
            legal_moves.push_back(Move{square, diagonal_right, BLACK_PAWN, EMPTY, false, false, false, false, false, false, true});
        }
    }
}

void get_legal_moves_black_pawn(Chessgame& game, Square square, vector<Move>& legal_moves) {
    auto on_board = [](int f, int r) { return f >= 0 && f < 8 && r >= 0 && r < 8; };

    auto add_pawn_move = [&](Square target, PieceName captured_piece) {
        if (target.row == 0) {
            legal_moves.push_back(Move{square, target, captured_piece, BLACK_QUEEN});
            legal_moves.push_back(Move{square, target, captured_piece, BLACK_ROOK});
            legal_moves.push_back(Move{square, target, captured_piece, BLACK_BISHOP});
            legal_moves.push_back(Move{square, target, captured_piece, BLACK_KNIGHT});
        } else {
            legal_moves.push_back(Move{square, target, captured_piece, EMPTY});
        }
    };

    Square push_single{square.row - 1, square.col};
    Square push_double{square.row - 2, square.col};
    Square diagonal_left{square.row - 1, square.col - 1};
    Square diagonal_right{square.row - 1, square.col + 1};

    if (on_board(push_single.row, push_single.col) && game.board.get_piece(push_single) == EMPTY) {
        add_pawn_move(push_single, EMPTY);

        if (square.row == 6 && game.board.get_piece(push_double) == EMPTY) {
            legal_moves.push_back(Move{square, push_double, EMPTY, EMPTY, 
                                       false, false, false, false, false, true});
        }
    }

    if (on_board(diagonal_left.row, diagonal_left.col)) {
        if (find(game.white_pieces.begin(), 
            game.white_pieces.end(), diagonal_left) != game.white_pieces.end()) {
            PieceName captured = game.board.get_piece(diagonal_left);
            add_pawn_move(diagonal_left, captured);
        }
        // En passant
        else if (game.en_passant_possible && square.row == 3 && game.en_passant_col == square.col - 1) {
            legal_moves.push_back(Move{square, diagonal_left, WHITE_PAWN, EMPTY, false, false, false, false, false, false, true});
        }
    }

    if (on_board(diagonal_right.row, diagonal_right.col)) {
        if (find(game.white_pieces.begin(), 
            game.white_pieces.end(), diagonal_right) != game.white_pieces.end()) {
            PieceName captured = game.board.get_piece(diagonal_right);
            add_pawn_move(diagonal_right, captured);
        }
        // En passant
        else if (game.en_passant_possible && square.row == 3 && game.en_passant_col == square.col + 1) {
            legal_moves.push_back(Move{square, diagonal_right, WHITE_PAWN, EMPTY, false, false, false, false, false, false, true});
        }
    }
}

void get_king_moves(Chessgame& game, Square square, vector<Move>& legal_moves, 
                    const vector<Square>& own_side, bool generate_castling) {
    auto on_board = [](int f, int r) { return f >= 0 && f < 8 && r >= 0 && r < 8; };

    int f_offset[] = {1, 1, 1, 0, 0, -1, -1, -1};
    int r_offset[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    
    for (size_t i = 0; i < 8; i++) {
        int target_row = square.row + f_offset[i];
        int target_col = square.col + r_offset[i];

        if (!on_board(target_row, target_col)) continue;

        Square target{target_row, target_col};

        if (find(own_side.begin(), own_side.end(), target) == own_side.end()) {
            legal_moves.push_back({square, target});
        }
    }

    bool own_king_moved = game.turn == WHITE ? game.white_king_moved : game.black_king_moved;

    if (generate_castling && !own_king_moved) {
        // Queen side castling
        bool own_a_rook_moved = game.turn == WHITE ? game.white_a_rook_moved : game.black_a_rook_moved;
        if (!own_a_rook_moved) {
            Square my_king_square = (game.turn == WHITE) ? game.white_king_pos : game.black_king_pos;

            int own_row = game.turn == WHITE ? 0 : 7;

            if (game.board.get_piece({own_row, 1}) == EMPTY && game.board.get_piece({own_row, 2}) == EMPTY && game.board.get_piece({own_row, 3}) == EMPTY) {
                game.swap_turn();
                vector<Move> enemy_moves = get_psuedo_moves(game, false);
                game.swap_turn();

                bool castable = true;
                for (const Move& enemy_move : enemy_moves) {
                    if (enemy_move.to == Square{own_row, 2} || enemy_move.to == Square{own_row, 3} || enemy_move.to == Square{own_row, 4}) {
                        castable = false;
                        break;
                    }
                }

                if (castable) {
                    legal_moves.push_back({my_king_square, Square{own_row, 2}, EMPTY, EMPTY, false, true});
                }
            }
        }

        // King side castling
        bool own_h_rook_moved = game.turn == WHITE ? game.white_h_rook_moved : game.black_h_rook_moved;
        if (!own_h_rook_moved) {
            Square my_king_square = (game.turn == WHITE) ? game.white_king_pos : game.black_king_pos;

            int own_row = game.turn == WHITE ? 0 : 7;

            if (game.board.get_piece({own_row, 6}) == EMPTY && game.board.get_piece({own_row, 5}) == EMPTY) {
                game.swap_turn();
                vector<Move> enemy_moves = get_psuedo_moves(game, false);
                game.swap_turn();

                bool castable = true;
                for (const Move& enemy_move : enemy_moves) {
                    if (enemy_move.to == Square{own_row, 6} || enemy_move.to == Square{own_row, 5} || enemy_move.to == Square{own_row, 4}) {
                        castable = false;
                        break;
                    }
                }

                if (castable) {
                    legal_moves.push_back({my_king_square, Square{own_row, 6}, EMPTY, EMPTY, true, false});
                }
            }
        }
    }
}

void get_legal_moves_white_king(Chessgame& game, Square square, vector<Move>& legal_moves, bool generate_castling) {
    get_king_moves(game, square, legal_moves, game.white_pieces, generate_castling);
}

void get_legal_moves_black_king(Chessgame& game, Square square, vector<Move>& legal_moves, bool generate_castling) {
    get_king_moves(game, square, legal_moves, game.black_pieces, generate_castling);
}

void get_knight_moves(Chessgame& game, Square square, vector<Move>& legal_moves, 
                      const vector<Square>& own_side) {
    auto on_board = [](int f, int r) { return f >= 0 && f < 8 && r >= 0 && r < 8; };

    int f_offset[] = {2, 2, 1, 1, -1, -1, -2, -2};
    int r_offset[] = {1, -1, 2, -2, 2, -2, 1, -1};
    
    for (size_t i = 0; i < 8; i++) {
        int target_row = square.row + f_offset[i];
        int target_col = square.col + r_offset[i];

        if (!on_board(target_row, target_col)) continue;

        Square target{target_row, target_col};

        if (find(own_side.begin(), own_side.end(), target) == own_side.end()) {
            legal_moves.push_back({square, target});
        }
    }
}

void get_legal_moves_white_knight(Chessgame& game, Square square, vector<Move>& legal_moves) {
    get_knight_moves(game, square, legal_moves, game.white_pieces);
}

void get_legal_moves_black_knight(Chessgame& game, Square square, vector<Move>& legal_moves) {
    get_knight_moves(game, square, legal_moves, game.black_pieces);
}

void get_sliding_moves(Chessgame& game, Square square, vector<Move>& legal_moves, 
                       const vector<int>& dr, const vector<int>& dc, 
                       const vector<Square>& own_side, const vector<Square>& enemy_side) {
    auto on_board = [](int f, int r) { return f >= 0 && f < 8 && r >= 0 && r < 8; };

    for (size_t i = 0; i < dr.size(); i++) {
        int cur_f = square.row + dr[i];
        int cur_r = square.col + dc[i];

        while (on_board(cur_f, cur_r)) {
            Square target{cur_f, cur_r};

            if (find(own_side.begin(), own_side.end(), target) != own_side.end()) {
                break;
            }

            legal_moves.push_back({square, target});


            if (find(enemy_side.begin(), enemy_side.end(), target) != enemy_side.end()) {
                break;
            }

            cur_f += dr[i];
            cur_r += dc[i];
        }
    }
}

void get_legal_moves_white_rook(Chessgame& game, Square square, vector<Move>& legal_moves) {
    vector<int> dr{1, -1, 0, 0};
    vector<int> dc{0, 0, 1, -1};
    get_sliding_moves(game, square, legal_moves, dr, dc, game.white_pieces, game.black_pieces);
}

void get_legal_moves_black_rook(Chessgame& game, Square square, vector<Move>& legal_moves) {
    vector<int> dr{1, -1, 0, 0};
    vector<int> dc{0, 0, 1, -1};
    get_sliding_moves(game, square, legal_moves, dr, dc, game.black_pieces, game.white_pieces);
}

void get_legal_moves_white_bishop(Chessgame& game, Square square, vector<Move>& legal_moves) {
    vector<int> dr{1, -1, 1, -1};
    vector<int> dc{1, 1, -1, -1};
    get_sliding_moves(game, square, legal_moves, dr, dc, game.white_pieces, game.black_pieces);
}

void get_legal_moves_black_bishop(Chessgame& game, Square square, vector<Move>& legal_moves) {
    vector<int> dr{1, -1, 1, -1};
    vector<int> dc{1, 1, -1, -1};
    get_sliding_moves(game, square, legal_moves, dr, dc, game.black_pieces, game.white_pieces);
}

void get_legal_moves_white_queen(Chessgame& game, Square square, vector<Move>& legal_moves) {
    vector<int> dr{1, -1, 0, 0, 1, -1, 1, -1};
    vector<int> dc{0, 0, 1, -1, 1, 1, -1, -1};
    get_sliding_moves(game, square, legal_moves, dr, dc, game.white_pieces, game.black_pieces);
}

void get_legal_moves_black_queen(Chessgame& game, Square square, vector<Move>& legal_moves) {
    vector<int> dr{1, -1, 0, 0, 1, -1, 1, -1};
    vector<int> dc{0, 0, 1, -1, 1, 1, -1, -1};
    get_sliding_moves(game, square, legal_moves, dr, dc, game.black_pieces, game.white_pieces);
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

    vector<Square>& own_side = (game.turn == WHITE) ? game.white_pieces : game.black_pieces;
    vector<Square>& enemy_side = (game.turn == WHITE) ? game.black_pieces : game.white_pieces;

    if (target_piece != EMPTY) {
        auto capture_it = find(enemy_side.begin(), enemy_side.end(), move.to);
        if (capture_it != enemy_side.end()) {
            enemy_side.erase(capture_it);
        }
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


    auto move_it = find(own_side.begin(), own_side.end(), move.from);
    if (move_it != own_side.end()) {
        *move_it = move.to;
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

            // Move rook in white_pieces vector
            auto rook_it = find(own_side.begin(), own_side.end(), Square{0, 0});
            if (rook_it != own_side.end()) {
                *rook_it = Square{0, 3};
            }
        }
        else if (move.castle_kingside) {
            game.white_castled = true;
            move.first_move_king = true;
            game.board.move_piece(Square{0, 7}, Square{0, 5}, WHITE_ROOK);

            // Move rook in white_pieces vector
            auto rook_it = find(own_side.begin(), own_side.end(), Square{0, 7});
            if (rook_it != own_side.end()) {
                *rook_it = Square{0, 5};
            }
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

            // Move rook in black_pieces vector
            auto rook_it = find(own_side.begin(), own_side.end(), Square{7, 0});
            if (rook_it != own_side.end()) {
                *rook_it = Square{7, 3};
            }
        }
        else if (move.castle_kingside) {
            game.black_castled = true;
            move.first_move_king = true;
            game.board.move_piece(Square{7, 7}, Square{7, 5}, BLACK_ROOK);

            // Move rook in black_pieces vector
            auto rook_it = find(own_side.begin(), own_side.end(), Square{7, 7});
            if (rook_it != own_side.end()) {
                *rook_it = Square{7, 5};
            }
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

        auto capture_it = find(enemy_side.begin(), enemy_side.end(), en_passant_square);
        if (capture_it != enemy_side.end()) {
            enemy_side.erase(capture_it);
        }
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

    vector<Square>& own_side = (game.turn == WHITE) ? game.white_pieces : game.black_pieces;
    vector<Square>& enemy_side = (game.turn == WHITE) ? game.black_pieces : game.white_pieces;

    auto move_it = find(own_side.begin(), own_side.end(), move.to);
    if (move_it != own_side.end()) {
        *move_it = move.from;
    }

    game.board.move_piece(move.to, move.from, moving_piece);

    if (moving_piece == WHITE_KING) {
        game.white_king_pos = move.from;
        if (move.first_move_king) {
            game.white_king_moved = false;
        }
        if (move.castle_queenside) {
            game.board.move_piece(Square{0, 3}, Square{0, 0}, WHITE_ROOK);

            // Move rook back in white_pieces vector
            auto rook_it = find(own_side.begin(), own_side.end(), Square{0, 3});
            if (rook_it != own_side.end()) {
                *rook_it = Square{0, 0};
            }
        }

        else if (move.castle_kingside) {
            game.board.move_piece(Square{0, 5}, Square{0, 7}, WHITE_ROOK);

            // Move rook back in white_pieces vector
            auto rook_it = find(own_side.begin(), own_side.end(), Square{0, 5});
            if (rook_it != own_side.end()) {
                *rook_it = Square{0, 7};
            }
        }
    }

    else if (moving_piece == BLACK_KING) {
        game.black_king_pos = move.from;
        if (move.first_move_king) {
            game.black_king_moved = false;
        }
        if (move.castle_queenside) {
            game.board.move_piece(Square{7, 3}, Square{7, 0}, BLACK_ROOK);

            // Move rook back in black_pieces vector
            auto rook_it = find(own_side.begin(), own_side.end(), Square{7, 3});
            if (rook_it != own_side.end()) {
                *rook_it = Square{7, 0};
            }
        }

        else if (move.castle_kingside) {
            game.board.move_piece(Square{7, 5}, Square{7, 7}, BLACK_ROOK);

            // Move rook back in black_pieces vector
            auto rook_it = find(own_side.begin(), own_side.end(), Square{7, 5});
            if (rook_it != own_side.end()) {
                *rook_it = Square{7, 7};
            }
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
        enemy_side.push_back(en_passant_square);
    }

    else if (target_piece != EMPTY) {
        enemy_side.push_back(move.to);
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

void filter_legal_moves(Chessgame& game, vector<Move>& psuedo_moves) {
    vector<Move> strictly_legal_moves;

    for (Move& move : psuedo_moves) {

        int my_color = game.turn;

        make_move(game, move);
        
        vector<Move> enemy_moves = get_psuedo_moves(game, false);

        bool in_check = false;

        Square my_king_square = my_color == WHITE ? game.white_king_pos : game.black_king_pos;
        for (const Move& enemy_move : enemy_moves) {
            if (enemy_move.to == my_king_square) {
                in_check = true;
                break;
            }
        }

        if (!in_check) {
            strictly_legal_moves.push_back(move);
        }

        unmake_move(game, move);

    }
    psuedo_moves = strictly_legal_moves;
}

bool is_in_check(Chessgame& game) {
    Square my_king_square = (game.turn == WHITE) ? game.white_king_pos : game.black_king_pos;

    game.swap_turn();
    vector<Move> enemy_moves = get_psuedo_moves(game, false);
    game.swap_turn();

    for (const Move& enemy_move : enemy_moves) {
        if (enemy_move.to == my_king_square) {
            return true;
        }
    }
    return false;
}

uint64_t perft(Chessgame& game, int depth) {
    if (depth == 0) return 1ULL;

    vector<Move> psudeo_moves = get_psuedo_moves(game);
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