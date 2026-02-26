# Raysim

![C++](https://img.shields.io/badge/C%2B%2B-17-blue?style=flat&logo=cplusplus)
![CMake](https://img.shields.io/badge/CMake-3.19+-green?style=flat&logo=cmake)
![License](https://img.shields.io/badge/License-MIT-yellow?style=flat)
![Version](https://img.shields.io/badge/Version-0.2.4-brightgreen?style=flat)

A basic C++ framework for 2D graphics and interactive applications powered by [raylib](https://www.raylib.com/).

Inspired by **p5.js** and **Processing**, Raysim provides a simple API for drawing shapes, handling input, and managing time.

Useful for learning graphics programming, prototyping ideas, or creating small games and simulations.

## 📦 Download

To use Raysim, you can either clone the repository or download the latest release from GitHub:

```bash
git clone https://github.com/DMsuDev/Raysim.git
```

## ⚙️ Development

This project uses CMake for building. To set up the development environment, follow these steps:

There is also a Makefile for those who prefer using `make` directly.

1. **Install Dependencies**: Make sure you have CMake and a C++17 compatible compiler installed.
2. **Build the Project**: Navigate to the project directory and run the following commands:

```bash
mkdir build
cd build
cmake ..
make
```

> Note: Use `make help` to see available targets and options.
>
## 🚀 Usage

Basic usage involves including the `Raysim.hpp` header and linking against the Raysim library. Here's a simple example:

```cpp
#include <Raysim/Raysim.hpp>

class MyApp : public Canvas {
private:
    // ... your application state and methods here (or in public section if needed)

public:
    void Setup() override {
        // Initialization code here
    }

    void Update() override {
        // Update logic here
        // This is optional, but you can use it to update your application state before drawing.
        // If you don't need it, you can simply leave it empty or omit it entirely and set logic in draw() instead.
    }

    void Draw() override {
        // Drawing code here
    }
};

int main()
{
    MyApp app;
    app.Run();
    return 0;
}
```

> See the [examples](examples/) directory for sample applications and usage patterns.
