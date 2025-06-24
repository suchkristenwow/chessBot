#include "game_manager.hpp"
#include "move_validator.hpp" 

GameManager::GameManager(const std::string& playerColor) :
    playerColor(playerColor), currentTurn("white") {}

void GameManager::start(){
    board.printBoard(); 
    while (true) {
        if (currentTurn == playerColor) {
            makePlayerMove();
        } else {
            makeComputerMove();
        }
        board.printBoard(); 

        //TO DO: Check if winner 
        if (checkGameOver()) {
            std::cout << "Game Over!\n";
            break;
        }

        if (currentTurn == "white") {
            currentTurn = "black";
        } else{
            currentTurn = "white";
        }
    }
}

void GameManager::makeComputerMove(){
    // This function is responsible for making moves for the computer 
    std::string move; 
    
    board.movePiece(move,currentTurn);
}

void GameManager::makePlayerMove(){
    std::string move;

    while (true) {
        std::cout << currentTurn <<"'s move. Enter your move in algebraic chess notation: ";
        std::cin >> move; 
        auto result = MoveValidator::isvalidMove(board,move,currentTurn);

        if (!result.valid){
            std::cout << result.reason << std::endl;
        } else {
            board.movePiece(move,currentTurn);
            break;
        }
    }
    
}

bool GameManager::gameOver_helper(const std::string& color){
    std::string opponentColor = (color == "white") ? "black" : "white";
    int kingRow = -1, kingCol = -1; 
    //trivial case if the color King has been captured, game is over 
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            auto piece = board.grid[r][c];
            if (piece && piece->name == "king" && piece->color == color) {
                kingRow = r;
                kingCol = c;
                break;
            }
        }
    }
    std::cout << "kingRow: "<< kingRow << " " << "kingCol: " << kingCol << std::endl;
    if (kingRow == -1 || kingCol == -1){
        winner = opponentColor;
        gameOver = true; 
        return true; 
    }
    
    bool InCheck = MoveValidator::isKingInCheck(board, color);
    if (InCheck){
        std::cout << color << " is in Check!" << std::endl;
    } else {
        std::cout << color << " is NOT in Check!" << std::endl;
    }

    // Check if opponent has *any* legal moves
    bool HasLegalMove = !getAllLegalMoves(color).empty();

    if (!HasLegalMove) {
        std::cout << "[checkGameOver:GameManager] Game Over! " << color << " has no legal moves!!" << std::endl; 
        board.printBoard();
        if (InCheck) {
            std::cout << "Checkmate! " << opponentColor << " wins!" << std::endl;
            winner = opponentColor; 
        } else {
            std::cout << "Stalemate! It's a draw." << std::endl;
            stalemate = true;  
        }
        gameOver = true; 
        return true;
    }

    return false;
}

bool GameManager::checkGameOver() {
    std::cout << "[checkGameOver:GameManager] Checking if game over ... " << std::endl;
    if (gameOver_helper("white") || gameOver_helper("black")){
        return true; 
    }
    return false;
}

std::vector<std::string> GameManager::getAllPawnMoves(int row, int col, const std::string &color){
    //NON Capturing Case 
    //it can move one square forward
    std::vector<std::string> legalMoves; 

    char deptFile = 'a' + col;
    char destFile = 'a' + col; 

    if (color == "white"){
        //row can decrease
        int destRank =  8 - (row - 1);
        std::string move = std::string(1, destFile) + std::to_string(destRank);
        if (MoveValidator::isvalidMove(board, move, color).valid){
            legalMoves.push_back(move);
        }
    } else{
        //row can increase 
        int destRank =  8 - (row + 1);
        std::string move = std::string(1, destFile) + std::to_string(destRank);
        if (MoveValidator::isvalidMove(board, move, color).valid){
            legalMoves.push_back(move);
        }
    }

    auto piece = board.grid[row][col];
    if (!piece->hasMoved){
        //it hasnt moved yet .... it could move 2 squares forward 
        if (color == "white"){
            //row can decrease
            int destRank =  8 - (row - 2);
            std::string move = std::string(1, destFile) + std::to_string(destRank);
            if (MoveValidator::isvalidMove(board, move, color).valid){
                legalMoves.push_back(move);
            }
        } else{
            //row can increase 
            int destRank =  8 - (row + 2);
            std::string move = std::string(1, destFile) + std::to_string(destRank);
            if (MoveValidator::isvalidMove(board, move, color).valid){
                legalMoves.push_back(move);
            }
        }
    } 
    //Capturing Case 
    if (color == "white"){
        //row decreases 1, col increases or decreases 1 
        //SAN notation is filexdestSquare
        //left capture 
        int destRank =  8 - (row - 1);
        if (row-1 >= 0 && col-1 >= 0){
            auto leftCapture_piece = board.grid[row-1][col-1];
            if (leftCapture_piece && leftCapture_piece->color != color){
                char destFile = 'a' + col - 1;
                std::string move = std::string(1,deptFile) + "x" + std::string(1,destFile) + std::to_string(destRank); 
                if (MoveValidator::isvalidMove(board,move,color).valid){
                    legalMoves.push_back(move);
                }
            } 
        }
        //right capture
        if (row-1 >= 0 && col+1 < 8){
            auto rightCapture_piece = board.grid[row-1][col+1];
            if (rightCapture_piece && rightCapture_piece->color != color){
                char destFile = 'a' + col + 1;
                std::string move = std::string(1,deptFile) + "x" + std::string(1,destFile) + std::to_string(destRank); 
                if (MoveValidator::isvalidMove(board,move,color).valid){
                    legalMoves.push_back(move);
                }
            } 
        }
    } else {
        int destRank =  8 - (row + 1);
        if (row+1 < 8 && col-1 >= 0){
            auto leftCapture_piece = board.grid[row-1][col-1];
            if (leftCapture_piece && leftCapture_piece->color != color){
                char destFile = 'a' + col - 1;
                std::string move = std::string(1,deptFile) + "x" + std::string(1,destFile) + std::to_string(destRank); 
                if (MoveValidator::isvalidMove(board,move,color).valid){
                    legalMoves.push_back(move);
                }
            } 
        }
        //right capture
        if (row+1 < 8 && col+1 < 8){
            auto rightCapture_piece = board.grid[row-1][col+1];
            if (rightCapture_piece && rightCapture_piece->color != color){
                char destFile = 'a' + col + 1;
                std::string move = std::string(1,deptFile) + "x" + std::string(1,destFile) + std::to_string(destRank); 
                if (MoveValidator::isvalidMove(board,move,color).valid){
                    legalMoves.push_back(move);
                }
            } 
        }
    }
    //En Passant 
    //1. check if opponent pawn is to the left or right 
    //checking for valid left pawn 
    int dir = (color == "white") ? -1 : 1;

    if (col > 0){
        auto potential_pawn = board.grid[row][col - 1];
        if (potential_pawn && potential_pawn->name == "pawn" && potential_pawn->color != color){
            //2. check if that pawn just moved 2 squares 
            if (potential_pawn->moveHistory.size() >= 2){
                int prevRow = potential_pawn->moveHistory[0].first;
                int currRow = potential_pawn->moveHistory[1].first;
                if (std::abs(currRow - prevRow) == 2 && currRow == row) {
                    //3. construct capture move
                    char destFile = 'a' + (col - 1);
                    int rowAhead = row + dir;
                    int targetRank = 8 - rowAhead;
                    std::string move = std::string(1,deptFile) + "x" + std::string(1,destFile) + std::to_string(targetRank); 
                    if (MoveValidator::isvalidMove(board,move,color).valid){
                        legalMoves.push_back(move);
                    }
                }
            }
        }
    }
    //checking for valid right pawn 
    if (col + 1 < 8){
        auto potential_pawn = board.grid[row][col + 1];
        if (potential_pawn && potential_pawn->name == "pawn" && potential_pawn->color != color){
            //2. check if that pawn just moved 2 squares 
            if (potential_pawn->moveHistory.size() >= 2){
                int prevRow = potential_pawn->moveHistory[0].first;
                int currRow = potential_pawn->moveHistory[1].first;
                if (std::abs(currRow - prevRow) == 2 && currRow == row) {
                    //3. construct capture move
                    char destFile = 'a' + (col + 1);
                    int rowAhead = row + dir;
                    int targetRank = 8 - rowAhead;
                    std::string move = std::string(1,deptFile) + "x" + std::string(1,destFile) + std::to_string(targetRank); 
                    if (MoveValidator::isvalidMove(board,move,color).valid){
                        legalMoves.push_back(move);
                    }
                }
            }
        }
    }
    return legalMoves;
}

std::vector<std::string> GameManager::getAllKnightMoves(int row, int col, const std::string& color){
    std::vector<std::string> legalMoves;
    //there is only 8 possible places it can go
    const std::vector<std::pair<int, int>> knightOffsets = {
        {-2, -1}, {-2, +1},
        {-1, -2}, {-1, +2},
        {+1, -2}, {+1, +2},
        {+2, -1}, {+2, +1}
    };
    std::vector<std::pair<int, int>> possibleKnightSquares;

    for (const auto& offset : knightOffsets) {
        int newRow = row + offset.first;
        int newCol = col + offset.second;
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
            possibleKnightSquares.emplace_back(newRow, newCol);
        }
    } 

    for (std::pair<int,int> possibleSquare : possibleKnightSquares){
        char destFile = 'a' + possibleSquare.second;
        int destRank = 8 - possibleSquare.first; 
        auto capturePiece = board.grid[possibleSquare.first][possibleSquare.second]; 
        if (capturePiece && capturePiece->color != color){
                //it could capture 
                std::string move = "Nx" + std::string(1,destFile) + std::to_string(destRank);
                if (MoveValidator::isvalidMove(board,move,color).valid){
                    legalMoves.push_back(move);
                }
            } else if (!capturePiece){
                std::string move = "N" + std::string(1,destFile) + std::to_string(destRank);
                //it can just move there and not capture 
                if (MoveValidator::isvalidMove(board,move,color).valid){
                    legalMoves.push_back(move);
                }
        }
    }
    return legalMoves;
}

std::vector<std::string> GameManager::getAllRookMoves(int row, int col, const std::string& color){
    std::vector<std::string> legalMoves;
    //Horizontal possibilities
    for (int c=0; c < 8; ++c){
        if (c == col) continue; 
        char destFile = 'a' + c; 
        int destRank = 8 - row;
        auto potential_piece = board.grid[row][c]; 
        std::string move;
        if (potential_piece && potential_piece->color != color){
            move = "Rx" + std::string(1,destFile) + std::to_string(destRank);
        } else if (!potential_piece){
            move = "R" + std::string(1,destFile) + std::to_string(destRank); 
        }
        if (MoveValidator::isvalidMove(board,move,color).valid){
            legalMoves.push_back(move);
        }
    } 
    //Vertical possibilities
    for (int r=0; r < 8; ++r){
        if (r == row) continue; 
        char destFile = 'a' + col; 
        int destRank = 8 - r;
        auto potential_piece = board.grid[r][col];
        std::string move;
        if (potential_piece && potential_piece->color != color){
            move = "Rx" + std::string(1,destFile) + std::to_string(destRank);
        } else if (!potential_piece){
            move = "R" + std::string(1,destFile) + std::to_string(destRank); 
        }
        if (MoveValidator::isvalidMove(board,move,color).valid){
            legalMoves.push_back(move);
        }
    } 
    return legalMoves;
}

std::vector<std::string> GameManager::getAllBishopMoves(int row, int col, const std::string& color){
    std::vector<std::string> legalMoves; 
    const std::vector<std::pair<int, int>> directions = {
                    {-1, -1}, {-1, +1},  // up-left, up-right
                    {+1, -1}, {+1, +1}   // down-left, down-right
                };

    for (const auto& dir : directions) {
        int r = row + dir.first;
        int c = col + dir.second;

        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
            auto target = board.grid[r][c];
            char destFile = 'a' + c;
            int destRank = 8 - r;
            std::string move;

            if (target && target->color == color) {
                break; // blocked by same-color piece
            } else if (target && target->color != color) {
                move = "Bx" + std::string(1, destFile) + std::to_string(destRank);
            } else {
                move = "B" + std::string(1, destFile) + std::to_string(destRank);
            }

            if (MoveValidator::isvalidMove(board, move, color).valid) {
                legalMoves.push_back(move);
            }

            if (target) break; // can't go past any piece

            r += dir.first;
            c += dir.second;
        }
    }

    return legalMoves;
}

std::vector<std::string> GameManager::getAllKingMoves(int row, int col, const std::string& color){
    std::cout << "Finding legal moves for " << color << " king" << std::endl;
    std::vector<std::string> legalMoves; 
    const std::vector<std::pair<int, int>> kingOffsets = {
        {-1, -1}, {-1, 0},{-1,+1},
        {0, -1}, {0, + 1},
        {+1, -1}, {+1, 0}, {+1,+1}
    };

    std::vector<std::pair<int, int>> possibleKingSquares;

    for (const auto& offset : kingOffsets) {
        int newRow = row + offset.first;
        int newCol = col + offset.second;
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
            possibleKingSquares.emplace_back(newRow, newCol);
        }
    } 

    for (auto square : possibleKingSquares){
        auto possiblePiece = board.grid[square.first][square.second];
        char destFile = 'a' + square.second;
        int destRank = 8 - square.first; 
        std::string move;
        if (possiblePiece && possiblePiece->color != color){
            //king capture 
            move = "Kx" + std::string(1, destFile) + std::to_string(destRank); 
        } else if (!possiblePiece){
            move = "K" + std::string(1, destFile) + std::to_string(destRank); 
        } else {
            continue; 
        }
        if (!move.empty() && MoveValidator::isvalidMove(board, move, color).valid) {
            std::cout << "This move: " << move << "is valid" << std::endl;
            legalMoves.push_back(move);
        }
    }

    //check castling options 
    std::string queenside_castle = "O-O-O";
    std::string kingside_castle = "O-O";
    if (MoveValidator::isvalidMove(board,queenside_castle,color).valid) {
        legalMoves.push_back(queenside_castle);
    }
    if (MoveValidator::isvalidMove(board,kingside_castle,color).valid) {
        legalMoves.push_back(kingside_castle);
    }
    return legalMoves;
}

std::vector<std::string> GameManager::getAllLegalMoves(const std::string& color){
    std::cout << "[checkGameOver:getAllLegalMoves] finding legal moves for " << color << std::endl;
    std::vector<std::string> legalMoves; 
    for (int row=0;row<8; ++row){
        for (int col=0; col<8; ++col){
            auto piece = board.grid[row][col]; 
            if (!piece) continue;
            if (piece->color != color) continue; 
            std::cout << "[checkGameOver:getAllLegalMoves] piece name: " << piece->name << " color: " << piece->color << std::endl;
            if (piece->name == "pawn"){
                auto moves = getAllPawnMoves(row,col,color); 
                legalMoves.insert(legalMoves.end(),moves.begin(),moves.end());
            } else if (piece->name == "knight"){
                auto moves = getAllKnightMoves(row,col,color);  
                legalMoves.insert(legalMoves.end(),moves.begin(),moves.end());  
            } else if (piece->name == "rook"){
                auto moves  = getAllRookMoves(row,col,color);    
                legalMoves.insert(legalMoves.end(),moves.begin(),moves.end());
            } else if (piece->name == "bishop"){
                auto moves  = getAllBishopMoves(row,col,color);
                legalMoves.insert(legalMoves.end(),moves.begin(),moves.end());
            } else if (piece->name == "queen"){
                std::vector<std::string> legalRookMoves = getAllRookMoves(row,col,color);
                std::vector<std::string> legalBishopMoves = getAllBishopMoves(row,col,color); 
                legalRookMoves.insert(legalRookMoves.end(), legalBishopMoves.begin(), legalBishopMoves.end());
                legalMoves.insert(legalMoves.end(), legalRookMoves.begin(), legalRookMoves.end());
            } else if (piece->name == "king"){
                auto moves  = getAllKingMoves(row,col,color);
                legalMoves.insert(legalMoves.end(),moves.begin(),moves.end()); 
            }
        }
    }
    std::cout << "[checkGameOver:getAllLegalMoves] there are " << legalMoves.size() <<  " legal moves." << std::endl;
    if (legalMoves.size() < 5){
        std::cout << "Legal Moves: " << std::endl; 
        for (std::string move : legalMoves){
            std::cout << move << std::endl;
        }
    }
    return legalMoves;
};