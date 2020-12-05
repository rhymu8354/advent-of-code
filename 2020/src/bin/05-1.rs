use aoc_2020::benchmarked_main;

fn decode(
    input: &str,
    one_flag: char,
) -> usize {
    input.chars().fold(0, |acc, ch| {
        (acc << 1)
            + if ch == one_flag {
                1
            } else {
                0
            }
    })
}

fn parse(input: &str) -> Vec<usize> {
    input
        .lines()
        .map(|line| {
            let line = line.trim();
            let row = decode(&line[..7], 'B');
            let col = decode(&line[7..], 'R');
            (row << 3) + col
        })
        .collect::<Vec<_>>()
}

fn solve_already_parsed<T: AsRef<[usize]>>(input: &T) -> Option<usize> {
    input.as_ref().iter().copied().max()
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
