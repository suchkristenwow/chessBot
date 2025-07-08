#include <iostream> 
#include <string>
#include "game_manager.hpp"
#include "board.hpp"

int main(int argc, char* argv[]) {
    std::string playerColor;
    std::string engineType;
    while (true){
        std::cout << "Choose your color (white/black) " << std::endl;
        std::cin >> playerColor; 
        if (playerColor == "white" || playerColor == "black"){break;}
        std::cout << "Invalid color. Please write white or black.\n" << std::endl;
        engineType = "heuristic";
        // std::cout << "Choose what engine you want to play with: heuristic" << std::endl;
        // std::cin >> engineType;
    }

    GameManager game(playerColor,engineType);
    game.start();

    return 0;
}
