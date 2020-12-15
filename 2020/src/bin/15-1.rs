use aoc_2020::benchmarked_main;
use std::collections::HashMap;

fn parse(input: &str) -> Vec<usize> {
    input
        .trim()
        .split(',')
        .map(str::parse)
        .map(Result::unwrap)
        .collect::<Vec<_>>()
}

fn solve_already_parsed<T: AsRef<[usize]>>(input: &T) -> Option<usize> {
    let mut input = input.as_ref();
    let mut last_spoken = 0;
    let mut turn_each_spoken = HashMap::new();
    let mut turn_last_spoken: Option<usize> = None;
    for turn in 1..=2020 {
        let spoken = input
            .iter()
            .next()
            // Rule 1: speak each input number in order.
            .map(|&next| {
                input = &input[1..];
                next
            })
            // Rule 2: once all input spoken:
            // * If the last number spoken was spoken for the first time,
            //   speak 0.
            // * Otherwise, speak the number of turns between the two turns
            //   in which that number was last spoken.
            .unwrap_or_else(|| {
                turn_last_spoken
                    .map(|last_turn| turn - last_turn - 1)
                    .unwrap_or(0)
            });
        turn_last_spoken = turn_each_spoken.insert(spoken, turn);
        last_spoken = spoken;
    }
    Some(last_spoken)
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
