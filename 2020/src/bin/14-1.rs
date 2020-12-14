use std::collections::HashMap;

use aoc_2020::benchmarked_main;

#[derive(Clone, Copy, Debug, Default)]
struct Masks {
    on: u64,
    off: u64,
}

#[derive(Debug)]
struct MemSetArgs {
    address: u64,
    value: u64,
}

#[derive(Debug)]
enum Instruction {
    SetMasks(Masks),
    MemSet(MemSetArgs),
}

fn parse(input: &str) -> Vec<Instruction> {
    input
        .lines()
        .map(|line| match &line[0..3] {
            "mas" => Instruction::SetMasks(line[7..].chars().fold(
                Masks::default(),
                |masks, ch| Masks {
                    on: (masks.on << 1) + (ch == '1') as u64,
                    off: (masks.off << 1) + (ch != '0') as u64,
                },
            )),
            "mem" => {
                let delimiter = line.find(']').unwrap();
                let address = line[4..delimiter].parse().unwrap();
                let line = &line[delimiter..];
                let delimiter = line.find(|ch: char| ch.is_digit(10)).unwrap();
                let value = line[delimiter..].parse().unwrap();
                Instruction::MemSet(MemSetArgs {
                    address,
                    value,
                })
            },
            _ => panic!("Your input is bad"),
        })
        .collect::<Vec<_>>()
}

fn solve_already_parsed<T: AsRef<[Instruction]>>(input: &T) -> Option<u64> {
    let mut memory = HashMap::new();
    let mut masks = Masks::default();
    for instruction in input.as_ref() {
        match instruction {
            Instruction::SetMasks(new_masks) => {
                masks = *new_masks;
            },
            Instruction::MemSet(args) => {
                let value = (args.value | masks.on) & masks.off;
                memory.insert(&args.address, value);
            },
        }
    }
    Some(memory.values().sum())
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
