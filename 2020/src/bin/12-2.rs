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
    let mut wdx = 10;
    let mut wdy = -1;
    for instruction in input.as_ref() {
        match &instruction.action {
            Action::Forward => {
                x += wdx * instruction.value;
                y += wdy * instruction.value;
            },
            Action::North => wdy -= instruction.value,
            Action::South => wdy += instruction.value,
            Action::West => wdx -= instruction.value,
            Action::East => wdx += instruction.value,
            Action::Left => {
                let (new_wdx, new_wdy) = match instruction.value {
                    90 => (wdy, -wdx),
                    180 => (-wdx, -wdy),
                    270 => (-wdy, wdx),
                    _ => panic!("invalid angle"),
                };
                wdx = new_wdx;
                wdy = new_wdy;
            },
            Action::Right => {
                let (new_wdx, new_wdy) = match instruction.value {
                    90 => (-wdy, wdx),
                    180 => (-wdx, -wdy),
                    270 => (wdy, -wdx),
                    _ => panic!("invalid angle"),
                };
                wdx = new_wdx;
                wdy = new_wdy;
            },
        }
    }
    Some((x.abs() + y.abs()) as usize)
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
