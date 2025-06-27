#include "board.hpp" 
#include "move_validator.hpp"

Board::Board(){
    grid.resize(8,std::vector<std::shared_ptr<Piece>>(8,nullptr));
    initialize();
}

void Board::simulateMove(int depRow, int depCol, int destRow, int destCol) {
    auto piece = grid[depRow][depCol];
    if (!piece) {
        //std::cerr << "[simulateMove] Error: No piece at source square (" << depRow << ", " << depCol << ")" << std::endl;
        return;
    }
    // } else {
    //     std::cout << "Moving a " << piece->color << " " << piece->name << std::endl;
    // }
    grid[destRow][destCol] = piece;
    grid[depRow][depCol] = nullptr;
}

void Board::initialize(){
    //Place Pawns
    for (int col = 0; col < 8; ++col) {
        grid[1][col] = std::make_shared<Piece>("pawn", "black", "b_P", 1, 1, col);
        grid[6][col] = std::make_shared<Piece>("pawn", "white", "w_P", 1, 6, col);
    }

    // Place rooks
    grid[0][0] = std::make_shared<Piece>("rook", "black", "b_R", 5, 0, 0);
    grid[0][7] = std::make_shared<Piece>("rook", "black", "b_R", 5, 0, 7);
    grid[7][0] = std::make_shared<Piece>("rook", "white", "w_R", 5, 7, 0);
    grid[7][7] = std::make_shared<Piece>("rook", "white", "w_R", 5, 7, 7);

    // Knights
    grid[0][1] = std::make_shared<Piece>("knight", "black", "b_N", 3, 0, 1);
    grid[0][6] = std::make_shared<Piece>("knight", "black", "b_N", 3, 0, 6);
    grid[7][1] = std::make_shared<Piece>("knight", "white", "w_N", 3, 7, 1);
    grid[7][6] = std::make_shared<Piece>("knight", "white", "w_N", 3, 7, 6);

    // Bishops
    grid[0][2] = std::make_shared<Piece>("bishop", "black", "b_B", 3, 0, 2);
    grid[0][5] = std::make_shared<Piece>("bishop", "black", "b_B", 3, 0, 5);
    grid[7][2] = std::make_shared<Piece>("bishop", "white", "w_B", 3, 7, 2);
    grid[7][5] = std::make_shared<Piece>("bishop", "white", "w_B", 3, 7, 5);

    // Queens
    grid[0][3] = std::make_shared<Piece>("queen", "black", "b_Q", 9, 0, 3);
    grid[7][3] = std::make_shared<Piece>("queen", "white", "w_Q", 9, 7, 3);

    // Kings
    grid[0][4] = std::make_shared<Piece>("king", "black", "b_K", 1000, 0, 4);
    grid[7][4] = std::make_shared<Piece>("king", "white", "w_K", 1000, 7, 4);
}

void Board::movePiece(const std::string& algebraic_move, const std::string &color){
    MoveValidator::MoveResult result = MoveValidator::isvalidMove(*this,algebraic_move,color);
    if (!result.valid){
        std::cout << result.reason << std::endl;
        return;
    }

    MoveValidator::ParsedSAN parsed_move = MoveValidator::parseSAN(algebraic_move,color,*this); 
    int depCol = parsed_move.deptFile - 'a';
    int depRow = 8 - parsed_move.deptRank; 

    int destCol = parsed_move.destFile - 'a';
    int destRow = 8 - parsed_move.destRank;

    auto movingPiece = grid[depRow][depCol];

    // Handle castling
    if (parsed_move.isCastle) {
        int row = (color == "white") ? 7 : 0;
        int kingStartCol = 4;
        int rookStartCol = (destCol == 6) ? 7 : 0;  // kingside = h-file, queenside = a-file
        int rookDestCol = (destCol == 6) ? 5 : 3;

        // Move king
        auto king = grid[row][kingStartCol];
        grid[row][destCol] = king;
        grid[row][kingStartCol] = nullptr;

        king->moveHistory.push_back({row, destCol});

        // Move rook
        auto rook = grid[row][rookStartCol];
        grid[row][rookDestCol] = rook;
        grid[row][rookStartCol] = nullptr;
        rook->moveHistory.push_back({row, rookDestCol});

        return;
    }
    
    if (!movingPiece){
        std::cout << "[BOARD] ERROR: Piece not instantiated at desired square!" << std::endl;
        return;
    }   

    if (parsed_move.isPromotion){
        std::string name;
        std::string displayName; 
        int point_val;
        switch (tolower(parsed_move.promotion_type)) {
                case 'p': name = "pawn"; break;
                case 'n': name = "knight"; break;
                case 'b': name = "bishop"; break;
                case 'r': name = "rook"; break;
                case 'q': name = "queen"; break;
                case 'k': name = "king"; break;
            }
        if (color == "white") {
            if (name == "pawn")      { displayName = "w_P"; point_val = 1; }
            else if (name == "knight"){ displayName = "w_N"; point_val = 3; }
            else if (name == "bishop"){ displayName = "w_B"; point_val = 3; }
            else if (name == "rook")  { displayName = "w_R"; point_val = 5; }
            else if (name == "queen") { displayName = "w_Q"; point_val = 9; }
            else if (name == "king")  { displayName = "w_K"; point_val = 1000; }
        } else {
            if (name == "pawn")      {displayName = "b_P"; point_val = 1; }
            else if (name == "knight"){ displayName = "b_N"; point_val = 3; }
            else if (name == "bishop"){ displayName = "b_B"; point_val = 3; }
            else if (name == "rook")  { displayName = "b_R"; point_val = 5; }
            else if (name == "queen") { displayName = "b_Q"; point_val = 9; }
            else if (name == "king")  { displayName = "b_K"; point_val = 1000; }
        } 
        auto promotionPiece = std::make_shared<Piece>(name, color, displayName, point_val, destRow, destCol); 
        promotionPiece->moveHistory = movingPiece->moveHistory;
        grid[destRow][destCol] = promotionPiece;
        grid[depRow][depCol] = nullptr; 
    } else{
        grid[destRow][destCol] = movingPiece;
        grid[depRow][depCol] = nullptr;
    }

    std::pair<int, int> lastMove = {destRow,destCol};
    movingPiece->moveHistory.push_back(lastMove);
}

// Display functions 
void Board::printFileLabels() {
    const std::string files = "abcdefgh";
    const int cellWidth = 6;  // includes the '|' or '+'

    std::cout << "    ";  // match the space before the grid starts
    for (char f : files) {
        int leftPad = (cellWidth - 1) / 2;  // center character in the cell
        std::cout << std::string(leftPad, ' ') << f << std::string(cellWidth - 1 - leftPad, ' ');
    }
    std::cout << "\n";
}

void Board::printBoard() {
    const std::string files = "abcdefgh";
    const int cellWidth = 6;

    printFileLabels();  // top
    std::cout << "  +" << std::string(8 * cellWidth - 1, '-') << "+\n";

    for (int row = 0; row < 8; ++row) {
        std::cout << 8 - row << " |";
        for (int col = 0; col < 8; ++col) {
            if (grid[row][col]) {
                std::cout << std::left << std::setw(cellWidth - 1)
                          << grid[row][col]->displayName << "|";
            } else {
                std::cout << std::string(cellWidth - 1, ' ') << "|";
            }
        }
        std::cout << " " << 8 - row << "\n";
        std::cout << "  +" << std::string(8 * cellWidth - 1, '-') << "+\n";
    }

    printFileLabels();  // bottom
}

std::pair<int, int> Board::parseSquare(const std::string& square) {
    if (square.length() != 2) throw std::invalid_argument("Invalid square format");

    char file = square[0];
    char rank = square[1];

    int col = file - 'a';         // 'a' to 'h' → 0 to 7
    int row = 8 - (rank - '0');   // '1' to '8' → 7 to 0

    if (col < 0 || col > 7 || row < 0 || row > 7)
        throw std::out_of_range("Square out of bounds");

    return {row, col};
}

void Board::placePiece(const std::string& square, const std::string& name, const std::string& color) {
    auto parsed = parseSquare(square);
    int row = parsed.first; int col = parsed.second; 

    std::string displayName;
    int point_val = 0;
    if (color == "white") {
        if (name == "pawn")      { displayName = "w_P"; point_val = 1; }
        else if (name == "knight"){ displayName = "w_N"; point_val = 3; }
        else if (name == "bishop"){ displayName = "w_B"; point_val = 3; }
        else if (name == "rook")  { displayName = "w_R"; point_val = 5; }
        else if (name == "queen") { displayName = "w_Q"; point_val = 9; }
        else if (name == "king")  { displayName = "w_K"; point_val = 1000; }
    } else {
        if (name == "pawn")      { displayName = "b_P"; point_val = 1; }
        else if (name == "knight"){ displayName = "b_N"; point_val = 3; }
        else if (name == "bishop"){ displayName = "b_B"; point_val = 3; }
        else if (name == "rook")  { displayName = "b_R"; point_val = 5; }
        else if (name == "queen") { displayName = "b_Q"; point_val = 9; }
        else if (name == "king")  { displayName = "b_K"; point_val = 1000; }
    }

    auto piece = std::make_shared<Piece>(name, color, displayName, point_val, row, col);
    bool hasMoved = check_hasMoved(name, color, row, col, "");
    //initting the piece will only add 1 move to the history ... if it has moved we need to add the original position 
    //so the array is length 2 
    if (hasMoved) {
        int originalRow = (color == "white") ? 7 : 0;
        int originalCol = col;

        if (name == "pawn") {
            originalRow = (color == "white") ? 6 : 1;
            // originalCol stays the same
        } else if (name == "rook") {
            originalCol = (std::abs(col - 0) < std::abs(col - 7)) ? 0 : 7;
        } else if (name == "knight") {
            originalCol = (std::abs(col - 1) < std::abs(col - 6)) ? 1 : 6;
        } else if (name == "bishop") {
            originalCol = (std::abs(col - 2) < std::abs(col - 5)) ? 2 : 5;
        } else if (name == "queen") {
            originalCol = 3;
        } else if (name == "king") {
            originalCol = 4;
        }

        piece->moveHistory.insert(piece->moveHistory.begin(), {originalRow, originalCol});
    }

    grid[row][col] = piece;
}

std::string Board::loadFEN(const std::string& fen){
    clearBoard();  // Reset grid to empty

    std::istringstream iss(fen);
    std::string piecePlacement, activeColor, castling, enPassant;
    int halfmoveClock, fullmoveNumber;

    iss >> piecePlacement >> activeColor >> castling >> enPassant >> halfmoveClock >> fullmoveNumber;

    int row = 0, col = 0;
    for (char c : piecePlacement) {
        std::string color = isupper(c) ? "white" : "black";
        if (c == '/') {
            row++;
            col = 0;
        } else if (isdigit(c)) {
            col += c - '0';
        } else {
            std::string name;
            switch (tolower(c)) {
                case 'p': name = "pawn"; break;
                case 'n': name = "knight"; break;
                case 'b': name = "bishop"; break;
                case 'r': name = "rook"; break;
                case 'q': name = "queen"; break;
                case 'k': name = "king"; break;
            }
            std::string displayName;            
            int point_val;
            if (color == "white"){
                if (name == "pawn"){displayName = "w_P"; point_val = 1;}
                if (name == "knight"){displayName = "w_N"; point_val = 3;} 
                if (name == "bishop"){displayName = "w_B"; point_val = 3;}
                if (name == "rook"){displayName = "w_R"; point_val = 5;} 
                if (name == "queen"){displayName = "w_Q"; point_val = 9;}
                if (name == "king"){displayName = "w_K"; point_val = 1000;} 
            } else {
                if (name == "pawn"){displayName = "b_P"; point_val = 1;}
                if (name == "knight"){displayName = "b_N"; point_val = 3;} 
                if (name == "bishop"){displayName = "b_B"; point_val = 3;}
                if (name == "rook"){displayName = "b_R"; point_val = 5;} 
                if (name == "queen"){displayName = "b_Q"; point_val = 9;}
                if (name == "king"){displayName = "b_K"; point_val = 1000;} 
            }
            char file = 'a' + col;
            int rank = 8 - row;
            //initting the piece will only add 1 move to the history ... if it has moved we need to add the original position 
            //so the array is length 2  

            auto piece = std::make_shared<Piece>(name, color, displayName, point_val, row, col);

            grid[row][col] = piece;
            col++;

            if (col > 8 || row > 7) {
                throw std::runtime_error("FEN parsing error: exceeded board bounds.");
            }
        }
    }
    std::cout << "Done loading FEN this is the board ... " << std::endl;
    std::string turn = (activeColor == "w") ? "white" : "black";
    return turn;
}

bool Board::check_hasMoved(const std::string& name, const std::string& color, int row, int col, const std::string& castling) {
    // Assume moved by default
    if (name == "pawn") return row != ((color == "white") ? 6 : 1);
    if (name == "king") return (color == "white") ? castling.find('K') == std::string::npos && castling.find('Q') == std::string::npos
                                                  : castling.find('k') == std::string::npos && castling.find('q') == std::string::npos;

    if (name == "rook") {
        if (color == "white") {
            if (row == 7 && col == 0) return castling.find('Q') == std::string::npos; // Queen-side
            if (row == 7 && col == 7) return castling.find('K') == std::string::npos; // King-side
        } else {
            if (row == 0 && col == 0) return castling.find('q') == std::string::npos;
            if (row == 0 && col == 7) return castling.find('k') == std::string::npos;
        }
    }

    return false;  // assume unmoved for other pieces
}


void Board::clearBoard(){
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            grid[row][col] = nullptr;
        }
    }
}