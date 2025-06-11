#include "move_validator.hpp" 

using namespace MoveValidator;

bool isCheck(const std::string& move, const std::string& color, const Board& board){
    //Checks if a potential move would result in *OPPOSITE* color being in Check 
    //Clone the board 
    Board clonedBoard = board;
    //Simulate the move 
    clonedBoard.movePiece(move);
    //Determine opponent color 
    std::string opponentColor = (color == "white") ? "black" : "white";
    //Find opponent king location
    int opp_kingRow = -1; int opp_kingCol = -1;
    for (int r=0; r<8; ++r){
        for (int c=0; c<8; ++c){
            auto piece = clonedBoard.grid[r][c];
            if (piece && piece->name == "king" && piece->color == opponentColor)
            {
                opp_kingRow = r;
                opp_kingCol = c;
                break;
            }
        }
    }

    if (opp_kingRow == -1 || opp_kingCol == -1) {
        throw std::runtime_error("Opponent King not Found!");
    }

    bool isCapture = move.find('x') != std::string::npos;
    //Check if any piece of the current player can reach that king's square 
    for (int r=0; r<8; ++r){
        for (int c=0; c<8; ++c){
            auto piece = clonedBoard.grid[r][c];
            if (!piece || piece->color == opponentColor) continue;
            if (pieceCanReach(*piece, r, c, opp_kingRow, opp_kingCol, isCapture, clonedBoard)){
                return true;
            }
        }
    }
    return false;
}

bool isvalidNotation(const std::string& move){
    static const std::regex sanPattern(
        R"(^(
            O-O(-O)?                             |  # Castling
            ([KQRNB])?                           # Optional piece letter
            ([a-h]|[1-8]|[a-h][1-8])?            # Optional disambiguation
            x?                                   # Optional capture
            [a-h][1-8]                           # Destination square
            (=)[QRNB]?                           # Optional promotion
            [+#]?                                # Optional check or mate
        )$)",
        std::regex::extended | std::regex::icase
    );
    return std::regex_match(move, sanPattern);
}

ParsedSAN parseSAN(const std::string& move, const std::string& color, const Board& board) {
    ParsedSAN result{};

    if (move == "O-O" || move == "O-O-O") {
        result.pieceType = 'K';
        result.destFile = (move == "O-O") ? 'g' : 'c';
        result.destRank = (color == "white") ? 1 : 8;
        result.isCheck = isCheck(move, color, board);

        // Departure square is always 'e1' for white or 'e8' for black
        result.deptFile = 'e';
        result.deptRank = (color == "white") ? 1 : 8;

        return result;
    }

    // Parse regular SAN move
    std::smatch matches;
    std::regex pattern(R"((?:(?P<piece>[KQRNB])?)(?:(?P<disamb>[a-h1-8]{0,2}))(?:(?P<capture>x)?)(?P<dest>[a-h][1-8])(?:=(?P<promo>[QRNB]))?(?P<check>[+#])?)", std::regex::icase);

    if (std::regex_match(move, matches, pattern)) {
        std::string piece = matches["piece"];
        result.pieceType = piece.empty() ? 'P' : std::toupper(piece[0]);

        std::string disamb = matches["disamb"];
        if (disamb.size() == 1) {
            if (std::isalpha(disamb[0])) result.disambiguationFile = disamb[0];
            else result.disambiguationRank = disamb[0] - '0';
        } else if (disamb.size() == 2) {
            result.disambiguationFile = disamb[0];
            result.disambiguationRank = disamb[1] - '0';
        }

        result.isCapture = matches["capture"].matched;
        result.destFile = matches["dest"].str()[0];
        result.destRank = matches["dest"].str()[1] - '0';
        result.isPromotion = matches["promo"].matched;
        result.isCheck = matches["check"].str() == "+";
        result.isMate = matches["check"].str() == "#";

        // Try to find the departure square by scanning the board
        int destCol = result.destFile - 'a';
        int destRow = 8 - result.destRank;
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                auto piece = board.grid[row][col];
                if (!piece || piece->color != color) continue;
                if (std::toupper(piece->name[0]) != result.pieceType) continue;
                if (result.disambiguationFile && col != result.disambiguationFile - 'a') continue;
                if (result.disambiguationRank && row != 8 - result.disambiguationRank) continue;

                if (pieceCanReach(*piece, row, col, destRow, destCol, result.isCapture, board)) {
                    result.deptFile = 'a' + col;
                    result.deptRank = 8 - row;
                    return result;  // First matching piece wins
                }
            }
        }
    }

    return result;
}


bool isValidPassant(const std::shared_ptr<Piece>& attacker, const Board& board){
    auto lastPos = attacker->moveHistory.back();
    int depRow = lastPos.first;
    int depCol = lastPos.second;

    int dir = (attacker->color == "white") ? -1 : 1;
    int targetRow = depRow + dir;

    // Check both diagonal squares (left and right)
    for (int dc = -1; dc <= 1; dc += 2) {
        int targetCol = depCol + dc;

        if (targetCol < 0 || targetCol >= 8) continue;  // off-board

        // Check if target square is empty
        if (board.grid[targetRow][targetCol]) continue;

        // Check if there's a capturable pawn next to us
        auto capturedPawn = board.grid[depRow][targetCol];
        if (!capturedPawn || capturedPawn->name != "pawn") continue;
        if (capturedPawn->color == attacker->color) continue;

        // Check that captured pawn just did a double advance
        if (capturedPawn->moveHistory.size() != 2) continue;
        int prevRow = capturedPawn->moveHistory[0].first;
        int currRow = capturedPawn->moveHistory[1].first;

        if (std::abs(currRow - prevRow) == 2) {
            return true;  // all conditions satisfied
        }
    }

    return false;
}

bool pieceCanReach(const Piece& piece, int depRow, int depCol, int destRow, int destCol, bool isCapture, const Board& board){
    if (piece.name == "pawn") {
        int dir = (piece.color == "white") ? -1 : 1;  // white goes up, black goes down
        int rowDelta = destRow - depRow;
        int colDelta = destCol - depCol;

        //Check en passant
        bool possiblePassant = false; 
        if (piece.moveHistory.size() == 2 && isCapture){
            //could be en passant
            auto capturedPawn = board.grid[depRow][destCol]; 
            if (capturedPawn){
                int prevRow = capturedPawn->moveHistory[0].first;
                int currRow = capturedPawn->moveHistory[1].first;
                if (std::abs(currRow - prevRow) == 2) {
                    possiblePassant = true;
                }
            }
        }

        if (possiblePassant){
            bool validPassant = isValidPassant(piece,board);
            if (validPassant){
                return true; 
            }
        }

        // Capturing move (must be diagonal)
        if (isCapture) {
            if (std::abs(colDelta) != 1 || rowDelta != dir) {
                return false;
            }
        } else {
            // Non-capturing move (must be straight forward)
            if (colDelta != 0) {
                return false;
            }
            if (rowDelta == dir) {
                // Normal 1-square forward
                if (board.grid[destRow][destCol]) {
                    return false;  // cannot move onto a piece
                }
            } else if (rowDelta == 2 * dir) {
                // 2-square forward
                if (piece.hasMoved) return false;
                int midRow = depRow + dir;
                if (board.grid[midRow][destCol] || board.grid[destRow][destCol]) {
                    return false;  // path must be clear
                }
            } else {
                return false;  // invalid forward move
            }
        }
    
    } else if (piece.name == "rook"){
        //rook cannot move diagonally 
        if (depRow != destRow && depCol != destCol) {
            return false;
        }
        
        // Horizontal movement
        if (depRow == destRow) {
            int step = (destCol > depCol) ? 1 : -1;
            for (int c = depCol + step; c != destCol; c += step) {
                if (board.grid[depRow][c]) {
                    return false;
                }
            }
        }

        // Vertical movement
        if (depCol == destCol) {
            int step = (destRow > depRow) ? 1 : -1;
            for (int r = depRow + step; r != destRow; r += step) {
                if (board.grid[r][depCol]) {
                    return false;
                }
            }
        }

    } else if piece.name == "knight"{
        //Knight can move 2 vertically and 1 horizontally or 2 horizontally and 1 vertically 
        std::vector<std::pair<int, int>> knightMoves = {
            {depRow + 2, depCol + 1}, {depRow + 2, depCol - 1},
            {depRow - 2, depCol + 1}, {depRow - 2, depCol - 1},
            {depRow + 1, depCol + 2}, {depRow + 1, depCol - 2},
            {depRow - 1, depCol + 2}, {depRow - 1, depCol - 2}
        };
        bool validSquare = false;
        for (const auto& move : knightMoves){
            if (move.first == destRow && move.second == destCol){
                validSquare = true;
            }
        }
        if (!validSquare){
            return false; 
        }
    } else if piece.name == "bishop"{
        std::vector<std::pair<int,int>> bishopMoves;
        const std::vector<std::pair<int,int>> directions = {
            {-1,-1},{-1,1},{1,-1},{1,1}
        };
        for (const auto& dir : directions){
            int r = depRow + dir.first;
            int c = depCol + dir.second;
            while (r >=0 && r < 8 && c >=0 && c < 8){
                auto piece = board.grid[r][c];
                if (!piece){
                    validSquares.push_back({r,c});
                } else {
                    if (piece->color != color){
                        validSquares.push_back({r,c});
                    }
                    break; //cannot phase through pieces
                }
                r += dir.first;
                c += dir.second;
            }
        }
    } else if piece.name == "queen"{
        //can move like a bishop or a rook 
        Piece pieceCopy = piece;
        pieceCopy.name = "bishop";
        bool valid_bishop_move = pieceCanReach(pieceCopy, depRow, depCol, destRow, destCol, isCapture, board);
        Piece pieceCopy = piece;
        pieceCopy.name = "rook";
        bool valid_rook_move = pieceCanReach(pieceCopy, depRow, depCol, destRow, destCol, isCapture, board);
        if (!(valid_rook_move || valid_bishop_move)){
            return false;
        }
    } else if piece.name == "king"{
        std::vector<std::pair<int,int>> kingMoves = {
            {depRow - 1,depCol},{depRow - 1, depCol + 1},
            {depRow,depCol + 1},{depRow + 1, depCol + 1},
            {depRow + 1, depCol},{depRow + 1, depCol - 1},
            {depRow, depCol - 1},{depRow - 1, depCol -1}
        };
        bool validSquare = false;
        for (const auto& move : kingMoves){
            if (move.first == destRow && move.second == destCol){
                validSquare = true; 
            }
        }
        if (!validSquare){
            return false; 
        }
    }
    //There cannot be a piece on the dest square unless its a capture
    if (board.grid[destRow][destCol] && !isCapture){
        return false; 
    }
    auto destPiece = board.grid[destRow][destCol];
    if (destPiece && destPiece->color == piece.color){
        return false; 
    }
    return true;
}

bool isvalidPattern(const Board&board, const std::string&move, const std::string&color){
    ParsedSAN parsed_move = parseSAN(move,color,board); 

    bool isCapture = move.find('x') != std::string::npos;

    int target_col = parsed_move.destFile - 'a';
    int target_row = 8 - parsed_move.destRank;

    //want to iterate over the board to determine where the piece is coming from
    std::vector<std::shared_ptr<Piece>> candidate_pieces;
    for (int row=0; row<8; ++row){
        for (int col=0; col<8; ++col){
            auto piece = board.grid[row][col]; //remember this is a shared pointer
            if (!piece || piece->color != color) continue;
            if (piece->name != parsed_move.pieceType) continue;
            if (parsed.disambiguationFile && col != (parsed.disambiguationFile - 'a')) continue;
            if (parsed.disambiguationRank && row != (8 - (parsed.disambiguationRank - '0'))) continue;
            //we have found the piece that this move is referring to 
            if pieceCanReach(*piece,row,col,target_row,target_col,isCapture,board){
                candidate_pieces.push_back(piece);
            }
        }
    }

    return !candidate_pieces.empty();
}

bool islegalMove(const Board&board, const std::string&move, const std::string&color){
    //You cant put yourself into check 
    ParsedSAN parsed_move = parseSAN(move,color,board); 
    int target_col = parsed_move.destFile - 'a';
    int target_row = 8 - parsed_move.destRank; 
    std::string opp_color = (color == "white") ? "black" : "white";
    if (isCheck(move,opp_color,board)){
        return false;
    }
    //If there is a piece on the destination square, it must be a capture
    //and it must be the opposite color
    bool isCapture = move.find('x') != std::string::npos;

    if (board.grid[target_row][target_col] && isCapture){
        //if there is a capture it needs to be the opposite color 
        if (board.grid[target_row][target_col]->color == color){
            return false;
        }
    } else if (board.grid[target_row][target_col] && !isCapture){
        return false;
    }
    return true; 
}

MoveResult isvalidMove(const Board&board, const std::string&move, const std::string&color){
    //Step 1: Notation check 
    if (!isvalidNotation(move)){
        return MoveResult::error("ERROR: This is not a valid SAN move.");
    }
    //Step 2: Check validity of pattern
    if (!isvalidPattern(board,move,color)){
        return MoveResult::error("ERROR: That piece cannot move that way.");
    }
    //Step 3: Check legality of the move 
    if (!islegalMove(board,move,color)){
        //i.e; you cannot put yourself in check, you cant move a piece that isnt on the dep square,
        //you can't move to a square with a piece of the same color 
        return MoveResult::error("ERROR: This move is illegal.");
    }
    return MoveResult::success();
}