#include "move_validator.hpp" 
#include "board.hpp"

namespace MoveValidator{

    
bool isCheck(const Board& board, const ParsedSAN &parsed_move, const std::string& color, int depRow, int depCol, int destRow, int destCol) {
    //std::cout << "inside isCheck ... Checking if this move would put " << color << " in check ..." << std::endl; 
    Board clonedBoard = board;
    // std::cout << "depRow: " << depRow << " depCol: " << depCol << std::endl; 
    // std::cout << "destRow: " << destRow << " destCol: " << destCol << std::endl; 
    clonedBoard.simulateMove(depRow, depCol, destRow, destCol);

    if (parsed_move.isCastle){
        //need to move the rook as well 
        if (std::abs(destCol - depCol) == 2 && destCol == 6){
            //Kingside Castle 
            clonedBoard.simulateMove(depRow, 7, destRow, 5);
        } else if (std::abs(destCol - depCol) == 2 && destCol == 2){
            //Queenside castle
            clonedBoard.simulateMove(depRow, 0, destRow, 3);
        } else {
             std::cerr << "[isCheck] ERROR: Unexpected castling destination column: " << destCol
                  << ". Expected 6 (kingside) or 2 (queenside)." << std::endl;
        }
    }

    int kingRow = -1, kingCol = -1;

    // Find our own king
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            auto piece = clonedBoard.grid[r][c];
            if (piece && piece->name == "king" && piece->color == color) {
                kingRow = r;
                kingCol = c;
                break;
            }
        }
    }
    // std::cout << color << " kingRow: "<< kingRow << " " << color << " kingCol: " << kingCol << std::endl;

    if (kingRow == -1) throw std::runtime_error("King not found"); 

    // Can any opponent piece attack our king?
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            auto piece = clonedBoard.grid[r][c];
            if (!piece || piece->color == color) continue; 
            // std::cout << "Found opponent piece ... Checking the reach of piece at row: " << r << " and col: " << c << std::endl; 
            // std::cout << "Checking if " << piece->color << " " << piece->name << " can reach " << color << " king" << std::endl;
            //const Piece& piece, int depRow, int depCol, int destRow, int destCol, bool isCapture, const Board& board  
            if (pieceCanReach(*piece, r, c, kingRow, kingCol, true, clonedBoard)) {
                return true;
            }
        }
    }

    return false;
}

bool isvalidNotation(const std::string& move){
    std::string trimmed = move;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r\f\v"));  // left trim
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r\f\v") + 1);  // right trim

    static const std::regex sanPattern(
        R"(^(O-O(-O)?|[KQRNB]?[a-h]?[1-8]?x?[a-h][1-8](=[QRNB])?[+#]?)$)",
        std::regex::icase
    );

    return std::regex_match(trimmed, sanPattern);
}

std::vector<std::pair<int, int>> findCandidatePieces(const ParsedSAN& parsed, const Board& board, const std::string& color) {
    std::vector<std::pair<int, int>> candidates;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            auto piece = board.grid[row][col];
            if (!piece || piece->color != color) continue;
            if (piece->name != parsed.pieceType) continue;

            if (parsed.disambiguationFile && col != parsed.disambiguationFile - 'a') continue;
            if (parsed.disambiguationRank && row != 8 - parsed.disambiguationRank) continue;

            candidates.emplace_back(row, col);
        }
    }
    return candidates;
}

ParsedSAN parseSAN(const std::string& raw_move, const std::string& color, const Board& board) {
    ParsedSAN result{};
    std::string move = trim(raw_move);

    if (isCastling(move)) {
        return parseCastling(move, color);
    }

    std::smatch matches;
    static const std::regex pattern(
        R"(^(?:([KQRNB])?([a-h1-8]{0,2})?(x)?([a-h][1-8])(=([QRNB]))?([+#])?)$)",
        std::regex::icase
    );

    if (!std::regex_match(move, matches, pattern)) {
        throw std::runtime_error("Invalid SAN format: " + move);
    }

    result = parsePattern(matches, color);
    auto candidates = findCandidatePieces(result, board, color);

    if (candidates.empty()) {
        throw std::runtime_error("Could not resolve departure square for move: " + move);
    }

    // Choose first valid candidate — optionally fallback on legality
    for (const auto& pos : candidates) {
        const auto& piece = board.grid[pos.first][pos.second];
        std::cout << "Checking if " << color << " " << piece->name 
          << " can go to " << result.destFile << static_cast<int>(result.destRank) << std::endl;

        if (piece && pieceCanReach(*piece, pos.first, pos.second,
                                   8 - result.destRank, result.destFile - 'a',
                                   result.isCapture, board)) {
            result.deptFile = 'a' + pos.second;
            result.deptRank = 8 - pos.first;
            return result;
        }
    }

    return result;
    //throw std::runtime_error("No piece can make the move: " + move);
}


std::string trim(const std::string& s) {
    const auto begin = s.find_first_not_of(" \t\n\r\f\v");
    const auto end = s.find_last_not_of(" \t\n\r\f\v");
    return (begin == std::string::npos) ? "" : s.substr(begin, end - begin + 1);
}

bool isCastling(const std::string& move) {
    return move == "O-O" || move == "O-O-O";
}

ParsedSAN parseCastling(const std::string& move, const std::string& color) {
    ParsedSAN result;
    result.isCastle = true;
    result.pieceType = "king";
    result.destFile = (move == "O-O") ? 'g' : 'c';
    result.destRank = (color == "white") ? 1 : 8;
    result.deptFile = 'e';
    result.deptRank = (color == "white") ? 1 : 8;
    return result;
}

ParsedSAN parsePattern(const std::smatch& m, const std::string& color) {
    ParsedSAN result;
    result.isCastle = false;

    const std::unordered_map<char, std::string> sanToPieceName = {
        {'K', "king"}, {'Q', "queen"}, {'R', "rook"}, {'B', "bishop"}, {'N', "knight"}
    };

    std::string piece = m[1].str();
    if (!piece.empty()) {
        result.pieceType = sanToPieceName.at(std::toupper(piece[0]));
    } else {
        result.pieceType = "pawn";
    }

    std::string disamb = m[2].str();
    if (disamb.size() == 1) {
        if (std::isalpha(disamb[0])) result.disambiguationFile = disamb[0];
        else result.disambiguationRank = disamb[0] - '0';
    } else if (disamb.size() == 2) {
        result.disambiguationFile = disamb[0];
        result.disambiguationRank = disamb[1] - '0';
    }

    result.isCapture = !m[3].str().empty();

    std::string dest = m[4].str();
    result.destFile = dest[0];
    result.destRank = dest[1] - '0';

    result.isPromotion = !m[5].str().empty();
    std::string checkFlag = m[7].str();
    result.isCheck = checkFlag == "+";
    result.isMate = checkFlag == "#";

    return result;
}

bool isValidPassant(const Piece& attacker, const Board& board){
    auto lastPos = attacker.moveHistory.back();
    int depRow = lastPos.first;
    int depCol = lastPos.second;

    int dir = (attacker.color == "white") ? -1 : 1;
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
        if (capturedPawn->color == attacker.color) continue;

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
        //std::cout << "[MoveValidator: pieceCanReach] checking reach of "<< piece.name << std::endl;
        int dir = (piece.color == "white") ? -1 : 1;  // white goes up, black goes down
        int rowDelta = destRow - depRow;
        int colDelta = destCol - depCol;

        //Check en passant
        bool possiblePassant = false; 
        auto attacker = board.grid[depRow][depCol];
        if (piece.moveHistory.size() == 2 && isCapture && !board.grid[destRow][destCol]){
            //could be en passant
            auto capturedPawn = board.grid[depRow][destCol]; 
            if (capturedPawn && capturedPawn->name == "pawn" && attacker->name == "pawn"){
                int prevRow = capturedPawn->moveHistory[0].first;
                int currRow = capturedPawn->moveHistory[1].first;
                if (std::abs(currRow - prevRow) == 2) {
                    possiblePassant = true;
                }
            }
        }

        if (possiblePassant){
            bool validPassant = isValidPassant(piece,board);
            std::cout << "[MoveValidator: pieceCanReach] WOW this is a valid passant?!!?" << std::endl;
            if (validPassant){
                return true; 
            }
        }
        //std::cout << "[pieceCanReach: MoveValidator] isCapture: " << isCapture << std::endl;
        // Capturing move (must be diagonal)
        if (isCapture) {
            //std::cout << "colDelta: "  << colDelta << " rowDelta: " << rowDelta << " this is dir: " << dir << std::endl;
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
                    //std::cout << "[MoveValidator: pieceCanReach] Cannot move onto another piece ... pawn captures diagonal!" << std::endl;
                    return false;  // cannot move onto a piece
                }
            } else if (rowDelta == 2 * dir) {
                //std::cout << "[MoveValidator: pieceCanReach] 2-square forward" << std::endl;
                // 2-square forward
                if (piece.moveHistory.size() > 1) return false; //piece has moved 
                int midRow = depRow + dir;
                if (board.grid[midRow][destCol] || board.grid[destRow][destCol]) {
                    //std::cout << "[MoveValidator: pieceCanReach] Path is not clear!" << std::endl;
                    return false;  // path must be clear
                }
            } else {
                return false;  // invalid forward move
            }
        }
    
    } else if (piece.name == "rook"){
        // std::cout << "[pieceCanReach:MoveValidator] Checking rook pattern ..." << std::endl;
        // std::cout << "This rook is departing from row: " << depRow << " and column: " << depCol << std::endl;
        //rook cannot move diagonally 
        if (depRow != destRow && depCol != destCol) {
            // std::cout << "[pieceCanReach:MoveValidator] Rook cannot move diagonally." << std::endl;
            return false;
        }
        
        // Horizontal movement
        if (depRow == destRow) {
            int step = (destCol > depCol) ? 1 : -1;
            for (int c = depCol + step; c != destCol; c += step) {
                //std::cout << "[pieceCanReach:MoveValidator] Checking horizontal path ... this is c: " << c << std::endl; 
                if (board.grid[depRow][c]) {
                    // std::cout << "There is something in the way!" << std::endl;
                    // std::cout << "The blocking piece is at row: " << depRow << " and col: " << c << std::endl;
                    // auto blocking_piece = board.grid[depRow][c];
                    // std::cout << "blocking_piece name: "<< blocking_piece->name << " blocking_piece color " << blocking_piece->color << std::endl;
                    return false;
                }
            }
        }

        // Vertical movement
        if (depCol == destCol) {
            int step = (destRow > depRow) ? 1 : -1;
            for (int r = depRow + step; r != destRow; r += step) {
                //std::cout << "[pieceCanReach:MoveValidator] Checking vertical path ... this is r: " << r << std::endl; 
                if (board.grid[r][depCol]) {
                    // std::cout << "There is something in the way!" << std::endl; 
                    // std::cout << "The blocking piece is at row: " << r << " and col: " << depCol << std::endl;
                    // auto blocking_piece = board.grid[r][depCol];
                    // std::cout << "blocking_piece name: "<< blocking_piece->name << " blocking_piece color " << blocking_piece->color << std::endl;
                    return false;
                }
            }
        }

    } else if (piece.name == "knight"){
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
    } else if (piece.name == "bishop"){
        std::vector<std::pair<int,int>> bishopMoves;
        const std::vector<std::pair<int,int>> directions = {
            {-1,-1},{-1,1},{1,-1},{1,1}
        };
        for (const auto& dir : directions){
            int r = depRow + dir.first;
            int c = depCol + dir.second;
            while (r >=0 && r < 8 && c >=0 && c < 8){
                auto sq_piece = board.grid[r][c];
                if (!sq_piece){
                    bishopMoves.push_back({r,c});
                } else {
                    if (piece.color != sq_piece->color){
                        bishopMoves.push_back({r,c});
                    }
                    break; //cannot phase through pieces
                }
                r += dir.first;
                c += dir.second;
            }
        }
        // Now check if dest is in bishopMoves
        auto target = std::make_pair(destRow, destCol);
        if (std::find(bishopMoves.begin(), bishopMoves.end(), target) == bishopMoves.end()) {
            return false;
        } 
    } else if (piece.name == "queen"){
        //can move like a bishop or a rook 
        Piece bishop_pieceCopy = piece;
        bishop_pieceCopy.name = "bishop";
        bool valid_bishop_move = pieceCanReach(bishop_pieceCopy, depRow, depCol, destRow, destCol, isCapture, board);
        Piece rook_pieceCopy = piece;
        rook_pieceCopy.name = "rook";
        bool valid_rook_move = pieceCanReach(rook_pieceCopy, depRow, depCol, destRow, destCol, isCapture, board);
        if (!(valid_rook_move || valid_bishop_move)){
            return false;
        }
    } else if (piece.name == "king"){
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
    //std::cout << "[pieceCanReach: MoveValidator] Checking if a piece occupies the target square" << std::endl; 
    if (board.grid[destRow][destCol] && !isCapture){
        //std::cout << "[pieceCanReach: MoveValidator] Square is occupied but no capture notated." << std::endl; 
        return false; 
    }

    auto destPiece = board.grid[destRow][destCol];
    if (destPiece && destPiece->color == piece.color){
        //std::cout << "[pieceCanReach: MoveValidator] Cannot capture a piece of the same color" << std::endl;
        return false; 
    }

    //std::cout << "[pieceCanReach: MoveValidator] All good this piece can reach!" << std::endl;
    return true;
}

bool isvalidPattern(const Board&board, const ParsedSAN &parsed_move, const std::string&color){
    if (parsed_move.isCastle){
        return true; //will handle in legality 
    }

    bool isCapture = parsed_move.isCapture;

    int target_col = parsed_move.destFile - 'a';
    int target_row = 8 - parsed_move.destRank;

    //want to iterate over the board to determine where the piece is coming from
    std::cout << "[isvalidPattern: MoveValidator]  Iterating over the board looking for: Piece Name: "  << parsed_move.pieceType << "  Piece Color: " << color <<std::endl;

    std::vector<std::shared_ptr<Piece>> candidate_pieces;
    for (int row=0; row<8; ++row){
        for (int col=0; col<8; ++col){
            auto piece = board.grid[row][col]; //remember this is a shared pointer
            if (!piece) continue;
            if (!piece || piece->color != color) continue;
            if (piece->name != parsed_move.pieceType) continue;
            //we have found the piece that this move is referring to 
            if (pieceCanReach(*piece,row,col,target_row,target_col,isCapture,board)){
                //std::cout << "We have found the piece that this move is referring to!" << std::endl;
                candidate_pieces.push_back(piece);
            } 
        }
    }
    std::cout << "[isvalidPattern: MoveValidator]  There are " << candidate_pieces.size() << " candidate pieces." << std::endl;
    return !candidate_pieces.empty();
}


bool isKingInCheck(const Board&board, const std::string&color){
    int kingRow = -1, kingCol = -1;

    // Find the king
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
    if (kingRow == -1) throw std::runtime_error("King not found");

    // Can any enemy piece attack the king?
    std::string oppColor = (color == "white") ? "black" : "white";
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            auto piece = board.grid[r][c];
            if (!piece || piece->color != oppColor) continue;
            if (pieceCanReach(*piece, r, c, kingRow, kingCol, true, board)) {
                std::cout << color << " is in Check!" << std::endl;
                return true;
            }
        }
    }

    return false;
}


bool islegalMove(const Board&board, const ParsedSAN &parsed_move, const std::string&color){
    //You cant put yourself into check 
    int target_col = parsed_move.destFile - 'a';
    int target_row = 8 - parsed_move.destRank; 

    std::string opp_color = (color == "white") ? "black" : "white";

    int depCol = parsed_move.deptFile - 'a';
    int depRow = 8 - parsed_move.deptRank;
    int destCol = parsed_move.destFile - 'a';
    int destRow = 8 - parsed_move.destRank;

    if (isKingInCheck(board,color)){
        if (parsed_move.isCastle){
            std::cout << "You can't castle out of check! :(" << std::endl;
            return false;
        }
        Board clonedBoard = board;
        clonedBoard.simulateMove(depRow, depCol, destRow, destCol);
        if (isKingInCheck(clonedBoard,color)){
            std::cout << "This move leaves " << color << " in check!" << std::endl;
            return false; 
        }
    }

    //std::cout << "[islegalMove: MoveValidator] Checking if this move puts " << color << " in check" << std::endl;
    if (isCheck(board,parsed_move,color,depRow,depCol,target_row,target_col)){
        std::cout << "[ERROR] This move puts yourself in check!" << std::endl;
        return false;}
    // } else {
    //     std::cout << "... it does not!" << std::endl;
    // }
    
    if (parsed_move.isCastle){
        //King and Rook need to be in the right place
        int row = (color == "white") ? 7 : 0;
        int kingCol = 4;
        int targetCol = (parsed_move.destFile == 'g') ? 6 : 2;
        int rookCol = (parsed_move.destFile == 'g') ? 7 : 0;
        // Check if king and rook are in place
        auto king = board.grid[row][kingCol];
        auto rook = board.grid[row][rookCol];
        if (!king || !rook || king->name != "king" || rook->name != "rook") {
            return false;
        }
        //Nothing can be inbtween them
        int start = std::min(kingCol, targetCol) + 1;
        int end   = std::max(kingCol, targetCol) - 1;
        for (int c = start; c <= end; ++c) {
            if (board.grid[row][c]) return false;
        }
        //Neither king/rook has moved yet 
        if (king->moveHistory.size() <= 1 && rook ->moveHistory.size() <= 1){
            return true; 
        } else {
            std::cout << "Invalid Castling ... rook or king as moved already!" << std::endl;
            return false; 
        }
    }

    //If there is a piece on the destination square, it must be a capture
    //and it must be the opposite color
    bool isCapture = parsed_move.isCapture;

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

MoveResult isvalidMove(const Board& board, const std::string& move, const std::string&color){
    //std::cout << "[INFO] Checking move validity: " << move << std::endl;
    //Step 1: Notation check 
    if (!isvalidNotation(move)){
        //std::cout << "[ERROR]: This: "<< move << " is not a valid SAN move." << std::endl;
        return MoveResult::error("ERROR: This is not a valid SAN move.");
    }

    ParsedSAN parsed_move = parseSAN(move,color,board);
    try {
        parsed_move = parseSAN(move, color, board);
    } catch (const std::exception& e) {
        return MoveResult::error("ERROR: " + std::string(e.what()));
    } 

    //Step 2: Check validity of pattern
    if (!isvalidPattern(board,parsed_move,color)){
        //std::cout << "[ERROR]: That piece cannot move that way." << std::endl;
        return MoveResult::error("ERROR: That piece cannot move that way.");
    }
    //Step 3: Check legality of the move 
    if (!islegalMove(board,parsed_move,color)){
        //std::cout << "[ERROR]: This move is illegal." << std::endl;
        return MoveResult::error("ERROR: This move is illegal.");
    }
    return MoveResult::success();
}


}; //end MoveValidator namespace 