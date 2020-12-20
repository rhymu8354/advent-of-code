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
    let mut data = HashMap::with_capacity(SIZE * SIZE);
    let mut parse_phase = ParsePhase::Header;
    let mut id = 0;
    let mut body = Vec::with_capacity(10);
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
                    body = Vec::with_capacity(10);
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

const ORIENTATION_SCANS: &[(isize, isize, isize, isize, isize, isize)] = &[
    (0, 0, 1, 0, 0, 1),   // 0
    (1, 0, 0, 1, -1, 0),  // 1
    (1, 1, -1, 0, 0, -1), // 2
    (0, 1, 0, -1, 1, 0),  // 3
    (1, 0, -1, 0, 0, 1),  // 4
    (0, 0, 0, 1, 1, 0),   // 5
    (0, 1, 1, 0, 0, -1),  // 6
    (1, 1, 0, -1, -1, 0), // 7
];

const TOP: [usize; ORIENTATION_SCANS.len()] = [0, 1, 2, 3, 4, 5, 6, 7];
const BOTTOM: [usize; ORIENTATION_SCANS.len()] = [6, 5, 4, 7, 2, 1, 0, 3];
const LEFT: [usize; ORIENTATION_SCANS.len()] = [5, 4, 7, 6, 1, 0, 3, 2];
const RIGHT: [usize; ORIENTATION_SCANS.len()] = [1, 2, 3, 0, 5, 6, 7, 4];

struct OrientationScan<'a> {
    tile: &'a [Vec<char>],
    ox: isize,
    oy: isize,
    sx: isize,
    sy: isize,
    x: isize,
    y: isize,
    dx1: isize,
    dy1: isize,
    dx2: isize,
    dy2: isize,
}

impl<'a> Iterator for OrientationScan<'a> {
    type Item = &'a char;

    fn next(&mut self) -> Option<Self::Item> {
        if self.y < self.sy {
            let ch = &self.tile
                [(self.oy + self.dy1 * self.x + self.dy2 * self.y) as usize]
                [(self.ox + self.dx1 * self.x + self.dx2 * self.y) as usize];
            self.x += 1;
            if self.x == self.sx {
                self.x = 0;
                self.y += 1;
            }
            Some(ch)
        } else {
            None
        }
    }
}

fn scan(
    tile: &[Vec<char>],
    orientation: usize,
    size: usize,
    sx: usize,
    sy: usize,
) -> OrientationScan {
    let (ox, oy, dx1, dy1, dx2, dy2) = ORIENTATION_SCANS[orientation];
    OrientationScan {
        tile,
        ox: ox * (size - 1) as isize,
        oy: oy * (size - 1) as isize,
        sx: sx as isize,
        sy: sy as isize,
        x: 0,
        y: 0,
        dx1,
        dy1,
        dx2,
        dy2,
    }
}

fn edge_match(
    first: &[Vec<char>],
    first_orientation: usize,
    second: &[Vec<char>],
    second_orientation: usize,
) -> bool {
    scan(first, first_orientation, SIZE, SIZE, 1)
        .zip(scan(second, second_orientation, SIZE, SIZE, 1))
        .all(|(first_char, second_char)| first_char == second_char)
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
            if !edge_match(
                left,
                RIGHT[left_tile.orientation],
                candidate,
                LEFT[orientation],
            ) {
                return false;
            }
        }
    }
    if y > 0 {
        if let Some(top_tile) = &tiles[y - 1][x] {
            let top = input.get(&top_tile.id).unwrap();
            if !edge_match(
                top,
                BOTTOM[top_tile.orientation],
                candidate,
                TOP[orientation],
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

const SEA_MONSTER: [[char; 20]; 3] = [
    [
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        ' ', ' ', ' ', ' ', '#', ' ',
    ],
    [
        '#', ' ', ' ', ' ', ' ', '#', '#', ' ', ' ', ' ', ' ', '#', '#', ' ',
        ' ', ' ', ' ', '#', '#', '#',
    ],
    [
        ' ', '#', ' ', ' ', '#', ' ', ' ', '#', ' ', ' ', '#', ' ', ' ', '#',
        ' ', ' ', '#', ' ', ' ', ' ',
    ],
];

fn sea_monsters(
    picture: &[[char; SIZE * 8]; SIZE * 8],
    orientation: usize,
) -> Option<usize> {
    let mut filtered_picture = *picture;
    let mut monsters = 0;
    let (ox, oy, dx1, dy1, dx2, dy2) = ORIENTATION_SCANS[orientation];
    let ox = ox * ((SIZE * 8) - 1) as isize;
    let oy = oy * ((SIZE * 8) - 1) as isize;
    while let Some((x, y)) = (0..=SIZE * 8 - 3).find_map(|y| {
        (0..=SIZE * 8 - 20).find_map(|x| {
            if (0..3).all(|dy| {
                let yi = (y + dy) as isize;
                (0..20).all(|dx| {
                    let xi = (x + dx) as isize;
                    SEA_MONSTER[dy][dx] != '#'
                        || filtered_picture[(oy + dy1 * xi + dy2 * yi) as usize]
                            [(ox + dx1 * xi + dx2 * yi) as usize]
                            == '#'
                })
            }) {
                Some((x, y))
            } else {
                None
            }
        })
    }) {
        monsters += 1;
        for (dy, sea_monster_row) in SEA_MONSTER.iter().enumerate() {
            let yi = (y + dy) as isize;
            for (dx, sea_monster_cell) in sea_monster_row.iter().enumerate() {
                let xi = (x + dx) as isize;
                if *sea_monster_cell == '#' {
                    filtered_picture[(oy + dy1 * xi + dy2 * yi) as usize]
                        [(ox + dx1 * xi + dx2 * yi) as usize] = 'O';
                }
            }
        }
    }
    if monsters == 0 {
        None
    } else {
        // for y in 0..SIZE * 8 {
        //     let yi = y as isize;
        //     for x in 0..SIZE * 8 {
        //         let xi = x as isize;
        //         print!(
        //             "{}",
        //             filtered_picture[(oy + dy1 * xi + dy2 * yi) as usize]
        //                 [(ox + dx1 * xi + dx2 * yi) as usize]
        //         );
        //     }
        //     println!()
        // }
        Some(
            filtered_picture
                .iter()
                .map(|row| row.iter().filter(|&&ch| ch == '#').count())
                .sum(),
        )
    }
}

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
                    edge_match(other, RIGHT[orientation], candidate, LEFT[0])
                        || edge_match(
                            other,
                            BOTTOM[orientation],
                            candidate,
                            TOP[0],
                        )
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
    let mut picture = [[' '; SIZE * 8]; SIZE * 8];
    for (ty, tile_row) in tiles.into_iter().enumerate() {
        for (tx, mut tile) in tile_row.into_iter().enumerate() {
            let tile = tile.take().unwrap();
            let orientation = tile.orientation;
            let tile = input.get(&tile.id).unwrap();
            let (ox, oy, dx1, dy1, dx2, dy2) = ORIENTATION_SCANS[orientation];
            for y in 0..8 {
                let yi = (y + 1) as isize;
                for x in 0..8 {
                    let xi = (x + 1) as isize;
                    picture[ty * 8 + y][tx * 8 + x] = tile
                        [(oy * 9 + dy1 * xi + dy2 * yi) as usize]
                        [(ox * 9 + dx1 * xi + dx2 * yi) as usize];
                }
            }
        }
    }
    (0..8).find_map(|orientation| sea_monsters(&picture, orientation))
}

const ITERATIONS: usize = 100;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
