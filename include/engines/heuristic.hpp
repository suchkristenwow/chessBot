// include/engines/heuristic.hpp
#pragma once
#include "engine_utils.hpp"
#include "board.hpp"
#include <string>

namespace Engines {
    namespace Heuristic {
        std::string chooseMove(const Board& board, const std::string& color);
    }
}