#pragma once
#include "board.hpp"
#include <string>
#include <vector> 

class GameManager {
    public:
        GameManager(const std::string& playerColor);

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
        std::vector<std::string> getAllPawnMoves(int row, int col, const std::string& color);
        std::vector<std::string> getAllKnightMoves(int row, int col, const std::string& color); 
        std::vector<std::string> getAllBishopMoves(int row, int col, const std::string& color);
        std::vector<std::string> getAllRookMoves(int row, int col, const std::string& color); 
        std::vector<std::string> getAllKingMoves(int row, int col, const std::string& color);
        std::vector<std::string> getAllQueenMoves(int row, int col, const std::string& color); 

        void makePlayerMove(); 
        void makeComputerMove();
        bool gameOver_helper(const std::string& color);
};
