#pragma once
#include "board.hpp" 
#include "engine_utils.hpp"
#include <string>
#include <vector> 
#include <chrono>
#include "engines/heuristic.hpp" 

class GameManager {
    public:
        GameManager(const std::string& playerColor, const std::string& engineType = "heuristic");

        void start();
        bool makeMove(const std::string& move);

        std::vector<std::string> getAllLegalMoves(const std::string& color);

        bool gameOver = false;
        bool stalemate = false; 
        std::string winner = ""; 
        
        Board board; 
        bool checkGameOver(); 
        std::string currentTurn; 

    private:
        std::string playerColor;
        std::string engineType;

        void makePlayerMove(); 
        void makeComputerMove();
        bool gameOver_helper(const std::string& color);
};
