# ChessBot

**ChessBot** is a modular C++14 chess engine built from scratch to explore search algorithms, evaluation heuristics, and system design in a self-contained game engine. This project emphasizes both correctness and performance, and serves as a sandbox for experimenting with ideas in AI planning, game logic, and object-oriented design.

> 🔗 Full design breakdown on my blog: [suchkristenwow.github.io](https://suchkristenwow.github.io)

---

Currently, the bot employs the same features as Mac Hack - the first chess bot to successfully play in tournaments back in 1966.

## Features

- Full board representation and FEN parsing
- Legal move generation for all piece types, including special rules (castling, en passant)
- Static evaluation heuristics (material, position tables, king safety)
- Move ordering and priority scoring
- Simple search and move selection framework
- Zobrist hashing for board state caching
- Unit tests using Catch2

---

## Project Goals

- Learn best practice in C++
- Explore search and evaluation techniques
- Simulate production-grade system modularity in a game environment
- Develop intuition for optimization, caching, and performance tuning

---

## Getting Started

```bash
mkdir build && cd build
cmake ..
make
./chess_bot
```

### File Structure
- src/: core engine logic (Board, Piece, MoveValidator, GameManager, etc.)
- engines/: move selection logic (e.g., Heuristic engine)
- include/: headers for all modules
- tests/: unit tests with Catch2
- config/: YAML config for engine settings 

### Blog Write-Up

For an in-depth write-up of how I structured the engine, implemented SAN move parsing, and handled evaluation challenges, see my blog:

📖 https://suchkristenwow.github.io/blog/ 