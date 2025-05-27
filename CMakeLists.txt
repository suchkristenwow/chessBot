cmake_minimum_required(VERSION 3.10)
project(chess_bot)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(chess_bot
    main.cpp
    src/board.cpp
    src/piece.cpp
)
