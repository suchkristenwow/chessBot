#pragma once 
#include <tuple> 
#include <string> 

class Piece{
    public:
        //constructor for Piece
        Piece(const std::string& name,
            const std::string& color,
            const std::string& displayName,
            char file,
            int rank,
            int point_val,
            bool hasMoved = false);
        std::string name;
        std::string color;
        std::string displayName;
        std::tuple<char,int> algebraic_location;
        std::tuple<int,int> coord;
        int point_val;
        bool inPlay = true;
        bool hasMoved;
        std::vector<std::pair<int, int>> moveHistory;
};