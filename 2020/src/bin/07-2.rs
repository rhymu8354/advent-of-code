use aoc_2020::benchmarked_main;

fn parse(input: &str) -> Vec<Vec<char>> {
    input
        .lines()
        .map(|line| line.trim().chars().collect::<Vec<_>>())
        .collect::<Vec<_>>()
}

const SLOPE_X: usize = 3;

fn try_path(
    map: &[Vec<char>],
    cols: usize,
) -> usize {
    map.iter()
        .enumerate()
        .filter(|(i, row)| row[(i * SLOPE_X) % cols] == '#')
        .count()
}

fn solve_already_parsed<T: AsRef<[Vec<char>]>>(map: &T) -> Option<usize> {
    let map = map.as_ref();
    if let Some(row) = map.get(0) {
        Some(try_path(map, row.len()))
    } else {
        None
    }
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
