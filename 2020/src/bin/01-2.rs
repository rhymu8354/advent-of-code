use std::{
    collections::HashSet,
    io::{
        stdin,
        Read,
    },
    time::{
        Duration,
        Instant,
    },
};

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

fn solve(input: &str) -> Option<u32> {
    solve_already_parsed(&parse(input))
}

const ITERATIONS: usize = 100000;

fn main() {
    let mut input = String::new();
    stdin().read_to_string(&mut input).unwrap();
    let solution = solve(&input).unwrap();
    println!("Solution: {}", solution);
    let mut overhead = Duration::default();
    for _ in 0..ITERATIONS {
        let start = Instant::now();
        let elapsed = start.elapsed();
        overhead += elapsed;
    }
    let mut total = Duration::default();
    for _ in 0..ITERATIONS {
        let start = Instant::now();
        let solution_repeat = solve(&input).unwrap();
        assert_eq!(solution_repeat, solution);
        let elapsed = start.elapsed();
        total += elapsed;
    }
    let map = parse(&input);
    let mut total_without_parsing = Duration::default();
    for _ in 0..ITERATIONS {
        let start = Instant::now();
        let solution_repeat = solve_already_parsed(&map).unwrap();
        assert_eq!(solution_repeat, solution);
        let elapsed = start.elapsed();
        total_without_parsing += elapsed;
    }
    println!("Time (with parsing): {:?}", total / ITERATIONS as u32);
    println!(
        "Time (without parsing): {:?}",
        total_without_parsing / ITERATIONS as u32
    );
    println!("Overhead: {:?}", overhead / ITERATIONS as u32);
    println!(
        "Time (with parsing) - Overhead: {:?}",
        (total - overhead) / ITERATIONS as u32
    );
    println!(
        "Time (without parsing) - Overhead: {:?}",
        (total_without_parsing - overhead) / ITERATIONS as u32
    );
}
