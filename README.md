# Advent of Code

[Advent of Code][aoc] is made by [Eric Wastl](http://was.tl/).  The following
description is from the Advent of Code [About
page](https://adventofcode.com/2020/about):

> [Advent of Code][aoc] is an Advent calendar of small programming puzzles for
a variety of skill sets and skill levels that can be solved in
[any](https://github.com/search?q=advent+of+code) programming language you
like.  People use them as a [speed
contest](https://adventofcode.com/leaderboard),
[interview](https://y3l2n.com/2018/05/09/interview-prep-advent-of-code/)
[prep](https://twitter.com/dznqbit/status/1037607793144938497), [company
training](https://twitter.com/pgoultiaev/status/950805811583963137),
[university](https://gitlab.com/imhoffman/fa19b4-mat3006/wikis/home)
[coursework](https://www.gribblelab.org/scicomp2019/),
[practice](https://twitter.com/mrdanielklein/status/936267621468483584)
[problems](https://comp215.blogs.rice.edu/), or to [challenge each
other](https://www.reddit.com/r/adventofcode/search?q=flair%3Aupping&restrict_sr=on).

This repository contains the code that `rhymu8354` [
[Twitch](https://twitch.tv/rhymu8354) |
[YouTube](https://youtube.com/rhymu8354) |
[github](https://github.com/rhymu8354) ] came up with for the puzzles he
attempted to solve.  Each year's code is contained in a deparate directory
named after the year.

* 2019 - Solutions attempted in C++
* 2020 - Solutions attempted in Rust

[aoc]: https://adventofcode.com/

## Usage

In general, each puzzle has a corresponding binary program built to solve it,
and the solution is printed to the standard output stream, with no command line
arguments required, but text input required.  This input may need to be piped
to standard input, or be available as a file named `input.txt` in the current
working directory when the programis run.  Alongside the code for each puzzle
is an `input.txt` file containing the input that rhymu8354 got for the
corresponding puzzle.

## Supported platforms / recommended toolchains

Solutions written in C++ should depend only on the C++11 compiler and the C and
C++ standard libraries, and so should be supported on almost any platform.  The
following are recommended toolchains for popular platforms.

* Windows -- [Visual Studio](https://www.visualstudio.com/) (Microsoft Visual
  C++)
* Linux -- clang or gcc
* MacOS -- Xcode (clang)

Solutions wirtten in Rust should only require the standard, stable Rust
toolchain, along with publicly-available Rust crates found on [crates.io](https://crates.io).

## Building

This project may be used stand-alone or as part of a larger project.

The included `CMakeLists.txt` file is intended to be used with
[CMake](https://cmake.org/) to generate the build system and build the
solutions written in C++.  There are two distinct steps in the build process
for the C++ solutions:

1. Generation of the build system, using CMake
2. Compiling, linking, etc., using CMake-compatible toolchain

For solutions written in Rust, a `Cargo.toml` file is provided which describes
a top-level Rust workspace.  Workspace members should be able to stand alone
and be reused in other workspaces as needed.

### Prerequisites

For the C++ solutions, a C++11 toolchain is required.  It's recommended to
leverage [CMake] to construct a suitable build system for your platform,
environment, and tools:

* [CMake] version 3.8 or newer
* C++11 toolchain compatible with CMake for your development platform (e.g.
  [Visual Studio](https://www.visualstudio.com/) on Windows)

For the Rust solutions, the standard Rust toolchain is required, along with any
crates upon which the solutions may depend.  It's recommended to use the
[rustup](https://rustup.rs/) and [cargo] tools to install the toolchain and
build the solutions.  The [cargo] tool should take care of downloading and
building any prerequisites besides the tools themselves.

[CMake]: https://cmake.org/
[cargo]: https://doc.rust-lang.org/cargo/

### Build system generation

For the C++ solutions, generate the build system using
[CMake](https://cmake.org/) from the solution root.  For example:

```bash
mkdir build
cd build
cmake -G "Visual Studio 15 2017" -A "x64" ..
```

### Compiling, linking, et cetera

For the C++ solutions, either use [CMake](https://cmake.org/) or your
toolchain's IDE to build.  For [CMake](https://cmake.org/):

```bash
cd build
cmake --build . --config Release
```

For the Rust solutions, use the [cargo](https://doc.rust-lang.org/cargo/) tool
to build, as well as to build/run individual programs:

```bash
cargo build
cargo build --bin aoc-2020-01-1
cargo run --bin aoc-2020-01-1
```

## License

Licensed under the [MIT license](LICENSE.txt).
