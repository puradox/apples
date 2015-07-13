apples (placeholder title)
==========================
###### An OpenGL game that utilizes **Divvy** (see https://github.com/puradox/divvy)

## Requirements

1. **OpenGL**
2. **CMake** (http://www.cmake.org/)
3. **GLEW** (http://glew.sourceforge.net/)
4. **GLFW** (http://www.glfw.org/)

## Downloading

Simply running a clone on this repository will not clone it's dependancies.
Therefore, running

    git clone --recursive https://github.com/puradox/apples

will clone both this repository and the submodules that we need.

## Building

Simply run

    mkdir build
    cp -r scripts build/
    cd build
    cmake ..

Then build with your compiler

    make
