use std::collections::HashSet;

use aoc_2020::benchmarked_main;

fn parse(input: &str) -> HashSet<u32> {
    input
        .lines()
        .map(|line| line.trim().parse().unwrap())
        .collect::<HashSet<u32>>()
}

fn solve_already_parsed(nums: &HashSet<u32>) -> Option<u32> {
    for &num1 in nums {
        let remainder = 2020 - num1;
        for &num2 in nums {
            if num2 == num1 || num2 > remainder {
                continue;
            }
            let num3 = 2020 - num1 - num2;
            if nums.contains(&num3) {
                return Some(num1 * num2 * num3);
            }
        }
    }
    None
}

const ITERATIONS: usize = 100000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
