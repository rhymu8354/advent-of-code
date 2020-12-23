use aoc_2020::benchmarked_main;

fn parse(input: &str) -> Vec<usize> {
    input
        .trim()
        .chars()
        .map(|ch| ch.to_digit(10).unwrap())
        .map(|digit| digit as usize)
        .collect()
}

fn play_game(
    cups: &mut [usize],
    first_cup: usize,
) {
    let num_cups = cups.len();
    let mut cup = first_cup;
    for _ in 0..10_000_000 {
        let mut pick = [0; 3];
        pick[0] = cups[cup - 1];
        pick[1] = cups[pick[0] - 1];
        pick[2] = cups[pick[1] - 1];
        let next = cups[pick[2] - 1];
        cups[cup - 1] = next;
        let mut dest_cup = cup;
        loop {
            dest_cup -= 1;
            if dest_cup == 0 {
                dest_cup = num_cups;
            }
            if !pick.contains(&dest_cup) {
                break;
            }
        }
        cups[pick[2] - 1] = cups[dest_cup - 1];
        cups[dest_cup - 1] = pick[0];
        cup = next;
    }
}

fn solve_already_parsed<T: AsRef<[usize]>>(input: &T) -> Option<usize> {
    let mut cups = vec![0; 1_000_000];
    let num_cups = cups.len();
    let mut input = input.as_ref().iter();
    let first_cup = input.next().copied().unwrap();
    let mut cup = first_cup;
    for next_cup in input {
        cups[cup - 1] = *next_cup;
        cup = *next_cup;
    }
    cups[cup - 1] = 10;
    for cup in 10..=num_cups - 1 {
        cups[cup - 1] = cup + 1;
    }
    cups[num_cups - 1] = first_cup;
    play_game(&mut cups, first_cup);
    let cup1 = cups[0];
    let cup2 = cups[cup1 - 1];
    Some(cup1 * cup2)
}

const ITERATIONS: usize = 10;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
