use aoc_2020::benchmarked_main;

struct Bus {
    id: usize,
    offset: usize,
}

fn parse(input: &str) -> Vec<Bus> {
    let mut input = input.lines();
    let _ = input.next();
    let line2 = input.next().unwrap().trim();
    line2
        .split(',')
        .enumerate()
        .filter_map(|(offset, id)| {
            id.parse().ok().map(|id| Bus {
                id,
                offset,
            })
        })
        .collect::<Vec<_>>()
}

fn find_pattern_match(
    x: usize,
    y: usize,
    offset: usize,
) -> usize {
    let mut xn = x;
    while (xn + offset) % y != 0 {
        xn += x;
    }
    xn
}

fn solve_already_parsed<T: AsRef<[Bus]>>(input: &T) -> Option<usize> {
    let busses = input.as_ref().iter();
    let mut x = 1;
    let mut xo = 0;
    for Bus {
        id: y,
        offset: yo,
    } in busses
    {
        xo += find_pattern_match(x, *y, xo + yo);
        x *= y;
    }
    Some(xo)
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
