#pragma once
#include "board.hpp"
#include "move_validator.hpp" 
#include <string> 

namespace EngineUtils {
    int evaluateBoard(const Board& board, const std::string& color);
    std::vector<std::string> getAllLegalMoves(const Board& board, const std::string& color);

    int alphaBeta(Board board, int depth, int alpha, int beta, bool maximizingPlayer, const std::string& rootColor);

    std::vector<std::string> getAllPawnMoves(const Board& board, int row, int col, const std::string& color);
    std::vector<std::string> getAllKnightMoves(const Board& board, int row, int col, const std::string& color); 
    std::vector<std::string> getAllBishopMoves(const Board& board, int row, int col, const std::string& color);
    std::vector<std::string> getAllRookMoves(const Board& board, int row, int col, const std::string& color); 
    std::vector<std::string> getAllKingMoves(const Board& board, int row, int col, const std::string& color);
    std::vector<std::string> getAllQueenMoves(const Board& board, int row, int col, const std::string& color); 

    extern const int pawn_table[8][8];
    extern const int knight_table[8][8];
    extern const int bishop_table[8][8];
    extern const int queen_table[8][8];
    extern const int rook_table[8][8];
    extern const int king_table[8][8];

    bool possiblePromotion(const Board& board, int row, int col, const std::string& color);
}
