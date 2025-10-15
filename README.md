# Hello Pyramid

![Demo](./demo.gif)

## My first OpenGL project 😃

I've been learning OpenGL from LearnOpenGL.com for three days, and this is the first thing I
created. (Yes, I made the triangle too, but that one didn’t fight back lol). Along the way, I also
picked up some basics of GPU architecture.

I know it’s a simple project, but it means a lot to me. It reminds me of how much effort I put in,
and maybe years from now, I’ll look back at this and see how far I’ve come. Keep grinding, my friend.

## Dependencies

- OpenGL 4.1 or higher
- C++ compiler
- CMake
- On Windows, ensure MinGW's g++ is installed and in your PATH.

## Build

### Linux / macOS
```bash
cmake -S . -B build
cmake --build build
./build/PyramidGL
```

### Windows (Command Prompt / PowerShell)
```bash
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
.\build\PyramidGL.exe
```
