#include <iostream>
#include <random>
#include <chrono>
#include <string>
#include <map>
#include <future>

#include "helpers.h"
#include "moves.h"
#include "raylib.h"

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

int run_terminal(int player_color) {
    init_zobrist();

    NNUE chess_evaluator(772, 1024);

    if (!chess_evaluator.load_weights("../../machine_learning/nnue_weights1024_huber_perspective.bin")) {
        return -1;
    }

    Chessgame ai_game(DEFAULT_BOARD);

    while(true) {
        ai_game.board.display(player_color);
        
        if ((int)ai_game.turn == player_color) {
            MoveList pseudo_moves;
            get_psuedo_moves(ai_game, pseudo_moves);
            filter_legal_moves(ai_game, pseudo_moves);

            if (pseudo_moves.count == 0) {
                cout << "Checkmate!" << endl;
                break;
            }

            string x;
            cout << "Type your next move: ";
            cin >> x;

            Move translated_move = translate_move(x);

            if (pseudo_moves.contains(translated_move)) {
                Move actual_move = pseudo_moves.find_match(translated_move);
                make_move(ai_game, actual_move);
            }
            else {
                cout << "\nMove not possible, try again with another move." << endl;
            }
        }
        else {
            auto start_time = chrono::high_resolution_clock::now();
    
            Move best_move = get_best_move(ai_game, 4, chess_evaluator);
            
            auto end_time = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end_time - start_time;

            cout << "\nBest Move Found: ";
            print_move(best_move);
            cout << "Time to think: " << elapsed.count() << " seconds\n" << endl;

            make_move(ai_game, best_move);
        }
    }

    return 0;
}

void run_gui(int player_color) {
    init_zobrist();

    bool play_against_ai = true; 
    
    NNUE chess_evaluator(772, 1024);
    if (play_against_ai) {
        if (!chess_evaluator.load_weights("../../machine_learning/nnue_weights1024_huber_perspective.bin")) {
            cout << "Failed to load NNUE weights! Please check the path." << endl;
            return;
        }
    }

    Chessgame ai_game(DEFAULT_BOARD);
    const int screenWidth = 1600;
    const int screenHeight = 1600;

    InitWindow(screenWidth, screenHeight, "chess-game");
    SetTargetFPS(144);

    std::map<PieceName, Texture2D> pieceTextures;
    auto load = [&](PieceName name, std::string filename) {
        pieceTextures[name] = LoadTexture(("../../images/" + filename).c_str());
    };

    load(WHITE_PAWN,   "Chess_plt45.png");
    load(WHITE_KNIGHT, "Chess_nlt45.png");
    load(WHITE_BISHOP, "Chess_blt45.png");
    load(WHITE_ROOK,   "Chess_rlt45.png");
    load(WHITE_QUEEN,  "Chess_qlt45.png");
    load(WHITE_KING,   "Chess_klt45.png");
    load(BLACK_PAWN,   "Chess_pdt45.png");
    load(BLACK_KNIGHT, "Chess_ndt45.png");
    load(BLACK_BISHOP, "Chess_bdt45.png");
    load(BLACK_ROOK,   "Chess_rdt45.png");
    load(BLACK_QUEEN,  "Chess_qdt45.png");
    load(BLACK_KING,   "Chess_kdt45.png");

    Square selectedSquare = {-1, -1};
    PieceName selectedPiece = EMPTY;
    bool isDragging = false; 
    
    Square lastMoveFrom = {-1, -1};
    Square lastMoveTo = {-1, -1};
    
    MoveList current_legal_moves;
    std::vector<int> highlighted_squares;
    
    bool ai_thinking = false;
    std::future<Move> ai_future;
    std::chrono::time_point<std::chrono::high_resolution_clock> ai_start_time;

    auto get_board_square = [&](int mX, int mY, int sWidth, int sHeight) -> Square {
        if (mX < 0 || mX >= GetScreenWidth() || mY < 0 || mY >= GetScreenHeight()) {
            return {-1, -1};
        }
        
        int visualCol = mX / sWidth;
        int visualRow = mY / sHeight; 
        
        int actualCol = (player_color == CHESS_WHITE) ? visualCol : (7 - visualCol);
        int actualRow = (player_color == CHESS_WHITE) ? (7 - visualRow) : visualRow;
        
        return {actualRow, actualCol};
    };

    while (!WindowShouldClose()) {
        int sw = GetScreenWidth() / 8;
        int sh = GetScreenHeight() / 8;
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();

        // Update logic
        if (play_against_ai && (int)ai_game.turn != player_color) {
            if (!ai_thinking) {
                ai_thinking = true;
                ai_start_time = std::chrono::high_resolution_clock::now();
                Chessgame game_copy = ai_game;
                
                ai_future = std::async(std::launch::async, [game_copy, &chess_evaluator]() mutable {
                    return get_best_move(game_copy, 6, chess_evaluator);
                });
            }
            else if (ai_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                Move best_move = ai_future.get();
                auto end_time = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = end_time - ai_start_time;

                cout << "AI Played: ";
                print_move(best_move);
                cout << "Time to think: " << elapsed.count() << " seconds\n" << endl;

                make_move(ai_game, best_move);
                lastMoveFrom = best_move.from;
                lastMoveTo = best_move.to;
                ai_thinking = false;
            }
        }
        else {
            // Human input logic
            if (isDragging && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                isDragging = false;
                selectedSquare = {-1, -1};
                highlighted_squares.clear();
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Square clicked_square = get_board_square(mouseX, mouseY, sw, sh);
                
                if (clicked_square.row != -1) { // Make sure click was valid
                    PieceName piece = ai_game.board.get_piece(clicked_square);
                    bool clicked_friendly = (piece != EMPTY && get_piece_color(piece) == (int)ai_game.turn);
                    int current_index = clicked_square.row * 8 + clicked_square.col;

                    if (selectedSquare.row != -1 && std::find(highlighted_squares.begin(), highlighted_squares.end(), current_index) != highlighted_squares.end()) {
                        Move attempted_move = { selectedSquare, clicked_square };
                        Move actual_move = current_legal_moves.find_match(attempted_move);
                        
                        make_move(ai_game, actual_move);
                        lastMoveFrom = actual_move.from;
                        lastMoveTo = actual_move.to;

                        selectedSquare = {-1, -1};
                        highlighted_squares.clear();
                        isDragging = false; 
                    }
                    else if (clicked_friendly) {
                        if (clicked_square == selectedSquare) {
                            selectedSquare = {-1, -1};
                            highlighted_squares.clear();
                            isDragging = false;
                        } 
                        else {
                            selectedSquare = clicked_square;
                            selectedPiece = piece;
                            isDragging = true; 

                            current_legal_moves.count = 0; 
                            get_psuedo_moves(ai_game, current_legal_moves);
                            filter_legal_moves(ai_game, current_legal_moves);
                            highlighted_squares = current_legal_moves.get_possible_piece_moves(selectedSquare);
                        }
                    }
                    else {
                        selectedSquare = {-1, -1};
                        highlighted_squares.clear();
                        isDragging = false;
                    }
                }
            }

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                if (isDragging) {
                    Square drop_square = get_board_square(mouseX, mouseY, sw, sh);

                    if (drop_square == selectedSquare) {
                        isDragging = false;
                    }
                    else if (drop_square.row != -1) {
                        Move attempted_move = { selectedSquare, drop_square };

                        if (current_legal_moves.contains(attempted_move)) {
                            Move actual_move = current_legal_moves.find_match(attempted_move);
                            make_move(ai_game, actual_move);
                            
                            lastMoveFrom = actual_move.from;
                            lastMoveTo = actual_move.to;
                        }

                        isDragging = false;
                        selectedSquare = {-1, -1};
                        highlighted_squares.clear();
                    }
                }
            }
        }

        // Draw logic
        BeginDrawing();
            ClearBackground(RAYWHITE);

            Color lightSquare = Color{235, 236, 208, 255};
            Color darkSquare  = Color{115, 149, 82, 255};
            Color highlightLight = Color{245, 246, 130, 255};
            Color highlightDark  = Color{185, 202, 67, 255};

            for (int i = 0; i < 64; i++) {
                int row = i / 8;
                int col = i % 8;
                
                int drawRow = (player_color == CHESS_WHITE) ? (7 - row) : row;
                int drawCol = (player_color == CHESS_WHITE) ? col : (7 - col);

                bool isLight = ((row + col) % 2 != 0);
                Color squareColor = isLight ? lightSquare : darkSquare;

                bool isLastMove = (row == lastMoveFrom.row && col == lastMoveFrom.col) ||
                                  (row == lastMoveTo.row && col == lastMoveTo.col);
                bool isSelected = (row == selectedSquare.row && col == selectedSquare.col);

                if (isLastMove || isSelected) {
                    squareColor = isLight ? highlightLight : highlightDark;
                }

                DrawRectangle(drawCol * sw, drawRow * sh, sw, sh, squareColor);

                PieceName piece = ai_game.board.get_piece({row, col});
                if (piece != EMPTY) {
                    bool isBeingDragged = isDragging && (row == selectedSquare.row && col == selectedSquare.col);
                    
                    if (!isBeingDragged) {
                        Texture2D txt = pieceTextures[piece];
                        Rectangle source = { 0.0f, 0.0f, (float)txt.width, (float)txt.height };
                        Rectangle dest = { (float)drawCol * sw, (float)drawRow * sh, (float)sw, (float)sh };
                        DrawTexturePro(txt, source, dest, { 0, 0 }, 0.0f, WHITE);
                    }
                }

                int current_index = row * 8 + col;
                if (selectedSquare.row != -1 && std::find(highlighted_squares.begin(), highlighted_squares.end(), current_index) != highlighted_squares.end()) {
                    
                    float centerX = drawCol * sw + (sw / 2.0f);
                    float centerY = drawRow * sh + (sh / 2.0f);
                    Color dotColor = Color{0, 0, 0, 60};

                    if (piece != EMPTY) {
                        DrawRing(Vector2{centerX, centerY}, sw * 0.38f, sw * 0.45f, 0, 360, 36, dotColor);
                    } else {
                        DrawCircle((int)centerX, (int)centerY, sw * 0.15f, dotColor);
                    }
                }
            }

            if (isDragging) {
                Texture2D txt = pieceTextures[selectedPiece];
                Rectangle source = { 0.0f, 0.0f, (float)txt.width, (float)txt.height };
                Rectangle dest = { (float)mouseX - (sw / 2.0f), (float)mouseY - (sh / 2.0f), (float)sw, (float)sh };
                DrawTexturePro(txt, source, dest, { 0, 0 }, 0.0f, WHITE);
            }

        EndDrawing();
    }

    for (auto const& [name, texture] : pieceTextures) {
        UnloadTexture(texture);
    }
    
    CloseWindow();
}

int main() {
    cout << "Choose your color:" << endl;
    cout << "1. White" << endl;
    cout << "2. Black" << endl;
    cout << "Choice: ";
    
    int colorChoice;
    cin >> colorChoice;
    int player_color = (colorChoice == 2) ? CHESS_BLACK : CHESS_WHITE;

    cout << "\nChoose display mode:" << endl;
    cout << "1. Terminal" << endl;
    cout << "2. GUI" << endl;
    cout << "Choice: ";
    
    int modeChoice;
    cin >> modeChoice;

    if (modeChoice == 1) {
        run_terminal(player_color);
    } else if (modeChoice == 2) {
        run_gui(player_color);
    } else {
        cout << "Invalid choice. Exiting..." << endl;
    }

    return 0;
}