use std::{
    fmt::{
        Debug,
        Display,
    },
    io::{
        stdin,
        Read,
    },
    time::{
        Duration,
        Instant,
    },
};

pub fn benchmark<F>(
    iterations: usize,
    f: F,
) -> Duration
where
    F: Fn(),
{
    let mut total = Duration::default();
    for _ in 0..iterations {
        let start = Instant::now();
        f();
        let elapsed = start.elapsed();
        total += elapsed;
    }
    total / iterations as u32
}

fn solve<T, U, P, S>(
    input: &str,
    parse: &P,
    solve_already_parsed: &S,
) -> Option<U>
where
    P: Fn(&str) -> T,
    S: Fn(&T) -> Option<U>,
{
    solve_already_parsed(&parse(input))
}

pub fn benchmarked_main<T, U, P, S>(
    parse: P,
    solve_already_parsed: S,
    iterations: usize,
) where
    P: Fn(&str) -> T,
    S: Fn(&T) -> Option<U>,
    U: Debug + Display + PartialEq,
{
    let mut input = String::new();
    stdin().read_to_string(&mut input).unwrap();
    let solution = solve(&input, &parse, &solve_already_parsed).unwrap();
    println!("Solution: {}", solution);
    let overhead = benchmark(iterations, || ());
    let time_with_parsing = benchmark(iterations, || {
        let solution_repeat =
            solve(&input, &parse, &solve_already_parsed).unwrap();
        assert_eq!(solution_repeat, solution);
    });
    let map = parse(&input);
    let total_without_parsing = benchmark(iterations, || {
        let solution_repeat = solve_already_parsed(&map).unwrap();
        assert_eq!(solution_repeat, solution);
    });
    println!("Time (with parsing): {:?}", time_with_parsing);
    println!("Time (without parsing): {:?}", total_without_parsing);
    println!("Overhead: {:?}", overhead);
    println!(
        "Time (with parsing) - Overhead: {:?}",
        (time_with_parsing - overhead)
    );
    println!(
        "Time (without parsing) - Overhead: {:?}",
        (total_without_parsing - overhead)
    );
}
