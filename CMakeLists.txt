cmake_minimum_required(VERSION 3.10)
project(chess_bot)

set(CMAKE_CXX_STANDARD 14)
find_package(Catch2 REQUIRED)

add_library(chess_core 
    src/board.cpp
    src/piece.cpp
    src/move_validator.cpp 
    src/game_manager.cpp
    src/engine_utils.cpp) 

target_include_directories(chess_core PUBLIC include)

add_library(chess_engines
    engines/heuristic.cpp)

target_include_directories(chess_engines PUBLIC include)
target_link_libraries(chess_engines PUBLIC chess_core) 

target_link_libraries(chess_core PUBLIC chess_engines) 

add_executable(chess_bot
    main.cpp
)
target_link_libraries(chess_bot PRIVATE chess_engines)

# Executable for test_gameOver
add_executable(test_gameOver
    tests/test_gameOver.cpp
)
target_link_libraries(test_gameOver PRIVATE chess_engines Catch2::Catch2)
add_test(NAME GameOverTest COMMAND test_gameOver)

# Executable for test_move_validator
add_executable(test_move_validator
    tests/test_move_validator.cpp
)
target_link_libraries(test_move_validator PRIVATE chess_engines Catch2::Catch2)
add_test(NAME MoveValidatorTest COMMAND test_move_validator)

# Executable for test_move_validator
add_executable(test_board_evaluation
    tests/test_board_evaluation.cpp
)
target_link_libraries(test_board_evaluation PRIVATE chess_engines Catch2::Catch2)
add_test(NAME BoardEvaluationTest COMMAND test_board_evaluation)

add_executable(replay_pgn
    src/pgn_replayer.cpp
)
target_include_directories(replay_pgn PRIVATE include)
target_link_libraries(replay_pgn PRIVATE chess_engines)

enable_testing()
