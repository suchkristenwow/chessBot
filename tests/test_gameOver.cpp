#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "board.hpp"
#include "game_manager.hpp"

TEST_CASE("Checkmate Detection", "[gameover]") {
    GameManager gm("white");

    gm.board.movePiece("e4", "white");
    gm.board.movePiece("f6", "black");
    gm.board.movePiece("d4", "white");
    gm.board.movePiece("g5", "black");
    gm.board.movePiece("Qh5#", "white");
    gm.currentTurn = "black"; 

    gm.checkGameOver(); 

    REQUIRE(gm.gameOver == true);
    REQUIRE(gm.winner == "white");
    REQUIRE(gm.stalemate == false);
}

TEST_CASE("Stalemate Detection", "[gameover]") {
    GameManager gm("white");

    gm.board.loadFEN("7k/5Q2/6K1/8/8/8/8/8 b - - 0 1"); // stalemate

    gm.board.printBoard();

    gm.checkGameOver();

    REQUIRE(gm.gameOver == true);
    REQUIRE(gm.stalemate == true);
    REQUIRE(gm.winner == "");
}

TEST_CASE("Game Not Over", "[gameover]") {
    GameManager gm("white");

    gm.board.movePiece("e4", "white");
    gm.board.movePiece("e5", "black");

    gm.checkGameOver();

    REQUIRE(gm.gameOver == false);
    REQUIRE(gm.stalemate == false);
    REQUIRE(gm.winner == "");
}

TEST_CASE("King Captured Ends Game", "[gameover]") {
    GameManager gm("white");

    gm.board.clearBoard();
    gm.board.placePiece("e1", "king", "white");
    gm.board.placePiece("e8", "king", "black");
    gm.board.placePiece("e2", "queen", "black");

    gm.board.movePiece("Qxe1", "black");
    gm.currentTurn = "white"; 
    gm.checkGameOver();

    REQUIRE(gm.gameOver == true);
    REQUIRE(gm.winner == "black");
}
