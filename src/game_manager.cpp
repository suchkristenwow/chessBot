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
        //Check if winner 
        if (currentTurn == "white") {
            currentTurn = "black";
        } else{
            currentTurn = "white";
        }
    }
}

void GameManager::makeComputerMove(){
    // This function is responsible for making moves for the computer 
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
            board.movePiece(move);
            break;
        }
    }
    
}