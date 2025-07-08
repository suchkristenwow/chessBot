// engines/heuristic.cpp
#include "engines/heuristic.hpp"

namespace Engines {
    const int DEPTH = 3;
    namespace Heuristic {
        std::string chooseMove(const Board& board, const std::string& color) {
            auto legalMoves = EngineUtils::getAllLegalMoves(board,color);
            if (legalMoves.empty()) return ""; //no legal moves
            std::string bestMove; 
            int bestEval = -1e8; 
            int d = DEPTH;
            for (const auto&move : legalMoves){
                Board simulatedBoard = board;
                simulatedBoard.movePiece(move,color);
                //std::cout << "Choosing move ... this is depth: " << d-1 << std::endl;
                int eval = EngineUtils::alphaBeta(simulatedBoard,d-1,-1e8,1e8,false,color); 
                if (eval > bestEval){
                    bestEval = eval;
                    bestMove = move; 
                    //std::cout << "Move: " << move << ", Eval: " << eval << std::endl;
                }
            }
            return bestMove; 
        }
    } //END HEURISTIC NAMESPACE
}