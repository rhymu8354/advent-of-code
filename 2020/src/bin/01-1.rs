use std::collections::HashSet;

use aoc_2020::benchmarked_main;

fn parse(input: &str) -> HashSet<u32> {
    input
        .lines()
        .map(|line| line.trim().parse().unwrap())
        .collect::<HashSet<u32>>()
}

fn solve_already_parsed(nums: &HashSet<u32>) -> Option<u32> {
    for num in nums {
        let complement = 2020 - num;
        if nums.contains(&complement) {
            return Some(num * complement);
        }
    }
    None
}

const ITERATIONS: usize = 100000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
