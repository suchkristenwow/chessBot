#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "board.hpp"
#include "move_validator.hpp"

TEST_CASE("Basic Pawn Move", "[move]") {
    Board board;
    auto result = MoveValidator::isvalidMove(board, "e4", "white");
    REQUIRE(result.valid == true);
}

//Knight Move 
TEST_CASE("Illegal Knight Move", "[move]") {
    Board board;
    auto result = MoveValidator::isvalidMove(board, "Ne5", "white");
    REQUIRE(result.valid == false);
}

TEST_CASE("Valid Knight Move", "[move]") {
    Board board;
    auto result = MoveValidator::isvalidMove(board, "Nf3", "white");
    REQUIRE(result.valid == true);
}

//Rook Move - Check if the pieces phase through 
TEST_CASE("Illegal Rook Move","[move]"){
    Board board;
    auto result = MoveValidator::isvalidMove(board,"Rh3","white");
    REQUIRE(result.valid == false);
}

TEST_CASE("Valid Rook Move","[move]"){
    Board board;
    board.movePiece("h4","white");
    auto result = MoveValidator::isvalidMove(board,"Rh3","white");
    REQUIRE(result.valid == true);
    board.movePiece("Rh3","white");
    auto result0 = MoveValidator::isvalidMove(board,"Rc3","white"); 
    REQUIRE(result0.valid == true);
}

//Bishop Move - Correct square
TEST_CASE("Invalid bishop Move", "[move]"){
    Board board;
    //Move the pieces around a bit
    //REQUIRE(MoveValidator::isvalidMove(board, "e4", "white").valid == true); 
    board.movePiece("e4","white");
    //REQUIRE(MoveValidator::isvalidMove(board, "e5", "black").valid == true);
    board.movePiece("e5","black");
    auto result = MoveValidator::isvalidMove(board,"Bf5","white"); 
    REQUIRE(result.valid == false);
}

//Bishop Move - Invalid square 
TEST_CASE("Valid bishop Move", "[move]"){
    Board board;
    //Move the pieces around a bit 
    REQUIRE(MoveValidator::isvalidMove(board, "e4", "white").valid == true); 
    board.movePiece("e4","white");
    REQUIRE(MoveValidator::isvalidMove(board, "e5", "black").valid == true);
    board.movePiece("e5","black");
    auto result = MoveValidator::isvalidMove(board,"Bd3","white"); 
    REQUIRE(result.valid == true);
}

//Cant put yourself in Check 
TEST_CASE("Scholars Mate","[move]"){
    Board board;
    REQUIRE(MoveValidator::isvalidMove(board, "e4", "white").valid == true); 
    board.movePiece("e4","white");
    REQUIRE(MoveValidator::isvalidMove(board, "e5", "black").valid == true);
    board.movePiece("e5","black");
    REQUIRE(MoveValidator::isvalidMove(board, "bc4", "white").valid == true);
    board.movePiece("bc4","white");
    REQUIRE(MoveValidator::isvalidMove(board, "Nc6", "black").valid == true);
    board.movePiece("Nc6","black");
    REQUIRE(MoveValidator::isvalidMove(board, "Qh5", "white").valid == true);
    board.movePiece("Qh5","white"); 
    board.movePiece("f6","black");
    auto result = MoveValidator::isvalidMove(board,"f6","black"); 
    REQUIRE(result.valid == false);
}

TEST_CASE("Pawn backwards","[move]"){
    Board board;
    board.movePiece("e4","white");
    board.movePiece("e5","black"); 
    auto result = MoveValidator::isvalidMove(board,"e3","white"); 
    REQUIRE(result.valid == false);
}

TEST_CASE("Valid King-Side Castling","move"){
    Board board;
    board.movePiece("e4","white");
    board.movePiece("e5","black");
    board.movePiece("Bb5","white");
    board.movePiece("Bc5","black"); 
    board.movePiece("Nf3","white"); 
    board.movePiece("h5","black");
    auto result = MoveValidator::isvalidMove(board,"O-O","white"); 
    REQUIRE(result.valid == true);
}

TEST_CASE("Valid Queen-Side Castling","move"){
    Board board;
    board.movePiece("d4","white");
    board.movePiece("Nf6","black");
    board.movePiece("Nc3","white");
    board.movePiece("d5","black"); 
    board.movePiece("Qd3","white"); 
    board.movePiece("Qb6","black");
    auto result = MoveValidator::isvalidMove(board,"O-O-O","white");
    REQUIRE(result.valid == true);
}


TEST_CASE("INValid Castling","move"){
    Board board;
    board.movePiece("d4","white");
    board.movePiece("Nf6","black");
    board.movePiece("Nc3","white");
    board.movePiece("d5","black"); 
    board.movePiece("Qd3","white"); 
    board.movePiece("Qb6","black");
    board.movePiece("Rb1","white");
    board.movePiece("Ng8","black");
    auto result = MoveValidator::isvalidMove(board,"O-O-O","white");
    REQUIRE(result.valid == false);
}

TEST_CASE("Cant leave yourself in check","move"){
    Board board;
    board.movePiece("d4","white");
    board.movePiece("e5","black");
    board.movePiece("f4","white");
    board.movePiece("Qh4+","black"); 
    auto result = MoveValidator::isvalidMove(board,"c3","white");
    REQUIRE(result.valid == false);
}

TEST_CASE("Cant put yourself in Check","move"){
    Board board;
    board.movePiece("d4","white");
    board.movePiece("c5","black");
    board.movePiece("Nc3","white");
    board.movePiece("Qa5","black"); 
    auto result = MoveValidator::isvalidMove(board,"Nd4","white");
    REQUIRE(result.valid == false);
}