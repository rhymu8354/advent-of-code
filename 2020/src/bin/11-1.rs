use aoc_2020::benchmarked_main;

fn parse(input: &str) -> Vec<Vec<char>> {
    input
        .lines()
        .map(|line| line.trim().chars().collect::<Vec<_>>())
        .collect::<Vec<_>>()
}

fn count_neighbor(
    counts: &mut Vec<Vec<usize>>,
    cols: usize,
    x: usize,
    y: usize,
) {
    let rows = counts.len();
    for dy in 0..3 {
        for dx in 0..3 {
            if dx == 1 && dy == 1 {
                continue;
            }
            if (y + dy >= 1)
                && (x + dx >= 1)
                && (y + dy <= rows)
                && (x + dx <= cols)
            {
                counts[y + dy - 1][x + dx - 1] += 1;
            }
        }
    }
}

fn count_occupied_neighbors(
    grid: &[Vec<char>],
    cols: usize,
) -> Vec<Vec<usize>> {
    let rows = grid.len();
    let mut counts = vec![vec![0; cols]; rows];
    #[allow(clippy::needless_range_loop)]
    for y in 0..rows {
        for x in 0..cols {
            if grid[y][x] == '#' {
                count_neighbor(&mut counts, cols, x, y);
            }
        }
    }
    counts
}

fn simulate(
    grid: &mut Vec<Vec<char>>,
    cols: usize,
) {
    loop {
        let mut changes = false;
        let occupied_neighbors = count_occupied_neighbors(&grid, cols);
        for y in 0..grid.len() {
            for x in 0..cols {
                match (grid[y][x], occupied_neighbors[y][x]) {
                    ('L', 0) => {
                        changes = true;
                        grid[y][x] = '#';
                    },
                    ('#', n) if n >= 4 => {
                        changes = true;
                        grid[y][x] = 'L';
                    },
                    _ => (),
                }
            }
        }
        if !changes {
            break;
        }
    }
}

fn solve_already_parsed<T: AsRef<[Vec<char>]>>(input: &T) -> Option<usize> {
    let mut grid = input.as_ref().to_vec();
    if let Some(row) = grid.get(0) {
        let cols = row.len();
        simulate(&mut grid, cols);
        Some(grid.iter().fold(0, |acc, row| {
            acc + row.iter().filter(|&&ch| ch == '#').count()
        }))
    } else {
        None
    }
}

const ITERATIONS: usize = 100;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
