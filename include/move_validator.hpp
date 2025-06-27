#pragma once

#include "piece.hpp"
#include <regex>
#include <string>
#include <vector>
#include <unordered_map>
#include <set> 

class Board;

namespace MoveValidator {

    struct MoveResult {
        bool valid;
        std::string reason;
        static MoveResult success() { return {true, ""}; }
        static MoveResult error(const std::string& msg) { return {false, msg}; }
    };

    struct ParsedSAN {
        std::string pieceType;
        bool isCapture = false;
        char disambiguationFile = 0;
        char disambiguationRank = 0;
        char destFile = 0;
        char destRank = 0;
        char deptFile = 0;
        char deptRank = 0;
        bool isPromotion = false;
        bool isCheck = false;
        bool isMate = false;
        bool isCastle = false;
        char promotion_type;
    };

    // Main validation API
    MoveResult isvalidMove(const Board& board, const std::string& move, const std::string& currentTurn);

    // Parsing
    ParsedSAN parseSAN(const std::string& move, const std::string& color, const Board& board);
    bool isvalidNotation(const std::string& move);
    bool isvalidPattern(const Board& board, const ParsedSAN& parsed_move, const std::string& color);
    bool islegalMove(const Board& board, const ParsedSAN& parsed_move, const std::string& color);

    // Utility: movement and legality
    bool pieceCanReach(const Piece& piece, int depRow, int depCol, int destRow, int destCol, bool isCapture, const Board& board);
    bool isKingInCheck(const Board& board, const std::string& color);

    // parseSAN helpers
    std::string trim(const std::string& s);
    bool isCastling(const std::string& move);
    ParsedSAN parseCastling(const std::string& move, const std::string& color);
    ParsedSAN parsePattern(const std::smatch& m, const std::string& color);
    std::vector<std::pair<int, int>> findCandidatePieces(const ParsedSAN& parsed, const Board& board, const std::string& color);

} // namespace MoveValidator
