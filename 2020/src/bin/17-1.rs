use aoc_2020::benchmarked_main;
use std::{
    collections::HashSet,
    ops::RangeInclusive,
};

type Coord = (isize, isize, isize);

fn parse(input: &str) -> HashSet<Coord> {
    input
        .lines()
        .map(|line| line.trim().chars())
        .enumerate()
        .map(|(y, row)| {
            row.enumerate()
                .filter(|(_, cell)| *cell == '#')
                .map(move |(x, _)| (x as isize, y as isize, 0_isize))
        })
        .flatten()
        .collect::<HashSet<_>>()
}

fn neighbors(coord: &Coord) -> impl Iterator<Item = Coord> + '_ {
    (-1..=1)
        .map(move |z| {
            (-1..=1)
                .map(move |y| {
                    (-1..=1).map(move |x| {
                        ((*coord).0 + x, (*coord).1 + y, (*coord).2 + z)
                    })
                })
                .flatten()
        })
        .flatten()
        .filter(move |neighbor| neighbor != coord)
}

fn active_neighbors<'a, 'b: 'a>(
    space: &'a HashSet<Coord>,
    coord: &'b Coord,
) -> impl Iterator<Item = Coord> + 'a {
    neighbors(coord).filter(move |coord| space.contains(coord))
}

// Here be the rules!
const NEIGHBORS_REQUIRED_TO_KEEP_ALIVE: RangeInclusive<usize> = 2..=3;
const NEIGHBORS_REQUIRED_TO_ACTIVATE: usize = 3;

fn simulate(input: &HashSet<Coord>) -> HashSet<Coord> {
    let keep_alive = input
        .iter()
        .filter(|coord| {
            NEIGHBORS_REQUIRED_TO_KEEP_ALIVE
                .contains(&active_neighbors(input, coord).count())
        })
        .copied();
    // NOTE: One might be tempted to *not* collect this `HashSet`,
    // but instead filter it immediately to form `activate`.  However,
    // this would make it run *slower*, since `neighbors_of_active`
    // collapses together many duplicate neighbors, allowing us to avoid
    // a *huge* number of redundant tests for the cells to activate.
    let neighbors_of_active = input
        .iter()
        .map(|coord| neighbors(coord))
        .flatten()
        .collect::<HashSet<_>>();
    let activate = neighbors_of_active
        .iter()
        .filter(|coord| {
            !input.contains(coord)
                && active_neighbors(input, coord).count()
                    == NEIGHBORS_REQUIRED_TO_ACTIVATE
        })
        .copied();
    keep_alive.chain(activate).collect::<HashSet<_>>()
}

#[cfg(debug)]
fn print_space(input: &HashSet<Coord>) {
    let (min_x, min_y, min_z, max_x, max_y, max_z) = input.iter().fold(
        (
            isize::MAX,
            isize::MAX,
            isize::MAX,
            isize::MIN,
            isize::MIN,
            isize::MIN,
        ),
        |acc, coord| {
            (
                acc.0.min(coord.0),
                acc.1.min(coord.1),
                acc.2.min(coord.2),
                acc.3.max(coord.0),
                acc.4.max(coord.1),
                acc.5.max(coord.2),
            )
        },
    );
    for z in min_z..=max_z {
        println!("z={}", z);
        for y in min_y..=max_y {
            for x in min_x..=max_x {
                if input.contains(&(x, y, z)) {
                    print!("#")
                } else {
                    print!(".")
                }
            }
            println!()
        }
        println!();
    }
}

fn solve_already_parsed(input: &HashSet<Coord>) -> Option<usize> {
    let mut space = input.clone();
    for _ in 0..6 {
        space = simulate(&space);
    }
    #[cfg(debug)]
    print_space(&space);
    Some(space.len())
}

const ITERATIONS: usize = 100;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
