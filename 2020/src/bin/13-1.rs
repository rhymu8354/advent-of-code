use aoc_2020::benchmarked_main;
use std::collections::HashSet;

struct Input {
    earliest: usize,
    ids: HashSet<usize>,
}

struct Bus {
    id: usize,
    wait_time: usize,
}

fn parse(input: &str) -> Input {
    let mut input = input.lines();
    let line1 = input.next().unwrap().trim();
    let line2 = input.next().unwrap().trim();
    Input {
        earliest: line1.parse().unwrap(),
        ids: line2
            .split(',')
            .filter_map(|id| id.parse().ok())
            .collect::<HashSet<_>>(),
    }
}

fn solve_already_parsed(input: &Input) -> Option<usize> {
    input
        .ids
        .iter()
        .map(|id| {
            let offset = input.earliest % id;
            Bus {
                id: *id,
                wait_time: if offset == 0 {
                    0
                } else {
                    id - offset
                },
            }
        })
        .fold(None::<Bus>, |acc, bus| {
            if let Some(acc) = acc {
                if bus.wait_time < acc.wait_time {
                    Some(bus)
                } else {
                    Some(acc)
                }
            } else {
                Some(bus)
            }
        })
        .map(|bus| bus.id * bus.wait_time)
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
