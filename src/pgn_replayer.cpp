// src/replay_pgn.cpp
#include "board.hpp"
#include "move_validator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>

std::string extractMovesSection(const std::string& pgnText) {
    std::istringstream stream(pgnText);
    std::ostringstream movesOnly;
    std::string line;

    bool inMoves = false;
    while (std::getline(stream, line)) {
        if (line.empty()) continue;
        if (line[0] == '[') continue; // Skip metadata lines
        inMoves = true;
        movesOnly << line << " ";
    }

    return movesOnly.str();
}

std::vector<std::string> extractMovesFromPGN(const std::string& pgnText) {
    std::vector<std::string> moves;
    std::regex movePattern(R"(\d+\.(\s*\S+)(?:\s+(\S+))?)");
    std::smatch match;
    std::string pgn = pgnText;

    while (std::regex_search(pgn, match, movePattern)) {
        if (match[1].matched) moves.push_back(match[1]);
        if (match[2].matched) moves.push_back(match[2]);
        pgn = match.suffix();
    }

    return moves;
}

bool isResultCode(const std::string& token) {
    return token == "1-0" || token == "0-1" || token == "1/2-1/2" || token == "*";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./replay_pgn <path_to_pgn_file>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Failed to open PGN file: " << argv[1] << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string pgnText = buffer.str();

    Board board;
    std::string currentColor = "white";

    std::string rawText = buffer.str();
    std::string movesText = extractMovesSection(rawText);
    auto moves = extractMovesFromPGN(movesText);

    for (const auto& move : moves) {
        if (isResultCode(move)) {
            std::cout << "Game ended with result: " << move << std::endl;
            if (move == "1-0") std::cout << "White wins by resignation or checkmate." << std::endl;
            else if (move == "0-1") std::cout << "Black wins by resignation or checkmate." << std::endl;
            else if (move == "1/2-1/2") std::cout << "Game drawn." << std::endl;
            break; // stop move processing
        }
        
        std::cout << currentColor << " plays: " << move << std::endl;
        auto result = MoveValidator::isvalidMove(board, move, currentColor);
        if (!result.valid) {
            std::cerr << "Invalid move: " << move << " Reason: " << result.reason << std::endl;
            return 1;
        }
        board.movePiece(move, currentColor);
        currentColor = (currentColor == "white") ? "black" : "white";
        //TO DO: check gameOver 
        board.printBoard();
        std::cout << "" << std::endl; 
    }
    
    std::cout << "Game replayed successfully.\n";
    return 0;
}
