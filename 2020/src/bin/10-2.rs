use aoc_2020::benchmarked_main;
use std::collections::{
    hash_map::Entry,
    BinaryHeap,
    HashMap,
    HashSet,
};

fn parse(input: &str) -> Vec<usize> {
    input.lines().map(|line| line.parse().unwrap()).collect::<Vec<_>>()
}

fn solve_already_parsed<T: AsRef<[usize]>>(input: &T) -> Option<usize> {
    let input =
        input.as_ref().iter().copied().chain(Some(0)).collect::<HashSet<_>>();
    let max = *input.iter().max()?;
    let mut paths_heap = BinaryHeap::new();
    let mut paths_count = HashMap::new();
    paths_count.insert(max, 1);
    paths_heap.push(max);
    while let Some(path) = paths_heap.pop() {
        let count = paths_count.remove(&path)?;
        if path == 0 {
            return Some(count);
        }
        for step in 1..=3 {
            if path < step {
                continue;
            }
            let next = path - step;
            if !input.contains(&next) {
                continue;
            }
            match paths_count.entry(next) {
                Entry::Occupied(mut entry) => {
                    *entry.get_mut() += count;
                },
                Entry::Vacant(entry) => {
                    entry.insert(count);
                    paths_heap.push(next);
                },
            }
        }
    }
    None
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
