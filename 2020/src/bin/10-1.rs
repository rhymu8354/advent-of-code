use aoc_2020::benchmarked_main;

fn parse(input: &str) -> Vec<usize> {
    input.lines().map(|line| line.parse().unwrap()).collect::<Vec<_>>()
}

fn solve_already_parsed<T: AsRef<[usize]>>(input: &T) -> Option<usize> {
    let mut input = input.as_ref().to_vec();
    input.push(0);
    input.sort_unstable();
    let mut jumps = [0; 3];
    input.windows(2).for_each(|adapters| {
        jumps[adapters[1] - adapters[0] - 1] += 1;
    });
    Some(jumps[0] * (jumps[2] + 1))
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
