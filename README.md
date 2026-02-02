# 3D Chess Visualization

A 3D chess visualization application built with OpenGL. Renders a textured chess board and full set of chess pieces with programmable lighting and spherical camera controls.

## Overview

This project implements a real-time 3D chess scene using OpenGL 3.3 Core Profile. It loads a stone chess board and wooden chess pieces from OBJ/3DS models, applies textures and Phong-style shading, and provides interactive camera and lighting controls.

## Tech Stack

- **Language:** C++
- **Graphics:** OpenGL 3.3 (Core Profile)
- **Windowing & Input:** GLFW 3.1
- **OpenGL Loader:** GLEW 1.13
- **Math:** GLM 0.9.7
- **Model Loading:** Assimp 3.0 (chess pieces), custom OBJ loader (board)
- **Build:** CMake 3.0+

## Project Structure

```text
3D_ChessGame/
├── CMakeLists.txt           # Root CMake configuration
├── common/                  # Shared utilities and rendering helpers
│   ├── controls.cpp/hpp     # Camera (spherical) and lighting controls
│   ├── objloader.cpp/hpp    # OBJ/Assimp loading, ChessPiece class
│   ├── shader.cpp/hpp       # Shader compilation and linking
│   ├── texture.cpp/hpp     # Texture loading (BMP, etc.)
│   └── vboindexer.cpp/hpp   # VBO indexing for meshes
├── external/                # Third-party libs (GLFW, GLEW, GLM, Assimp, etc.)
└── Lab3/
    ├── src/main.cpp         # Application entry and render loop
    ├── shaders/             # Vertex and fragment shaders
    │   ├── StandardShading.vertexshader
    │   └── StandardShading.fragmentshader
    ├── Chess/               # Chess piece models and textures
    │   ├── chess.obj, chess.3ds, chess.mtl
    │   └── wooddark*.jpg, woodlight*.jpg, etc.
    └── Stone_Chess_Board/   # Board model and textures
        ├── 12951_Stone_Chess_Board_v1_L3.obj
        └── 12951_Stone_Chess_Board_diff.bmp
```

## Features

- **3D chess board** with stone-style texture and indexed mesh rendering
- **Full chess set** (pawns, rooks, knights, bishops, king, queen) for both sides, loaded via Assimp
- **Phong-style shading** with configurable light position and toggle
- **Spherical camera:** orbit and zoom via keyboard and mouse
- **Lighting toggle** to enable/disable scene lighting at runtime

## Controls

| Input | Action |
| ------ | ------ |
| **Mouse** | Look / rotate camera (spherical angles) |
| **W / S** | Zoom in / out (decrease / increase radial distance) |
| **A / D** | Rotate camera horizontally |
| **↑ / ↓** | Rotate camera vertically |
| **L** | Toggle lighting on/off |
| **ESC** | Exit application |

## Building

### Prerequisites

- CMake 3.0 or newer
- C++ compiler with OpenGL support
- OpenGL development libraries
- The `external/` directory populated with:
  - GLFW 3.1.2
  - GLEW 1.13.0
  - GLM 0.9.7.1
  - Assimp 3.0.1270
  - AntTweakBar 1.16 (for launcher modules; optional depending on build)

### Build Steps

1. **Create a build directory** (do not build in the source root):

   ```bash
   mkdir build
   cd build
   ```

2. **Configure and build:**

   ```bash
   cmake ..
   cmake --build .
   ```

3. **Run the application:**

   The executable `Lab3` is configured to use `Lab3/` as the working directory so shaders and assets are found. Run from the build directory, for example:

   ```bash
   ./Lab3/Lab3
   ```

   Or run the target from your IDE; the project is set up so the working directory is `Lab3/`.

### Notes

- If the source or build path contains spaces, CMake may warn; avoid spaces if you run into issues.
- Shaders and assets under `Lab3/shaders`, `Lab3/Chess`, and `Lab3/Stone_Chess_Board` are copied into the build tree by CMake.

## Author & Course

- **Author:** Ruiyang Li    
- **Last modified:** October 26, 2024

## License

See repository or course materials for license information.
