use aoc_2020::benchmarked_main;

#[derive(Debug)]
struct Passport {
    fields: Vec<Option<String>>,
}

impl Default for Passport {
    fn default() -> Self {
        Self {
            fields: vec![None; 8],
        }
    }
}

fn parse(input: &str) -> Vec<Passport> {
    let keys = ["byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid", "cid"];

    let mut passports = Vec::new();
    let mut passport = Passport::default();
    for line in input.lines().map(|line| line.trim()) {
        if line.is_empty() {
            let mut new_passport = Passport::default();
            std::mem::swap(&mut passport, &mut new_passport);
            passports.push(new_passport);
        } else {
            for key_value in line.split(' ') {
                let mut key_value = key_value.split(':');
                let this_key = key_value.next().unwrap();
                let value = key_value.next().unwrap();
                let i = keys
                    .iter()
                    .enumerate()
                    .find(|(_, key)| **key == this_key)
                    .unwrap()
                    .0;
                passport.fields[i] = Some(String::from(value));
            }
        }
    }
    passports.push(passport);
    passports
}

fn solve_already_parsed<T: AsRef<[Passport]>>(input: &T) -> Option<usize> {
    let input = input.as_ref();
    Some(
        input
            .iter()
            .filter(|passport| {
                passport
                    .fields
                    .iter()
                    .take(7)
                    .filter(|value| value.is_none())
                    .count()
                    == 0
            })
            .count(),
    )
}

const ITERATIONS: usize = 4000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
