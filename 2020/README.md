# aoc-2020

This crate contains the solutions `rhymu8354` attempted for [Advent of Code
2020][aoc2020].

[aoc2020]: https://adventofcode.com/2020

## Usage

Each solution is a separate binary, with the name being `aoc-2020-XX-Y` where
`XX` is the day number and `Y` is the part (1 or 2).  Each binary is run
without any arguments, and outputs the solution to standard output.  Standard
input is required.  The input provided to `rhymu8354` is provided in
`input.txt` files that can be found alongside the code.

## Formatting

This crate uses a custom configuration for `rustfmt` which relies on unstable
features.  Attempting `cargo fmt` on the stable toolchain may fail.  Use `cargo
+nightly fmt` to format the code.

## License

Licensed under the [MIT license](LICENSE.txt).
