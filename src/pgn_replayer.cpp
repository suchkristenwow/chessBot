// src/replay_pgn.cpp
#include "board.hpp"
#include "move_validator.hpp"
#include "game_manager.hpp"
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
    std::string rawText = buffer.str();
    std::string movesText = extractMovesSection(rawText);
    auto moves = extractMovesFromPGN(movesText);

    GameManager gm("none");  // 'none' disables user input logic
    std::string currentColor = "white";

    for (const auto& move : moves) {
        if (isResultCode(move)) {
            std::cout << "Game ended with result: " << move << std::endl;
            break;
        }

        std::cout << currentColor << " plays: " << move << std::endl;
        auto result = MoveValidator::isvalidMove(gm.board, move, currentColor);
        if (!result.valid) {
            std::cerr << "Invalid move: " << move << " Reason: " << result.reason << std::endl;
            return 1;
        }

        gm.board.movePiece(move, currentColor);
        gm.board.printBoard();

        if (gm.checkGameOver()) {
            std::cout << "[Replay] Detected game over after move: " << move << "\n";
            break;
        }

        currentColor = (currentColor == "white") ? "black" : "white";
        std::cout << "\n";
    }

    std::cout << "Game replayed successfully.\n";
    return 0;
}

