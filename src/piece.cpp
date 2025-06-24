#include "piece.hpp"

Piece::Piece(
    const std::string& name,
    const std::string& color,
    const std::string& displayName,
    char file,
    int rank,
    int point_val,
    bool hasMoved,
    std::vector<std::pair<int, int>> moveHistory
) : name(name),
    color(color),
    displayName(displayName),
    algebraic_location(file, rank),
    point_val(point_val),
    hasMoved(hasMoved),
    moveHistory(moveHistory)
{
    int col = file - 'a';
    int row = 8 - rank;
    coord = {row, col};

    // Record spawn location in history if not already included
    if (moveHistory.empty()) {
        this->moveHistory.emplace_back(row, col);
    }
}

// 6-argument delegating constructor for convenience
Piece::Piece(
    const std::string& name,
    const std::string& color,
    const std::string& displayName,
    char file,
    int rank,
    int point_val
) : Piece(name, color, displayName, file, rank, point_val, false, {}) {}
