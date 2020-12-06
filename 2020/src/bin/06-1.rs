use std::collections::HashSet;

use aoc_2020::benchmarked_main;

fn parse(input: &str) -> Vec<HashSet<char>> {
    let mut groups = Vec::new();
    let mut group = HashSet::new();
    for line in input.lines().map(|line| line.trim()) {
        if line.is_empty() {
            let mut new_group = HashSet::new();
            std::mem::swap(&mut group, &mut new_group);
            groups.push(new_group);
        } else {
            group.extend(line.chars());
        }
    }
    groups.push(group);
    groups
}

fn solve_already_parsed<T: AsRef<[HashSet<char>]>>(input: &T) -> Option<usize> {
    let input = input.as_ref();
    Some(input.iter().fold(0, |acc, group| acc + group.len()))
}

const ITERATIONS: usize = 1000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
