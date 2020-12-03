use std::{
    io::{
        stdin,
        Read,
    },
    time::{
        Duration,
        Instant,
    },
};

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

fn solve_already_parsed(map: &[Vec<char>]) -> Option<usize> {
    if let Some(row) = map.get(0) {
        Some(try_path(map, row.len()))
    } else {
        None
    }
}

fn solve(input: &str) -> Option<usize> {
    solve_already_parsed(&parse(input))
}

const ITERATIONS: usize = 10000;

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
