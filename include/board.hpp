#pragma once 
#include <memory>
#include <iostream> 
#include <vector> 
#include <iomanip> 

#include "piece.hpp" 
#include "move_validator.hpp"

class Board{
    public:
        Board();
        void initialize(); 
        void printBoard(); 
        std::vector<std::vector<std::shared_ptr<Piece>>> grid; 
        void movePiece(const std::string& algebraic_move, const std::string& color);
        void placePiece(const std::string& square, const std::string& name, const std::string& color);
        void simulateMove(int depRow, int depCol, int destRow, int destCol);
        std::string loadFEN(const std::string& fen);
        void clearBoard();
    private:
        bool check_hasMoved(const std::string& name, const std::string& color, int row, int col, const std::string& castling);
        void printFileLabels();
        std::pair<int,int> parseSquare(const std::string &square);
};