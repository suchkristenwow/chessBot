cmake_minimum_required(VERSION 3.10)
project(chess_bot)

set(CMAKE_CXX_STANDARD 14)
find_package(Catch2 REQUIRED)

include_directories(include
                    external)

add_executable(chess_bot
    main.cpp
    src/board.cpp
    src/piece.cpp
    src/move_validator.cpp 
    src/game_manager.cpp
)


# Executable for test_gameOver
add_executable(test_gameOver
    tests/test_gameOver.cpp
    src/board.cpp
    src/piece.cpp
    src/move_validator.cpp 
    src/game_manager.cpp
)

# Executable for test_move_validator
add_executable(test_move_validator
    tests/test_move_validator.cpp
    src/board.cpp
    src/piece.cpp
    src/move_validator.cpp 
    src/game_manager.cpp
)

add_executable(replay_pgn
    src/pgn_replayer.cpp
    src/board.cpp
    src/piece.cpp
    src/move_validator.cpp
)
target_include_directories(replay_pgn PRIVATE include)

enable_testing()

# Link and register tests
target_link_libraries(test_gameOver Catch2::Catch2)
add_test(NAME GameOverTest COMMAND test_gameOver)

target_link_libraries(test_move_validator Catch2::Catch2)
add_test(NAME MoveValidatorTest COMMAND test_move_validator)
