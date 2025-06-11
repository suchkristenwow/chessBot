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
        void movePiece(const std::string& algebraic_move);
    private:
        void printFileLabels();
};