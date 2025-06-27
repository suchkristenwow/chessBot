#pragma once 

#include <string> 
#include <vector> 

class Piece {
public:
    std::string name;
    std::string color;
    std::string displayName;
    int point_val;
    std::vector<std::pair<int, int>> moveHistory;

    Piece(const std::string& name,
          const std::string& color,
          const std::string& displayName,
          int point_val,
          int initRow,
          int initCol);
};