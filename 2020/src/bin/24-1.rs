use aoc_2020::benchmarked_main;
use std::{
    collections::HashSet,
    fmt::Debug,
};

#[derive(Clone, Copy, Eq, PartialEq)]
enum Direction {
    East,
    SouthEast,
    SouthWest,
    West,
    NorthWest,
    NorthEast,
}

impl Debug for Direction {
    fn fmt(
        &self,
        f: &mut std::fmt::Formatter<'_>,
    ) -> std::fmt::Result {
        write!(f, "{}", match self {
            Direction::East => "E",
            Direction::SouthEast => "SE",
            Direction::SouthWest => "SW",
            Direction::West => "W",
            Direction::NorthWest => "NW",
            Direction::NorthEast => "NE",
        })
    }
}

enum ParsePhase {
    Initial,
    North,
    South,
}

#[derive(Clone, Copy, Debug, Default, Eq, Hash, PartialEq)]
struct Coord {
    x: isize,
    y: isize,
}

impl Coord {
    fn jump(
        &self,
        dir: Direction,
    ) -> Self {
        match dir {
            Direction::East => Self {
                x: self.x + 1,
                y: self.y - 1,
            },
            Direction::SouthEast => Self {
                x: self.x,
                y: self.y - 1,
            },
            Direction::SouthWest => Self {
                x: self.x - 1,
                y: self.y,
            },
            Direction::West => Self {
                x: self.x - 1,
                y: self.y + 1,
            },
            Direction::NorthWest => Self {
                x: self.x,
                y: self.y + 1,
            },
            Direction::NorthEast => Self {
                x: self.x + 1,
                y: self.y,
            },
        }
    }
}

fn parse(input: &str) -> Vec<Vec<Direction>> {
    input
        .lines()
        .map(|line| {
            let mut parse_phase = ParsePhase::Initial;
            line.trim()
                .chars()
                .filter_map(|ch| match ch {
                    'n' => {
                        parse_phase = ParsePhase::North;
                        None
                    },
                    's' => {
                        parse_phase = ParsePhase::South;
                        None
                    },
                    ch => {
                        let new_dir = match (ch, &parse_phase) {
                            ('w', ParsePhase::North) => Direction::NorthWest,
                            ('w', ParsePhase::South) => Direction::SouthWest,
                            ('w', ParsePhase::Initial) => Direction::West,
                            ('e', ParsePhase::North) => Direction::NorthEast,
                            ('e', ParsePhase::South) => Direction::SouthEast,
                            ('e', ParsePhase::Initial) => Direction::East,
                            (_, _) => panic!("bad input"),
                        };
                        parse_phase = ParsePhase::Initial;
                        Some(new_dir)
                    },
                })
                .collect()
        })
        .collect()
}

fn solve_already_parsed<T: AsRef<[Vec<Direction>]>>(
    input: &T
) -> Option<usize> {
    let mut black_tiles = HashSet::new();
    for path in input.as_ref() {
        let pos = path.iter().fold(Coord::default(), |pos, dir| pos.jump(*dir));
        if !black_tiles.remove(&pos) {
            black_tiles.insert(pos);
        }
    }
    Some(black_tiles.len())
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
