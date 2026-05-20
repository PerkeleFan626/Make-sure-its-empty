# Project Overview

`PingPong` is a modern recreation of the classic 1970s arcade game Pong. It is implemented in **C++20** using the **Raylib** library for graphics and input handling. The project features a special aesthetic theme for **Bully Awareness Month**, where the paddles and ball are colored hot pink.

The project is designed to be self-contained and avoids reliance on external shell commands for game logic, making it suitable for restricted environments.

## Technologies

- **Language:** C++20
- **Library:** Raylib (Graphics, Input, Audio)
- **Build System:** CMake (3.24+)

## Architecture

The project follows a simple, structured approach:
- **`Constants` Namespace:** Contains game-wide configuration like screen dimensions, speeds, and colors.
- **`Paddle` Struct:** Manages paddle position, movement logic, scoring, and rendering.
- **`Ball` Struct:** Handles ball physics, collision detection with screen boundaries, and resetting.
- **`main.cpp`:** Orchestrates the game lifecycle (initialization, main loop, cleanup), including collision detection between the ball and paddles.

# Building and Running

The project uses CMake and requires a C++20 compatible compiler.

### Build Commands

To build the project, use the following commands from the root directory:

```powershell
# Configure the project (replace <path_to_compiler> with your actual paths if not in PATH)
cmake -B build -G "Ninja" -DCMAKE_C_COMPILER="gcc.exe" -DCMAKE_CXX_COMPILER="g++.exe"

# Build the project
cmake --build build
```

*Note: The project uses `FetchContent` to automatically download and build Raylib from its GitHub repository (Commit: `c04e57399acfc860a68ac0c11ff6d1c77da9cfda`).*

### Running the Game

After building, the executable can be found in the `build/` directory:

```powershell
./build/PingPong.exe
```

# Development Conventions

- **C++20 Features:** Use modern C++ features such as `std::format`, designated initializers, and `constexpr`.
- **Naming Conventions:**
  - Structs/Classes: `PascalCase`
  - Variables/Members: `camelCase`
  - Constants: `PascalCase` (within namespaces)
- **Aesthetics:** Preserve the Bully Awareness Month theme by using `Constants::BullyAwarenessPink` for all primary game elements.
- **Headers:** The `header/` directory contains specific Raylib headers (`raylib.h`, `raymath.h`, `rlgl.h`, `config.h`) for manual reference, as requested.

# Usage

- **Player 1 (Left):** Use `W` and `S` keys to move.
- **Player 2 (Right):** Use `UP` and `DOWN` arrow keys to move.
- **Goal:** Bounce the ball past the opponent's paddle to score points.
