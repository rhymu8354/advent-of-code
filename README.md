# Advent of Code

(TODO: summarize what Advent of Code is, and how this workspace contains
solvers for the various puzzles.)

## Usage

    Usage: aoc1_1

    Start solving puzzle 1-1.


## Supported platforms / recommended toolchains

This is a portable C++11 program which depends only on the C++11 compiler, the
C and C++ standard libraries, and other C++11 libraries with similar
dependencies, so it should be supported on almost any platform.  The following
are recommended toolchains for popular platforms.

* Windows -- [Visual Studio](https://www.visualstudio.com/) (Microsoft Visual C++)
* Linux -- clang or gcc
* MacOS -- Xcode (clang)

## Building

This project may be used stand-alone or as part of a larger project.  Either
way, the included `CMakeLists.txt` file is intended to be used with
[CMake](https://cmake.org/) to generate the build system and build the project.

There are two distinct steps in the build process:

1. Generation of the build system, using CMake
2. Compiling, linking, etc., using CMake-compatible toolchain

### Prerequisites

* [CMake](https://cmake.org/) version 3.8 or newer
* C++11 toolchain compatible with CMake for your development platform (e.g. [Visual Studio](https://www.visualstudio.com/) on Windows)

### Build system generation

Generate the build system using [CMake](https://cmake.org/) from the solution root.  For example:

```bash
mkdir build
cd build
cmake -G "Visual Studio 15 2017" -A "x64" ..
```

### Compiling, linking, et cetera

Either use [CMake](https://cmake.org/) or your toolchain's IDE to build.
For [CMake](https://cmake.org/):

```bash
cd build
cmake --build . --config Release
```
