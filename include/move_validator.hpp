#pragma once 
#include "board.hpp"
#include "piece.hpp"
#include <regex> 
#include <string> 
#include <vector> 

namespace MoveValidator{
    struct MoveResult{
        bool valid;
        std::string reason;
        static MoveResult success() {return {true,""};}
        static MoveResult error(const std::string& msg){return {false,msg};}
    }
    struct ParsedSAN{
        char pieceType;
        bool isCapture;
        char disambiguationFile;
        char disambiguationRank;
        char destFile;
        char destRank;
        bool isPromotion;
        bool isCheck;
        bool isMate; 
    }

    MoveResult isvalidMove(const Board& board, const std::string& move,const std::string& currentTurn);
    bool isvalidNotation(const std::string& move);
    bool isvalidPattern(const Board& board, const std::string& move);
    ParsedSAN parseSAN(const std::string& move);
    bool pieceCanReach();
}