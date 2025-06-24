#include <iostream> 
#include <string>
#include "game_manager.hpp"
#include "board.hpp"

void runTests() {
    std::cout << "[TEST MODE] Placeholder for running scripted tests.\n";
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--test") {
        runTests();
        return 0;
    }

    std::string playerColor;
    while (true){
        std::cout << "Choose your color (white/black) " << std::endl;
        std::cin >> playerColor; 
        if (playerColor == "white" || playerColor == "black"){break;}
        std::cout << "Invalid color. Please write white or black.\n" << std::endl;
    }

    GameManager game(playerColor);
    game.start();

    return 0;
}
