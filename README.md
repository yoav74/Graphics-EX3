# Exercise 3 – OpenGL Car Viewer

An OpenGL application that renders a 3D car model with interactive viewing controls.

## Requirements

- WSL (Windows Subsystem for Linux) with Ubuntu
- g++ compiler
- OpenGL, GLU, and freeglut libraries

Install dependencies:
```bash
sudo apt install build-essential libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev
```

## Build & Run

```bash
make
./ex3
```

```bash
make clean   # remove build artifacts
```

## Controls

| Input | Action |
|-------|--------|
| Left mouse drag | Rotate model (virtual trackball) |
| ESC | Quit |

## Project Structure

```
ex3/
├── main.cpp        # GLUT setup and event callbacks
├── trackball.cpp   # Virtual trackball rotation math
├── trackball.h
├── drawing.cpp     # Scene drawing functions
├── drawing.h
└── Makefile
```

## Contributing

1. Each logical component has its own `.h`/`.cpp` pair — keep it that way when adding new features.
2. Build and verify with `make` before committing.
3. The car model will be built incrementally — add one element at a time and confirm it renders correctly before moving on.
