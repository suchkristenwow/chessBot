#pragma once 

#include <string> 
#include <vector> 

class Piece {
public:
    std::string name;
    std::string color;
    std::string displayName;
    std::pair<char, int> algebraic_location;
    std::pair<int, int> coord;
    int point_val;
    bool hasMoved;
    std::vector<std::pair<int, int>> moveHistory;

    Piece(const std::string& name,
          const std::string& color,
          const std::string& displayName,
          char file,
          int rank,
          int point_val,
          bool hasMoved,
          std::vector<std::pair<int, int>> moveHistory);

    Piece(const std::string& name,
          const std::string& color,
          const std::string& displayName,
          char file,
          int rank,
          int point_val);
};