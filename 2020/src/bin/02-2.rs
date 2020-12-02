use std::io::{
    stdin,
    Read,
};

struct PasswordWithPolicy<'a> {
    first: usize,
    second: usize,
    ch: char,
    password: &'a str,
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
    let password = &line[password_start..];
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

fn main() {
    let mut input = String::new();
    stdin().read_to_string(&mut input).unwrap();
    let valid_passwords = input
        .lines()
        .map(|line| scan(line.trim()))
        .filter(|password_with_policy| is_valid_password(password_with_policy))
        .count();
    println!("There are {} valid passwords.", valid_passwords);
}
