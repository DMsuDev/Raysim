# Raysim

![C++](https://img.shields.io/badge/C%2B%2B-17-blue?style=flat&logo=cplusplus)
![CMake](https://img.shields.io/badge/CMake-3.19+-green?style=flat&logo=cmake)
![License](https://img.shields.io/badge/License-MIT-yellow?style=flat)
![Version](https://img.shields.io/badge/Version-0.2.1-brightgreen?style=flat)

A basic C++ framework for 2D graphics and interactive applications powered by [raylib](https://www.raylib.com/).

Inspired by **p5.js** and **Processing**, Raysim provides a simple API for drawing shapes, handling input, and managing time.

Useful for learning graphics programming, prototyping ideas, or creating small games and simulations.

## 📦 Integration

Add Raysim to your CMake project:

```cmake
include(FetchContent)

FetchContent_Declare(
    Raysim
    GIT_REPOSITORY https://github.com/DMsuDev/Raysim.git
    GIT_TAG main
)
FetchContent_MakeAvailable(Raysim)

add_executable(myapp main.cpp)
target_link_libraries(myapp Raysim)
```

## Usage

> See the [examples](examples/) directory for sample applications and usage patterns.
