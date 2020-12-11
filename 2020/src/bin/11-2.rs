use aoc_2020::benchmarked_main;

fn parse(input: &str) -> Vec<Vec<char>> {
    input
        .lines()
        .map(|line| line.trim().chars().collect::<Vec<_>>())
        .collect::<Vec<_>>()
}

fn ray_hits_occupied_seat(
    grid: &[Vec<char>],
    cols: isize,
    mut x: isize,
    mut y: isize,
    dx: isize,
    dy: isize,
) -> bool {
    let rows = grid.len() as isize;
    loop {
        x += dx;
        y += dy;
        if y < 0 || y >= rows || x < 0 || x >= cols {
            break false;
        }
        match grid[y as usize][x as usize] {
            'L' => break false,
            '#' => break true,
            _ => (),
        }
    }
}

fn occupied_seats_found(
    grid: &[Vec<char>],
    cols: usize,
    x: usize,
    y: usize,
) -> usize {
    let mut occupied_seats_found = 0;
    for dy in -1..=1 {
        for dx in -1..=1 {
            if dx == 0 && dy == 0 {
                continue;
            }
            if ray_hits_occupied_seat(
                &grid,
                cols as isize,
                x as isize,
                y as isize,
                dx,
                dy,
            ) {
                occupied_seats_found += 1;
            }
        }
    }
    occupied_seats_found
}

fn simulate_round(
    grid: &mut Vec<Vec<char>>,
    cols: usize,
) -> bool {
    let mut changes = false;
    let old_grid = grid.clone();
    for y in 0..grid.len() {
        for x in 0..cols {
            match (old_grid[y][x], occupied_seats_found(&old_grid, cols, x, y))
            {
                ('L', 0) => {
                    changes = true;
                    grid[y][x] = '#';
                },
                ('#', n) if n >= 5 => {
                    changes = true;
                    grid[y][x] = 'L';
                },
                _ => (),
            }
        }
    }
    changes
}

fn solve_already_parsed<T: AsRef<[Vec<char>]>>(input: &T) -> Option<usize> {
    let mut grid = input.as_ref().to_vec();
    if let Some(row) = grid.get(0) {
        let cols = row.len();
        while simulate_round(&mut grid, cols) {}
        Some(grid.iter().fold(0, |acc, row| {
            acc + row.iter().filter(|&&ch| ch == '#').count()
        }))
    } else {
        None
    }
}

const ITERATIONS: usize = 10;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
