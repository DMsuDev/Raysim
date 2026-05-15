# System & Build Requirements

This document describes the requirements needed to build, run, and develop the **Raysim** framework, including its examples and auxiliary tools.

Raysim is a C++ framework for 2D graphics and real‑time interaction, built on top of [raylib](https://www.raylib.com/) as primary backend (Will be extended in the future) and managed through **CMake**, **Ninja**, and **vcpkg**.

---

## 🖥️ 1. System Requirements

Raysim officially supports:

- **Linux** (Debian/Ubuntu recommended)
- **Windows 10/11**
- **macOS** (not officially tested; expected to work in theory)

## 🔧 2. Required Tools

| Tool                                  | Minimum Version | Purpose                              |
| ------------------------------------- | --------------- | ------------------------------------ |
| **CMake**                             | 3.28+           | Project configuration and presets    |
| **C++ Compiler**                      | C++20           | Building the framework and examples  |
| **Ninja** (or any other build system) | Any             | Recommended build generator          |
| **Git**                               | 2.30+           | Repository cloning and submodules    |
| **Python 3**                          | 3.8+            | Auxiliary scripts (setup, utilities) |

### Supported Compilers

- **Linux**: GCC 10+, Clang 12+
- **Windows**: MSVC (Visual Studio 2022)

---

## 📚 3. External Dependencies

Raysim uses **vcpkg** as its package manager.  
Dependencies are defined in `vcpkg.json` and installed automatically via:

```bash
./tools/setup_all.sh       # Linux
.\tools\setup_all.ps1      # Windows
```
