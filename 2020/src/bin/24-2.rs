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

struct NeighborIterator {
    center: Coord,
    dir: Direction,
    neighbors_left: usize,
}

impl Iterator for NeighborIterator {
    type Item = Coord;

    fn next(&mut self) -> Option<Self::Item> {
        self.neighbors_left.checked_sub(1).map(|neighbors_left| {
            self.neighbors_left = neighbors_left;
            match self.dir {
                Direction::East => {
                    self.dir = Direction::SouthEast;
                    self.center.jump(Direction::East)
                },
                Direction::SouthEast => {
                    self.dir = Direction::SouthWest;
                    self.center.jump(Direction::SouthEast)
                },
                Direction::SouthWest => {
                    self.dir = Direction::West;
                    self.center.jump(Direction::SouthWest)
                },
                Direction::West => {
                    self.dir = Direction::NorthWest;
                    self.center.jump(Direction::West)
                },
                Direction::NorthWest => {
                    self.dir = Direction::NorthEast;
                    self.center.jump(Direction::NorthWest)
                },
                Direction::NorthEast => {
                    self.dir = Direction::East;
                    self.center.jump(Direction::NorthEast)
                },
            }
        })
    }
}

fn coord_neighbors(center: Coord) -> NeighborIterator {
    NeighborIterator {
        center,
        dir: Direction::East,
        neighbors_left: 6,
    }
}

fn investigate_black_tiles(
    black_tiles: &HashSet<Coord>
) -> (HashSet<Coord>, HashSet<Coord>) {
    let mut white_neighbors = HashSet::new();
    let mut flip_to_white = HashSet::new();
    for coord in black_tiles.iter() {
        let mut num_black_neighbors = 0;
        for neighbor in coord_neighbors(*coord) {
            if black_tiles.contains(&neighbor) {
                num_black_neighbors += 1;
            } else {
                white_neighbors.insert(neighbor);
            }
        }
        if !(1..=2).contains(&num_black_neighbors) {
            flip_to_white.insert(*coord);
        }
    }
    (white_neighbors, flip_to_white)
}

fn investigate_white_tiles<'a, 'b, T>(
    black_tiles: &'a HashSet<Coord>,
    white_neighbors: T,
) -> impl Iterator<Item = Coord> + 'a
where
    T: IntoIterator<Item = Coord> + 'b,
    'b: 'a,
{
    white_neighbors.into_iter().filter(move |white_tile| {
        coord_neighbors(*white_tile)
            .filter(|white_tile_neighbor| {
                black_tiles.contains(white_tile_neighbor)
            })
            .count()
            == 2
    })
}

fn flip(black_tiles: &mut HashSet<Coord>) {
    let (white_neighbors, flip_to_white) = investigate_black_tiles(black_tiles);
    let flip_to_black = investigate_white_tiles(black_tiles, white_neighbors)
        .collect::<HashSet<_>>();
    for coord in flip_to_white.into_iter() {
        black_tiles.remove(&coord);
    }
    for coord in flip_to_black.into_iter() {
        black_tiles.insert(coord);
    }
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
    for _ in 0..100 {
        flip(&mut black_tiles);
    }
    Some(black_tiles.len())
}

const ITERATIONS: usize = 10;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
