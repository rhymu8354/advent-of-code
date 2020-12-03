use aoc_2020::benchmarked_main;

fn try_path(
    map: &[Vec<char>],
    cols: usize,
    dx: usize,
    dy: usize,
) -> usize {
    map.iter()
        .step_by(dy)
        .enumerate()
        .filter(|(i, row)| row[(i * dx) % cols] == '#')
        .count()
}

fn parse(input: &str) -> Vec<Vec<char>> {
    input
        .lines()
        .map(|line| line.trim().chars().collect::<Vec<_>>())
        .collect::<Vec<_>>()
}

fn solve_already_parsed<T: AsRef<[Vec<char>]>>(map: &T) -> Option<usize> {
    let map = map.as_ref();
    if let Some(row) = map.get(0) {
        let cols = row.len();
        let product = [(1, 1), (3, 1), (5, 1), (7, 1), (1, 2)]
            .iter()
            .fold(1, |product, (dx, dy)| {
                product * try_path(&map, cols, *dx, *dy)
            });
        Some(product)
    } else {
        None
    }
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
