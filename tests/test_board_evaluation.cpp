#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "board.hpp"
#include "game_manager.hpp" 
#include "engine_utils.hpp"

TEST_CASE("KingRook", "[boardEval]") {
    GameManager gm("white");

    gm.board.loadFEN("R6k/7p/6p1/8/8/8/8/4K3 b - - 0 1");

    gm.board.printBoard();

    int whiteScore = EngineUtils::evaluateBoard(gm.board,"white");
    int blackScore = EngineUtils::evaluateBoard(gm.board,"black"); 

    REQUIRE(blackScore < whiteScore);
}

TEST_CASE("CoveredPromotion", "[boardEval]") {
    GameManager gm("white");

    gm.board.loadFEN("6rk/1P3pp1/7p/8/2Q5/8/8/4K3 w - - 0 1");

    gm.board.printBoard();

    int whiteScore = EngineUtils::evaluateBoard(gm.board,"white");
    int blackScore = EngineUtils::evaluateBoard(gm.board,"black"); 

    REQUIRE(blackScore < whiteScore);
}

TEST_CASE("Immortal", "[boardEval]") {
    GameManager gm("white");

    gm.board.loadFEN("b2r3r/k3qp1p/p4np1/NppP4/3p1Q2/P4PPB/1PP4P/1K1RR3 w - - 0 1");

    gm.board.printBoard();

    int whiteScore = EngineUtils::evaluateBoard(gm.board,"white");
    int blackScore = EngineUtils::evaluateBoard(gm.board,"black"); 

    REQUIRE(blackScore < whiteScore);
}

TEST_CASE("Opera", "[boardEval]") {
    GameManager gm("white");

    gm.board.loadFEN("rn1qkb1r/ppp2ppp/5n2/4p3/2B1P3/5Q2/PPP2PPP/RNB1K2R w KQkq - 0 1");

    gm.board.printBoard();

    int whiteScore = EngineUtils::evaluateBoard(gm.board,"white");
    int blackScore = EngineUtils::evaluateBoard(gm.board,"black"); 

    REQUIRE(blackScore < whiteScore);
}

TEST_CASE("FischerByrne56", "[boardEval]") {
    GameManager gm("white");

    gm.board.loadFEN("r2q1rk1/pp2ppbp/2p2np1/6B1/n2PP1b1/Q1N2N2/PP3PPP/3RKB1R w K - 0 1");

    gm.board.printBoard();

    int whiteScore = EngineUtils::evaluateBoard(gm.board,"white");
    int blackScore = EngineUtils::evaluateBoard(gm.board,"black"); 

    REQUIRE(blackScore < whiteScore);
}