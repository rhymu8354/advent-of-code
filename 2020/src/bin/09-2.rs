use aoc_2020::benchmarked_main;
use std::{
    cmp::Ordering,
    collections::HashSet,
};

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

fn find_invalid_number<T: AsRef<[usize]>>(input: &T) -> Option<usize> {
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

fn solve_already_parsed<T: AsRef<[usize]>>(input: &T) -> Option<usize> {
    let input = input.as_ref();
    let invalid_number = find_invalid_number(&input).unwrap();
    let mut first = 0;
    let mut last = 0;
    let mut sum = 0;
    while last < input.len() {
        match sum.cmp(&invalid_number) {
            Ordering::Less => {
                sum += input[last];
                last += 1;
            },
            Ordering::Equal => {
                let mut window =
                    input.iter().skip(first).take(last - first).copied();
                let first = window.next()?;
                let (min, max) = window.fold((first, first), |acc, next| {
                    (acc.0.min(next), acc.1.max(next))
                });
                return Some(min + max);
            },
            Ordering::Greater => {
                sum -= input[first];
                first += 1;
            },
        }
    }
    None
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
