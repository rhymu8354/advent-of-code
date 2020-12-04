use aoc_2020::benchmarked_main;

struct PasswordWithPolicy {
    first: usize,
    second: usize,
    ch: char,
    password: String,
}

fn chomp(
    line: &str,
    delimiter: char,
) -> (&str, &str) {
    let mut parts = line.splitn(2, delimiter);
    let left = parts.next().unwrap();
    let right = parts.next().unwrap();
    (left, right)
}

fn chomp_number(
    line: &str,
    delimiter: char,
) -> (usize, &str) {
    let (left, right) = chomp(line, delimiter);
    (left.parse().unwrap(), right)
}

// Each line must adhere to the following regular expression:
// (\d+)-(\d+) (.): (.*)
//   \        \   \     \
//    \        \   -- ch -- password
//     -- first -- second
fn scan(line: &str) -> PasswordWithPolicy {
    let (first, line) = chomp_number(line, '-');
    let (second, line) = chomp_number(line, ' ');
    let ch = line.chars().next().unwrap();
    let password_start = ch.len_utf8() + ": ".len();
    let password = String::from(&line[password_start..]);
    PasswordWithPolicy {
        first,
        second,
        ch,
        password,
    }
}

fn is_valid_password(
    PasswordWithPolicy {
        first,
        second,
        ch,
        password,
    }: &PasswordWithPolicy
) -> bool {
    [*first, *second]
        .iter()
        .filter(|&&index| password.chars().nth(index - 1).unwrap() == *ch)
        .count()
        == 1
}

fn parse(input: &str) -> Vec<PasswordWithPolicy> {
    input.lines().map(|line| scan(line.trim())).collect()
}

fn solve_already_parsed<T: AsRef<[PasswordWithPolicy]>>(
    input: &T
) -> Option<usize> {
    let input = input.as_ref();
    Some(
        input
            .iter()
            .filter(|password_with_policy| {
                is_valid_password(password_with_policy)
            })
            .count(),
    )
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
