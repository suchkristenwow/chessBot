#include "piece.hpp"

Piece::Piece(
    const std::string& name,
    const std::string& color,
    const std::string& displayName,
    char file,
    int rank,
    int point_val,
    bool hasMoved
) : name(name),
    color(color),
    displayName(displayName),
    algebraic_location(file,rank),
    point_val(point_val),
    hasMoved(hasMoved)
{
    //convert 'a'-'h' into 0 index columns
    int col = file - 'a';
    int row = 8 - rank;
    coord = {row,col};

    moveHistory.emplace_back(row,col);
}