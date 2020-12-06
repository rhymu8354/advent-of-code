use std::collections::HashSet;

use aoc_2020::benchmarked_main;

fn parse(input: &str) -> Vec<Vec<HashSet<char>>> {
    let mut groups = Vec::new();
    let mut group = Vec::new();
    for line in input.lines().map(|line| line.trim()) {
        if line.is_empty() {
            let mut new_group = Vec::new();
            std::mem::swap(&mut group, &mut new_group);
            groups.push(new_group);
        } else {
            group.push(line.chars().collect::<HashSet<char>>());
        }
    }
    groups.push(group);
    groups
}

fn solve_already_parsed<T: AsRef<[Vec<HashSet<char>>]>>(
    input: &T
) -> Option<usize> {
    let input = input.as_ref();
    Some(input.iter().fold(0, |acc, group| {
        let mut group = group.iter();
        let first = group.next().unwrap();
        acc + group
            .fold(first.clone(), |acc, person| {
                acc.intersection(person).copied().collect::<HashSet<char>>()
            })
            .len()
    }))
}

const ITERATIONS: usize = 1000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
