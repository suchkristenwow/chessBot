#pragma once 
#include "piece.hpp"
#include <regex> 
#include <string> 
#include <vector> 
#include <unordered_map> 

class Board; 

namespace MoveValidator{
    struct MoveResult{
        bool valid;
        std::string reason;
        static MoveResult success() {return {true,""};}
        static MoveResult error(const std::string& msg){return {false,msg};}
    }; 

    struct ParsedSAN{
        char cdType;
        std::string pieceType;
        bool isCapture;
        char disambiguationFile;
        char disambiguationRank;
        char destFile;
        char destRank;
        char deptFile;
        char deptRank;
        bool isPromotion;
        bool isCheck;
        bool isMate; 
        bool isCastle;
    };

    MoveResult isvalidMove(const Board& board, const std::string& move,const std::string& currentTurn);
    bool isvalidNotation(const std::string& move);
    bool isvalidPattern(const Board& board, const std::string& move);
    ParsedSAN parseSAN(const std::string& move, const std::string& color, const Board& board);
    bool pieceCanReach(const Piece& piece,
                   int depRow,
                   int depCol,
                   int destRow,
                   int destCol,
                   bool isCapture,
                   const Board& board); 
    bool isKingInCheck(const Board&board, const std::string&color);
}