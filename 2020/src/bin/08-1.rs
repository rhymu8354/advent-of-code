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

fn solve_already_parsed<T: AsRef<[Instruction]>>(input: &T) -> Option<isize> {
    let input = input.as_ref();
    let mut accumulator = 0;
    let mut ptr = 0;
    let mut instructions_hit = HashSet::new();
    Some(loop {
        if instructions_hit.insert(ptr) {
            let instruction = &input[ptr as usize];
            match instruction.opcode {
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
        } else {
            break accumulator;
        }
    })
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
