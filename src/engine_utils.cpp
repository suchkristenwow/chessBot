#include "engine_utils.hpp" 

namespace EngineUtils{
    //piece tables 
    const int MOBILITY_WEIGHT = 10;
    const int MATERIAL_WEIGHT = 1;

    const int pawn_table[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    { 5,  5, 10, 25, 25, 10,  5,  5},
    { 0,  0,  0, 20, 20,  0,  0,  0},
    { 5, -5,-10,  0,  0,-10, -5,  5},
    { 5, 10, 10,-20,-20, 10, 10,  5},
    { 0,  0,  0,  0,  0,  0,  0,  0}
    };

    const int knight_table[8][8] = {
        {-50,-40,-30,-30,-30,-30,-40,-50},
        {-40,-20,  0,  0,  0,  0,-20,-40},
        {-30,  0, 10, 15, 15, 10,  0,-30},
        {-30,  5, 15, 20, 20, 15,  5,-30},
        {-30,  0, 15, 20, 20, 15,  0,-30},
        {-30,  5, 10, 15, 15, 10,  5,-30},
        {-40,-20,  0,  5,  5,  0,-20,-40},
        {-50,-40,-30,-30,-30,-30,-40,-50}
    };

    const int bishop_table[8][8] = {
        {-50,-20,-20,-20,-20,-20,-20,-50},
        {-20,  0,  0,  0,  0,  0,  0,-20},
        {-20,  0, 25, 40, 40, 25,  0,-20},
        {-20, 25, 25, 40, 40, 25, 25,-20},
        {-20,  0, 40, 40, 40, 40,  0,-20},
        {-20, 40, 40, 40, 40, 40, 40,-20},
        {-20, 25,  0,  0,  0,  0, 25,-20},
        {-50,-20,-20,-20,-20,-20,-20,-50}
    };

    const int rook_table[8][8] = {
        {50,50,50,50,50,50,50,50},
        {52,52,52,52,52,52,52,52},
        {50,50,50,50,50,50,50,50},
        {50,50,50,50,50,50,50,50},
        {50,50,50,50,50,50,50,50},
        {50,50,50,50,50,50,50,50},
        {50,50,50,50,50,50,50,50},
        {50,50,50,51,51,50,50,50},
    };

    const int king_table[8][8] = {
    {-95,  -95,  -90,  -90,  -90,  -90,  -95,  -95},  
    {-95,  -50,  -50,  -50,  -50,  -50,  -50,  -95},  
    {-90,  -50,  -20,  -20,  -20,  -20,  -50,  -90},  
    {-90,  -50,  -20,    0,    0,  -20,  -50,  -90},  
    {-90,  -50,  -20,    0,    0,  -20,  -50,  -90},  
    {-90,  -50,  -20,  -20,  -20,  -20,  -50,  -90},  
    {-95,  -50,  -50,  -50,  -50,  -50,  -50,  -95},  
    {-95,  -95,  -90,  -90,  -90,  -90,  -95,  -95}
    };

    const int queen_table[8][8] = {
    {-90,  -50,  -50,  -25,  -25,  -50,  -50,  -90},  
    {-50,    0,    0,    0,    0,    0,    0,  -50}, 
    {-50,    0,    25,   25,   25,   25,   0,  -50},
    {-25,    0,    25,   25,   25,   25,   0,  -25},   
    {  0,    0,    25,   25,   25,   25,   0,    0},
    {-25,    25,   25,   25,   25,   25,  25,  -25}, 
    {-25,    0,    0,    0,    0,    0,   0,   -25},
    {-90,  -50,  -50,  -25,  -25,  -50,  -50,  -90}
    };

    bool possiblePromotion(const Board& board, int row, int col, const std::string& color){
        //is it possible to promote this pawn 
        if (color == "white" && row == 1){
            auto piece = board.grid[0][col]; 
            if (!piece){
                return true; 
            }
            //check the left square 
            if (1 <= col){
                auto piece = board.grid[0][col - 1];
                if (piece->color != color){
                    //then I can take and promote 
                    return true; 
                }
            }
            //check the right square 
            if (col < 7){
                auto piece = board.grid[0][col + 1];
                if (piece->color != color){
                    //then I can take and promote 
                    return true; 
                }
            } 
        } else if (color == "black" && row == 6){
            auto piece = board.grid[7][col]; 
            if (!piece){
                return true; 
            }
            //check the left square 
            if (1 <= col){
                auto piece = board.grid[7][col - 1];
                if (piece->color != color){
                    //then I can take and promote 
                    return true; 
                }
            }
            //check the right square 
            if (col < 7){
                auto piece = board.grid[7][col + 1];
                if (piece->color != color){
                    //then I can take and promote 
                    return true; 
                }
            } 
        }
        return false; 
    }

    int evaluateBoard(const Board& board, const std::string& color){
        //std::cout  << "Evaluating board position for " << color << std::endl;
        int score = 0; 
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                auto piece = board.grid[row][col];
                if (!piece) continue;
                //std::cout  << "Evaluating the " <<  piece->color << " " << piece->name << " on row: " <<  row << " col: " << col << std::endl;
                int value = piece->point_val * MATERIAL_WEIGHT;
                //std::cout  << "piece value: " << value << std::endl;
                int bonus = 0;
                int eval_row = piece->color == "white" ? row : 7 - row;
                //std::cout  << "this is eval row: " << eval_row << std::endl; 
                if (piece->name == "pawn") {
                    bonus = pawn_table[eval_row][col];
                    //std::cout  << "bonus for this pawn: " << bonus << std::endl;
                    if (possiblePromotion(board,row,col,piece->color)){
                        // bonus is the same as if there was a queen on the promotion square 
                        //std::cout  << "Adding promotion bonus ... " << std::endl;
                        bonus += 100;
                    }
                } else if (piece->name == "knight") {
                    bonus = knight_table[eval_row][col];
                } else if (piece->name == "bishop") {
                    bonus = bishop_table[eval_row][col];
                } else if (piece->name == "rook") {
                    bonus = rook_table[eval_row][col];
                } else if (piece->name == "queen") {
                    bonus = queen_table[eval_row][col];
                } else if (piece->name == "king") {
                    bonus = king_table[eval_row][col];
                } 
                
                //std::cout  << "position bonus: " << bonus << std::endl;
                int total = value + bonus;
                //std::cout  << "total for this piece " << total << std::endl;
                score += (piece->color == color) ? total : -total;
            }
        }
        //std::cout  << "This is score before mobility: " << score << std::endl;
        //Reward mobility 
        auto myMoves = getAllLegalMoves(board,color); 
        std::string oppColor = (color=="white") ? "black" : "white";
        auto theirMoves = getAllLegalMoves(board,oppColor);
        ////std::cout  << color << " has " << myMoves.size() << std::endl; 
        //std::cout  << oppColor << " has " << theirMoves.size() << std::endl;
        score += (static_cast<int>(myMoves.size()) - static_cast<int>(theirMoves.size()))* MOBILITY_WEIGHT;  
        //std::cout  << color << " score: " << score << std::endl;
        return score;
    } 

    int alphaBeta(Board board, int depth, int alpha, int beta, bool maximizingPlayer, const std::string& rootColor){
        std::cout << "alphaBeta called at depth: " << depth << " maximizing: " << maximizingPlayer << std::endl;

        std::string currentColor;
        if (maximizingPlayer){
            currentColor = (rootColor == "white") ? "white" : "black";
        } else{ 
            currentColor = (rootColor == "white") ? "black" : "white";
        }

        if (depth == 0){
            return evaluateBoard(board,currentColor); 
        }
        
        auto legalMoves = getAllLegalMoves(board,currentColor); 

        if (legalMoves.empty()){
            //game over
            return evaluateBoard(board,currentColor); 
        }

        if (maximizingPlayer){
            int val = -1e8;
            for (const std::string& move : legalMoves){
                Board newBoard = board;
                newBoard.movePiece(move,currentColor); 
                int newBoard_eval = alphaBeta(newBoard,depth-1,alpha,beta,false,rootColor);
                val = std::max(val, newBoard_eval);
                if (beta <= val) break; 
                alpha = std::max(alpha,newBoard_eval);
            } 
            return val;
        } else {
            int val = 1e8;
            for (const std::string& move : legalMoves){
                Board newBoard = board;
                newBoard.movePiece(move,currentColor); 
                int newBoard_eval = alphaBeta(newBoard,depth-1,alpha,beta,true,rootColor);
                val = std::min(val, newBoard_eval);
                if (val <= alpha) break; 
                beta = std::min(beta,newBoard_eval);
            }
            return val;
        }
        return 0;
    }

    std::vector<std::string> getAllLegalMoves(const Board& board, const std::string& color){
        ////std::cout  << "[checkGameOver:getAllLegalMoves] finding legal moves for " << color << std::endl;
        std::vector<std::string> legalMoves; 
        for (int row=0;row<8; ++row){
            for (int col=0; col<8; ++col){
                auto piece = board.grid[row][col]; 
                if (!piece) continue;
                if (piece->color != color) continue; 
                ////std::cout  << "[checkGameOver:getAllLegalMoves] piece name: " << piece->name << " color: " << piece->color << std::endl;
                if (piece->name == "pawn"){
                    auto moves = getAllPawnMoves(board, row,col,color); 
                    legalMoves.insert(legalMoves.end(),moves.begin(),moves.end());
                } else if (piece->name == "knight"){
                    auto moves = getAllKnightMoves(board,row,col,color);  
                    legalMoves.insert(legalMoves.end(),moves.begin(),moves.end());  
                } else if (piece->name == "rook"){
                    auto moves  = getAllRookMoves(board, row,col,color);    
                    legalMoves.insert(legalMoves.end(),moves.begin(),moves.end());
                } else if (piece->name == "bishop"){
                    auto moves  = getAllBishopMoves(board,row,col,color);
                    legalMoves.insert(legalMoves.end(),moves.begin(),moves.end());
                } else if (piece->name == "queen"){
                    std::vector<std::string> legalRookMoves = getAllRookMoves(board,row,col,color);
                    std::vector<std::string> legalBishopMoves = getAllBishopMoves(board,row,col,color); 
                    legalRookMoves.insert(legalRookMoves.end(), legalBishopMoves.begin(), legalBishopMoves.end());
                    legalMoves.insert(legalMoves.end(), legalRookMoves.begin(), legalRookMoves.end());
                } else if (piece->name == "king"){
                    auto moves  = getAllKingMoves(board,row,col,color);
                    legalMoves.insert(legalMoves.end(),moves.begin(),moves.end()); 
                }
            }
        }
        return legalMoves;
    };

    std::vector<std::string> getAllPawnMoves(const Board& board, int row, int col, const std::string &color){
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
        if (piece->moveHistory.size() < 2){
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

    std::vector<std::string> getAllKnightMoves(const Board& board, int row, int col, const std::string& color){
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

    std::vector<std::string> getAllRookMoves(const Board& board, int row, int col, const std::string& color){
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

    std::vector<std::string> getAllBishopMoves(const Board& board, int row, int col, const std::string& color){
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

    std::vector<std::string> getAllKingMoves(const Board& board, int row, int col, const std::string& color){
        ////std::cout  << "Finding legal moves for " << color << " king" << std::endl;
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
                ////std::cout  << "This move: " << move << "is valid" << std::endl;
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

}