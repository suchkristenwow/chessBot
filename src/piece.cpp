#include "piece.hpp"

Piece::Piece(
    const std::string& name,
    const std::string& color,
    const std::string& displayName,
    int point_val,
    int initRow,
    int initCol
) : name(name),
    color(color),
    displayName(displayName),
    point_val(point_val)
{
    moveHistory.emplace_back(initRow, initCol);
}
