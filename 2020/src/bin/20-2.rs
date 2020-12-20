use aoc_2020::benchmarked_main;
use std::{
    collections::{
        HashMap,
        HashSet,
    },
    panic,
};

const PICTURE_SIZE_TILES: usize = 12;

enum ParsePhase {
    Header,
    Body(usize),
    Separator,
}

fn parse(input: &str) -> HashMap<usize, Vec<Vec<char>>> {
    let mut data =
        HashMap::with_capacity(PICTURE_SIZE_TILES * PICTURE_SIZE_TILES);
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

struct OrientationScan {
    ox: isize,
    oy: isize,
    sx: isize,
    sy: isize,
    x: isize,
    y: isize,
    dx: isize,
    dy: isize,
    sx1: isize,
    sy1: isize,
    sx2: isize,
    sy2: isize,
}

impl Iterator for OrientationScan {
    type Item = (usize, usize);

    fn next(&mut self) -> Option<Self::Item> {
        if self.dy < self.sy {
            let x = (self.ox
                + self.sx1 * (self.x + self.dx)
                + self.sx2 * (self.y + self.dy)) as usize;
            let y = (self.oy
                + self.sy1 * (self.x + self.dx)
                + self.sy2 * (self.y + self.dy)) as usize;
            self.dx += 1;
            if self.dx == self.sx {
                self.dx = 0;
                self.dy += 1;
            }
            Some((x, y))
        } else {
            None
        }
    }
}

fn scan(
    orientation: usize,
    size: usize,
    x: usize,
    y: usize,
    sx: usize,
    sy: usize,
) -> OrientationScan {
    let (ox, oy, dx1, dy1, dx2, dy2) = ORIENTATION_SCANS[orientation];
    OrientationScan {
        ox: ox * (size - 1) as isize,
        oy: oy * (size - 1) as isize,
        sx: sx as isize,
        sy: sy as isize,
        x: x as isize,
        y: y as isize,
        dx: 0,
        dy: 0,
        sx1: dx1,
        sy1: dy1,
        sx2: dx2,
        sy2: dy2,
    }
}

fn edge_match(
    first: &[Vec<char>],
    first_orientation: usize,
    second: &[Vec<char>],
    second_orientation: usize,
) -> bool {
    scan(first_orientation, 10, 0, 0, 10, 1)
        .zip(scan(second_orientation, 10, 0, 0, 10, 1))
        .all(|((x1, y1), (x2, y2))| first[y1][x1] == second[y2][x2])
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

const SEA_MONSTER: [char; 20 * 3] = [
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', '#', ' ', '#', ' ', ' ', ' ', ' ', '#', '#', ' ', ' ', ' ',
    ' ', '#', '#', ' ', ' ', ' ', ' ', '#', '#', '#', ' ', '#', ' ', ' ', '#',
    ' ', ' ', '#', ' ', ' ', '#', ' ', ' ', '#', ' ', ' ', '#', ' ', ' ', ' ',
];

fn sea_monsters(
    picture: &[Vec<char>],
    orientation: usize,
) -> Option<usize> {
    let mut filtered_picture = picture.to_vec();
    let mut monsters = 0;
    while let Some((x, y)) = (0..=PICTURE_SIZE_TILES * 8 - 3).find_map(|y| {
        (0..=PICTURE_SIZE_TILES * 8 - 20).find_map(|x| {
            let found_sea_monster =
                scan(orientation, PICTURE_SIZE_TILES * 8, x, y, 20, 3)
                    .zip(SEA_MONSTER.iter())
                    .all(|((x, y), sea_monster_char)| {
                        *sea_monster_char != '#'
                            || filtered_picture[y][x] == '#'
                    });
            if found_sea_monster {
                Some((x, y))
            } else {
                None
            }
        })
    }) {
        monsters += 1;
        for ((x, y), sea_monster_cell) in
            scan(orientation, PICTURE_SIZE_TILES * 8, x, y, 20, 3)
                .zip(SEA_MONSTER.iter())
        {
            if *sea_monster_cell == '#' {
                filtered_picture[y][x] = 'O';
            }
        }
    }
    if monsters == 0 {
        None
    } else {
        #[cfg(debug)]
        for (i, (x, y)) in
            scan(orientation, SIZE * 8, 0, 0, SIZE * 8, SIZE * 8).enumerate()
        {
            print!("{}", filtered_picture[y][x]);
            if (i + 1) % (SIZE * 8) == 0 {
                println!()
            }
        }
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
    let mut tiles = vec![vec![None; PICTURE_SIZE_TILES]; PICTURE_SIZE_TILES];
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
    for y in 0..PICTURE_SIZE_TILES {
        for x in 0..PICTURE_SIZE_TILES {
            if x == 0 && y == 0 {
                continue;
            }
            find_fit(input, &mut tiles, &mut unused_tiles, x, y);
            if let Some(tile) = &tiles[y][x] {
                unused_tiles.remove(&tile.id);
            }
        }
    }
    let mut picture =
        vec![vec![' '; PICTURE_SIZE_TILES * 8]; PICTURE_SIZE_TILES * 8];
    for (ty, tile_row) in tiles.into_iter().enumerate() {
        for (tx, mut tile) in tile_row.into_iter().enumerate() {
            let tile = tile.take().unwrap();
            let orientation = tile.orientation;
            let tile = input.get(&tile.id).unwrap();
            for (i, (x, y)) in scan(orientation, 10, 1, 1, 8, 8).enumerate() {
                picture[ty * 8 + i / 8][tx * 8 + i % 8] = tile[y][x];
            }
        }
    }
    (0..8).find_map(|orientation| sea_monsters(&picture, orientation))
}

const ITERATIONS: usize = 100;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
