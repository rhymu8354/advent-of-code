#[cfg(not(feature = "visualize"))]
use aoc_2020::benchmarked_main;
#[cfg(feature = "visualize")]
use aoc_2020::unbenchmarked_main;

fn decode(
    input: &str,
    one_flag: char,
) -> usize {
    input.chars().fold(0, |acc, ch| {
        (acc << 1)
            + if ch == one_flag {
                1
            } else {
                0
            }
    })
}

fn parse(input: &str) -> Vec<Vec<usize>> {
    let mut cols = vec![Vec::with_capacity(128); 8];
    for line in input.lines() {
        let line = line.trim();
        let row = decode(&line[..7], 'B');
        let col = decode(&line[7..], 'R');
        cols[col].push(row);
    }
    for col in &mut cols {
        col.sort_unstable();
    }
    cols
}

#[cfg(feature = "visualize")]
fn visualize_seats<T: AsRef<[Vec<usize>]>>(input: &T) {
    let input = input.as_ref();
    let mut plot = vec![vec!['.'; 8]; 128];
    for (col_num, col) in input.iter().enumerate() {
        for row in col {
            plot[*row][col_num] = '#';
        }
    }
    println!("    01234567");
    (0..128).for_each(|row| {
        println!("{:3} {}", row, plot[row].iter().collect::<String>());
    });
}

fn solve_already_parsed<T: AsRef<[Vec<usize>]>>(input: &T) -> Option<usize> {
    let input = input.as_ref();
    #[cfg(feature = "visualize")]
    visualize_seats(&input);
    for (col_num, col) in input.iter().enumerate() {
        for window in col.windows(2) {
            let seat = window[0] + 1;
            if seat != window[1] {
                return Some((seat << 3) + col_num);
            }
        }
    }
    None
}

#[cfg(not(feature = "visualize"))]
const ITERATIONS: usize = 10000;

fn main() {
    #[cfg(feature = "visualize")]
    unbenchmarked_main(parse, solve_already_parsed);
    #[cfg(not(feature = "visualize"))]
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
