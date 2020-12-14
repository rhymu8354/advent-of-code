use std::collections::{
    HashMap,
    HashSet,
};

use aoc_2020::benchmarked_main;

#[derive(Clone, Copy, Debug, Default)]
struct Masks {
    on: u64,
    float: u64,
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
                    float: (masks.float << 1) + (ch == 'X') as u64,
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

fn permute(
    float_permutations: &mut HashSet<u64>,
    mut pos: u64,
    path: u64,
    float: u64,
) {
    while float & pos == 0 {
        if pos == (1 << 36) {
            float_permutations.insert(path);
            return;
        }
        pos <<= 1;
    }
    permute(float_permutations, pos << 1, path, float);
    permute(float_permutations, pos << 1, path + pos, float);
}

fn solve_already_parsed<T: AsRef<[Instruction]>>(input: &T) -> Option<u64> {
    let mut memory = HashMap::new();
    let mut masks = Masks::default();
    let mut float_permutations = HashSet::new();
    for instruction in input.as_ref() {
        match instruction {
            Instruction::SetMasks(new_masks) => {
                masks = *new_masks;
                float_permutations.clear();
                permute(&mut float_permutations, 1, 0, masks.float);
            },
            Instruction::MemSet(args) => {
                let base_address = (args.address & !masks.float) | masks.on;
                for address in float_permutations
                    .iter()
                    .map(|address| address | base_address)
                {
                    memory.insert(address, args.value);
                }
            },
        }
    }
    Some(memory.values().sum())
}

const ITERATIONS: usize = 100;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
