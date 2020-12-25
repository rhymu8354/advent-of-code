use aoc_2020::benchmarked_main;

fn parse(input: &str) -> Vec<usize> {
    input.lines().map(|line| line.trim().parse().unwrap()).collect()
}

fn transform(
    mut value: usize,
    loop_size: usize,
    subject_number: usize,
) -> usize {
    for _ in 0..loop_size {
        value = (value * subject_number) % 20201227;
    }
    value
}

fn guess_loop_count(public_key: usize) -> usize {
    let mut value = 1;
    for loop_count in 0.. {
        if value == public_key {
            return loop_count;
        }
        value = transform(value, 1, 7);
    }
    unreachable!()
}

fn solve_already_parsed<T: AsRef<[usize]>>(input: &T) -> Option<usize> {
    let input = input.as_ref();
    let secret = guess_loop_count(input[1]);
    Some(transform(1, secret, input[0]))
}

const ITERATIONS: usize = 100;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
