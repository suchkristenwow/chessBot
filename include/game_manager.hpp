#pragma once
#include "board.hpp"
#include <string>

class GameManager {
public:
    GameManager(const std::string& playerColor);

    void start();
    bool makeMove(const std::string& move);

private:
    Board board;
    std::string playerColor;
    std::string currentTurn;
};
