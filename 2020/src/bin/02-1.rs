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

use aoc_2020::benchmarked_main;

struct PasswordWithPolicy<'a> {
    min: usize,
    max: usize,
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
//     --- min  -- max
fn scan(line: &str) -> PasswordWithPolicy {
    let (min, line) = chomp_number(line, '-');
    let (max, line) = chomp_number(line, ' ');
    let ch = line.chars().next().unwrap();
    let password_start = ch.len_utf8() + ": ".len();
    let password = &line[password_start..];
    PasswordWithPolicy {
        min,
        max,
        ch,
        password,
    }
}

fn is_valid_password(
    PasswordWithPolicy {
        min,
        max,
        ch,
        password,
    }: &PasswordWithPolicy
) -> bool {
    let count =
        password.chars().filter(|&password_ch| *ch == password_ch).count();
    (*min..=*max).contains(&count)
}

fn parse<'a, 'b>(input: &'a str) -> Vec<PasswordWithPolicy<'b>>
where
    'a: 'b,
{
    input.lines().map(|line| scan(line.trim())).collect()
}

fn solve_already_parsed<'a, 'b, T: AsRef<[PasswordWithPolicy<'a>]>>(
    input: &'b T
) -> Option<usize> {
    let input = input.as_ref();
    Some(
        input
            .iter()
            .filter(|password_with_policy| {
                is_valid_password(password_with_policy)
            })
            .count(),
    )
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
