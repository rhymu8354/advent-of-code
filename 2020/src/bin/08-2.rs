use aoc_2020::benchmarked_main;
use std::collections::HashSet;

#[derive(Clone, Copy, Debug)]
enum Opcode {
    Accumulate,
    Jump,
    NoOp,
}

struct Instruction {
    opcode: Opcode,
    arg: isize,
}

#[derive(Debug, PartialEq)]
enum Outcome {
    InfiniteLoop(isize),
    Terminate(isize),
}

fn parse(input: &str) -> Vec<Instruction> {
    input
        .lines()
        .map(|line| Instruction {
            opcode: match &line[..3] {
                "acc" => Opcode::Accumulate,
                "jmp" => Opcode::Jump,
                "nop" => Opcode::NoOp,
                _ => panic!("Oh my goodness your input is broken!"),
            },
            arg: line[4..].parse().unwrap(),
        })
        .collect::<Vec<_>>()
}

fn execute<T: AsRef<[Instruction]>>(
    input: &T,
    hack: Option<isize>,
) -> Outcome {
    let input = input.as_ref();
    let mut accumulator = 0;
    let mut ptr = 0;
    let mut instructions_hit = HashSet::new();
    loop {
        if instructions_hit.insert(ptr) {
            let instruction = &input[ptr as usize];
            let opcode = if let Some(hack) = hack {
                if ptr == hack {
                    match instruction.opcode {
                        Opcode::Accumulate => Opcode::Accumulate,
                        Opcode::Jump => Opcode::NoOp,
                        Opcode::NoOp => Opcode::Jump,
                    }
                } else {
                    instruction.opcode
                }
            } else {
                instruction.opcode
            };
            match opcode {
                Opcode::Accumulate => {
                    accumulator += instruction.arg;
                    ptr += 1;
                },
                Opcode::Jump => {
                    ptr += instruction.arg;
                },
                Opcode::NoOp => {
                    ptr += 1;
                },
            }
            if ptr as usize == input.len() {
                break Outcome::Terminate(accumulator);
            }
        } else {
            break Outcome::InfiniteLoop(accumulator);
        }
    }
}

fn solve_already_parsed<T: AsRef<[Instruction]>>(input: &T) -> Option<isize> {
    let input = input.as_ref();
    for ptr in 0..input.len() {
        let ptr = ptr as isize;
        if let Opcode::Accumulate = input[ptr as usize].opcode {
            continue;
        }
        let outcome = execute(&input, Some(ptr));
        if let Outcome::Terminate(accumulator) = outcome {
            return Some(accumulator);
        }
    }
    None
}

const ITERATIONS: usize = 1000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
