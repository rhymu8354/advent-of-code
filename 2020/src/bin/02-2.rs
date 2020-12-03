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

struct PasswordWithPolicy<'a> {
    first: usize,
    second: usize,
    ch: char,
    password: &'a str,
}

fn chomp(
    line: &str,
    delimiter: char,
) -> (&str, &str) {
    let mut parts = line.splitn(2, delimiter);
    let left = parts.next().unwrap();
    let right = parts.next().unwrap();
    (left, right)
}

fn chomp_number(
    line: &str,
    delimiter: char,
) -> (usize, &str) {
    let (left, right) = chomp(line, delimiter);
    (left.parse().unwrap(), right)
}

// Each line must adhere to the following regular expression:
// (\d+)-(\d+) (.): (.*)
//   \        \   \     \
//    \        \   -- ch -- password
//     -- first -- second
fn scan(line: &str) -> PasswordWithPolicy {
    let (first, line) = chomp_number(line, '-');
    let (second, line) = chomp_number(line, ' ');
    let ch = line.chars().next().unwrap();
    let password_start = ch.len_utf8() + ": ".len();
    let password = &line[password_start..];
    PasswordWithPolicy {
        first,
        second,
        ch,
        password,
    }
}

fn is_valid_password(
    PasswordWithPolicy {
        first,
        second,
        ch,
        password,
    }: &PasswordWithPolicy
) -> bool {
    [*first, *second]
        .iter()
        .filter(|&&index| password.chars().nth(index - 1).unwrap() == *ch)
        .count()
        == 1
}

fn parse(input: &str) -> Vec<PasswordWithPolicy> {
    input.lines().map(|line| scan(line.trim())).collect()
}

fn solve_already_parsed(input: &[PasswordWithPolicy]) -> usize {
    input
        .iter()
        .filter(|password_with_policy| is_valid_password(password_with_policy))
        .count()
}

fn solve(input: &str) -> usize {
    solve_already_parsed(&parse(input))
}

const ITERATIONS: usize = 10000;

fn main() {
    let mut input = String::new();
    stdin().read_to_string(&mut input).unwrap();
    let solution = solve(&input);
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
        let solution_repeat = solve(&input);
        assert_eq!(solution_repeat, solution);
        let elapsed = start.elapsed();
        total += elapsed;
    }
    let map = parse(&input);
    let mut total_without_parsing = Duration::default();
    for _ in 0..ITERATIONS {
        let start = Instant::now();
        let solution_repeat = solve_already_parsed(&map);
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
