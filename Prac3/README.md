![gif](gif.gif)

# OpenGLBase
Base for C++ OpenGL Apps

# Requirements
[SDL2](https://www.libsdl.org/download-2.0.php) (lib files are included for Windows, must be installed on Linux/Mac)

# Included
[winmake](https://github.com/LAK132/winmake) (makefile-like compiling on Windows)

[dear-imgui](https://github.com/ocornut/imgui). Currently building against [v1.60](https://github.com/ocornut/imgui/releases/tag/v1.60)

[dear-imgui SDL2/gl3w implementation](https://github.com/ocornut/imgui/tree/master/examples/sdl_opengl3_example)

[gl3w](https://github.com/skaslev/gl3w)

[tiny-obj-loader](https://github.com/syoyo/tinyobjloader)

[glm](https://glm.g-truc.net/0.9.8/index.html)

# Building

## Windows

1. Make sure makelist.bat is set up for your compiler (see [winmake README](https://github.com/LAK132/winmake/blob/master/README.md))

2. Run `make debug x86` for debug on x86 system, `make release x64` for release on x64 system, etc.

## Linux

1. Install the SDL2

2. Run `make all`
