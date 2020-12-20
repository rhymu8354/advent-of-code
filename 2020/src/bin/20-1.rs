use aoc_2020::benchmarked_main;
use std::{
    collections::{
        HashMap,
        HashSet,
    },
    panic,
};

enum ParsePhase {
    Header,
    Body(usize),
    Separator,
}

fn parse(input: &str) -> HashMap<usize, Vec<Vec<char>>> {
    let mut data = HashMap::new();
    let mut parse_phase = ParsePhase::Header;
    let mut id = 0;
    let mut body = Vec::new();
    for line in input.lines().map(|line| line.trim()) {
        match parse_phase {
            ParsePhase::Header => {
                id = line[5..line.len() - 1].parse().unwrap();
                parse_phase = ParsePhase::Body(0);
            },
            ParsePhase::Body(row_num) => {
                body.push(line.chars().collect::<Vec<_>>());
                parse_phase = if row_num == 9 {
                    data.insert(id, body);
                    body = Vec::new();
                    ParsePhase::Separator
                } else {
                    ParsePhase::Body(row_num + 1)
                };
            },
            ParsePhase::Separator => {
                assert_eq!(line, "");
                parse_phase = ParsePhase::Header;
            },
        }
    }
    data
}

#[derive(Clone)]
struct Tile {
    id: usize,
    orientation: usize,
}

const COORDS: [[(usize, usize); 10]; 8] = [
    [
        // 0
        (9, 0),
        (9, 1),
        (9, 2),
        (9, 3),
        (9, 4),
        (9, 5),
        (9, 6),
        (9, 7),
        (9, 8),
        (9, 9),
    ],
    [
        // 1
        (9, 9),
        (8, 9),
        (7, 9),
        (6, 9),
        (5, 9),
        (4, 9),
        (3, 9),
        (2, 9),
        (1, 9),
        (0, 9),
    ],
    [
        // 2
        (0, 9),
        (0, 8),
        (0, 7),
        (0, 6),
        (0, 5),
        (0, 4),
        (0, 3),
        (0, 2),
        (0, 1),
        (0, 0),
    ],
    [
        // 3
        (0, 0),
        (1, 0),
        (2, 0),
        (3, 0),
        (4, 0),
        (5, 0),
        (6, 0),
        (7, 0),
        (8, 0),
        (9, 0),
    ],
    [
        // 4
        (0, 0),
        (0, 1),
        (0, 2),
        (0, 3),
        (0, 4),
        (0, 5),
        (0, 6),
        (0, 7),
        (0, 8),
        (0, 9),
    ],
    [
        // 5
        (0, 9),
        (1, 9),
        (2, 9),
        (3, 9),
        (4, 9),
        (5, 9),
        (6, 9),
        (7, 9),
        (8, 9),
        (9, 9),
    ],
    [
        // 6
        (9, 9),
        (9, 8),
        (9, 7),
        (9, 6),
        (9, 5),
        (9, 4),
        (9, 3),
        (9, 2),
        (9, 1),
        (9, 0),
    ],
    [
        // 7
        (9, 0),
        (8, 0),
        (7, 0),
        (6, 0),
        (5, 0),
        (4, 0),
        (3, 0),
        (2, 0),
        (1, 0),
        (0, 0),
    ],
];

const L2R: [usize; 8] = [4, 7, 6, 5, 0, 3, 2, 1];
const L2T: [usize; 8] = [5, 4, 7, 6, 1, 0, 3, 2];
const L2B: [usize; 8] = [3, 0, 1, 2, 7, 4, 5, 6];

fn left_right_match(
    left: &[Vec<char>],
    left_orientation: usize,
    right: &[Vec<char>],
    right_orientation: usize,
) -> bool {
    for i in 0..10 {
        let (left_x, left_y) = COORDS[left_orientation][i];
        let (right_x, right_y) = COORDS[L2R[right_orientation]][i];
        if left[left_y][left_x] != right[right_y][right_x] {
            return false;
        }
    }
    true
}

fn top_bottom_match(
    top: &[Vec<char>],
    top_orientation: usize,
    bottom: &[Vec<char>],
    bottom_orientation: usize,
) -> bool {
    for i in 0..10 {
        let (top_x, top_y) = COORDS[L2T[top_orientation]][i];
        let (bottom_x, bottom_y) = COORDS[L2B[bottom_orientation]][i];
        if top[top_y][top_x] != bottom[bottom_y][bottom_x] {
            return false;
        }
    }
    true
}

fn fits(
    input: &HashMap<usize, Vec<Vec<char>>>,
    tiles: &mut Vec<Vec<Option<Tile>>>,
    candidate_id: usize,
    orientation: usize,
    x: usize,
    y: usize,
) -> bool {
    let candidate = input.get(&candidate_id).unwrap();
    if x > 0 {
        if let Some(left_tile) = &tiles[y][x - 1] {
            let left = input.get(&left_tile.id).unwrap();
            if !left_right_match(
                left,
                left_tile.orientation,
                candidate,
                orientation,
            ) {
                return false;
            }
        }
    }
    if y > 0 {
        if let Some(top_tile) = &tiles[y - 1][x] {
            let top = input.get(&top_tile.id).unwrap();
            if !top_bottom_match(
                top,
                top_tile.orientation,
                candidate,
                orientation,
            ) {
                return false;
            }
        }
    }
    true
}

fn find_fit(
    input: &HashMap<usize, Vec<Vec<char>>>,
    tiles: &mut Vec<Vec<Option<Tile>>>,
    unused_tiles: &mut HashSet<usize>,
    x: usize,
    y: usize,
) {
    for candidate_id in unused_tiles.iter() {
        for orientation in 0..8 {
            if fits(input, tiles, *candidate_id, orientation, x, y) {
                tiles[y][x] = Some(Tile {
                    id: *candidate_id,
                    orientation,
                });
            }
        }
    }
}

const SIZE: usize = 12;

fn solve_already_parsed(
    input: &HashMap<usize, Vec<Vec<char>>>
) -> Option<usize> {
    let mut tiles = vec![vec![None; SIZE]; SIZE];
    let mut unused_tiles = input.keys().copied().collect::<HashSet<_>>();
    let first_tile = *unused_tiles
        .iter()
        .find(|&&candidate_id| {
            let candidate = input.get(&candidate_id).unwrap();
            !unused_tiles.iter().any(|&other_id| {
                if other_id == candidate_id {
                    return false;
                }
                let other = input.get(&other_id).unwrap();
                (0..8).any(|orientation| {
                    left_right_match(other, orientation, candidate, 0)
                        || top_bottom_match(other, orientation, candidate, 0)
                })
            })
        })
        .unwrap();
    tiles[0][0] = Some(Tile {
        id: first_tile,
        orientation: 0,
    });
    unused_tiles.remove(&first_tile);
    for y in 0..SIZE {
        for x in 0..SIZE {
            if x == 0 && y == 0 {
                continue;
            }
            find_fit(input, &mut tiles, &mut unused_tiles, x, y);
            if let Some(tile) = &tiles[y][x] {
                unused_tiles.remove(&tile.id);
            }
        }
    }
    Some(
        tiles[0][0].take().unwrap().id
            * tiles[SIZE - 1][0].take().unwrap().id
            * tiles[SIZE - 1][SIZE - 1].take().unwrap().id
            * tiles[0][SIZE - 1].take().unwrap().id,
    )
}

const ITERATIONS: usize = 100;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
