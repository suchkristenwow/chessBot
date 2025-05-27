#include "board.hpp" 


Board::Board(){
    grid.resize(8,std::vector<std::shared_ptr<Piece>>(8,nullptr));
    initialize();
}

void Board::initialize(){
    const std::string files = "abcdefgh";
    //Place Pawns
    for (int col=0; col<8; ++col){
        char file = files[col];
        //black pawns
        //grid[1][col] = std::make_shared<Piece>(Piece{"pawn","black",{algebraic_col_map[col],7},{1,col},1}); <-struct inst.
        grid[1][col] = std::make_shared<Piece>("pawn","black","b_P",col,7,1);
        //white pawns
        grid[6][col] = std::make_shared<Piece>("pawn","white","w_P",col,2,1);
    }

    // Place rooks
    grid[0][0] = std::make_shared<Piece>("rook", "black", "b_R", 'a', 8, 5);
    grid[0][7] = std::make_shared<Piece>("rook", "black", "b_R", 'h', 8, 5);
    grid[7][0] = std::make_shared<Piece>("rook", "white", "w_R", 'a', 1, 5);
    grid[7][7] = std::make_shared<Piece>("rook", "white", "w_R", 'h', 1, 5);

    // Place knights
    grid[0][1] = std::make_shared<Piece>("knight", "black", "b_N", 'b', 8, 3);
    grid[0][6] = std::make_shared<Piece>("knight", "black", "b_N", 'g', 8, 3);
    grid[7][1] = std::make_shared<Piece>("knight", "white", "w_N", 'b', 1, 3);
    grid[7][6] = std::make_shared<Piece>("knight", "white", "w_N", 'g', 1, 3);

    // Place bishops
    grid[0][2] = std::make_shared<Piece>("bishop", "black", "b_B", 'c', 8, 3);
    grid[0][5] = std::make_shared<Piece>("bishop", "black", "b_B", 'f', 8, 3);
    grid[7][2] = std::make_shared<Piece>("bishop", "white", "w_B", 'c', 1, 3);
    grid[7][5] = std::make_shared<Piece>("bishop", "white", "w_B", 'f', 1, 3);

    // Place queens
    grid[0][3] = std::make_shared<Piece>("queen", "black", "b_Q", 'd', 8, 9);
    grid[7][3] = std::make_shared<Piece>("queen", "white", "w_Q", 'd', 1, 9);

    // Place kings
    grid[0][4] = std::make_shared<Piece>("king", "black", "b_K", 'e', 8, 1000);
    grid[7][4] = std::make_shared<Piece>("king", "white", "w_Q", 'e', 1, 1000);
}

void Board::movePiece(const std::string& algebraic_move){
    //TODO: implement moving logic
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
