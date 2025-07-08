#include "game_manager.hpp"
#include "move_validator.hpp" 

GameManager::GameManager(const std::string& playerColor, const std::string& engineType) :
    playerColor(playerColor), currentTurn("white"), engineType(engineType) {}

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
    //TO DO switch cases for different engines 
    auto start = std::chrono::high_resolution_clock::now();
    move = Engines::Heuristic::chooseMove(board,currentTurn); 
    auto finish = std::chrono::high_resolution_clock::now(); 
    std::cout << "Search took: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
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

    //std::cout << "kingRow: "<< kingRow << " " << "kingCol: " << kingCol << std::endl;
    if (kingRow == -1 || kingCol == -1){
        std::cout << "no " << color << " King found!" << std::endl;
        winner = opponentColor;
        gameOver = true; 
        return true; 
    }
    
    bool InCheck = MoveValidator::isKingInCheck(board, color);
    // if (InCheck){
    //     std::cout << color << " is in Check!" << std::endl;
    // } 
    // else {
    //     std::cout << color << " is NOT in Check!" << std::endl;
    // }

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
    //std::cout << "[checkGameOver:GameManager] Checking if game over ... " << std::endl;
    if (gameOver_helper("white") || gameOver_helper("black")){
        return true; 
    }
    return false;
}

std::vector<std::string> GameManager::getAllLegalMoves(const std::string& color) {
    return EngineUtils::getAllLegalMoves(board, color);
}