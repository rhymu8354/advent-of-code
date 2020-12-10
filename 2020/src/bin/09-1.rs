use aoc_2020::benchmarked_main;
use std::collections::HashSet;

const WINDOW_LENGTH: usize = 25;

fn parse(input: &str) -> Vec<usize> {
    input.lines().map(|line| line.parse().unwrap()).collect::<Vec<_>>()
}

fn is_valid<T: AsRef<[usize]>>(
    input: T,
    window_contents: &HashSet<usize>,
    total: usize,
) -> bool {
    input.as_ref().iter().any(|&lhs| {
        let rhs = total - lhs;
        lhs != rhs && window_contents.contains(&rhs)
    })
}

fn solve_already_parsed<T: AsRef<[usize]>>(input: &T) -> Option<usize> {
    let input = input.as_ref();
    let mut window_contents =
        input.iter().take(WINDOW_LENGTH).copied().collect::<HashSet<_>>();
    input[WINDOW_LENGTH..].iter().enumerate().find_map(|(i, &total)| {
        if is_valid(&input[i..i + WINDOW_LENGTH], &window_contents, total) {
            window_contents.remove(&input[i]);
            window_contents.insert(total);
            None
        } else {
            Some(total)
        }
    })
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
