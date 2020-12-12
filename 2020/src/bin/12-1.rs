use aoc_2020::benchmarked_main;

enum Action {
    Forward,
    North,
    South,
    West,
    East,
    Left,
    Right,
}

struct Instruction {
    action: Action,
    value: isize,
}

fn parse(input: &str) -> Vec<Instruction> {
    input
        .lines()
        .map(|line| line.trim())
        .map(|line| Instruction {
            action: match line.chars().next().unwrap() {
                'F' => Action::Forward,
                'N' => Action::North,
                'S' => Action::South,
                'W' => Action::West,
                'E' => Action::East,
                'L' => Action::Left,
                'R' => Action::Right,
                _ => panic!("invalid action"),
            },
            value: line[1..].parse().unwrap(),
        })
        .collect::<Vec<_>>()
}

fn solve_already_parsed<T: AsRef<[Instruction]>>(input: &T) -> Option<usize> {
    let mut x = 0;
    let mut y = 0;
    let mut angle = 0;
    for instruction in input.as_ref() {
        match &instruction.action {
            Action::Forward => match angle {
                0 => x += instruction.value,
                90 => y += instruction.value,
                180 => x -= instruction.value,
                270 => y -= instruction.value,
                _ => panic!("invalid angle"),
            },
            Action::North => y -= instruction.value,
            Action::South => y += instruction.value,
            Action::West => x -= instruction.value,
            Action::East => x += instruction.value,
            Action::Left => angle = (angle + 360 - instruction.value) % 360,
            Action::Right => angle = (angle + instruction.value) % 360,
        }
    }
    Some((x.abs() + y.abs()) as usize)
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
