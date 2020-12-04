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

fn v_byr(value: &str) -> bool {
    if value.chars().count() != 4 {
        return false;
    }
    value
        .parse::<usize>()
        .map(|year| (1920..=2002).contains(&year))
        .unwrap_or(false)
}

fn v_iyr(value: &str) -> bool {
    if value.chars().count() != 4 {
        return false;
    }
    value
        .parse::<usize>()
        .map(|year| (2010..=2020).contains(&year))
        .unwrap_or(false)
}

fn v_eyr(value: &str) -> bool {
    if value.len() != 4 {
        return false;
    }
    value
        .parse::<usize>()
        .map(|year| (2020..=2030).contains(&year))
        .unwrap_or(false)
}

fn v_hgt(value: &str) -> bool {
    let len = value.len();
    if len < 3 {
        return false;
    }
    match &value[len - 2..] {
        "cm" => value[..len - 2]
            .parse::<usize>()
            .map(|cm| (150..=193).contains(&cm))
            .unwrap_or(false),
        "in" => value[..len - 2]
            .parse::<usize>()
            .map(|inches| (59..=76).contains(&inches))
            .unwrap_or(false),
        _ => false,
    }
}

const HEX_CHARS: &[char] = &[
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e',
    'f',
];

fn v_hcl(value: &str) -> bool {
    if value.len() != 7 {
        return false;
    }
    value.chars().enumerate().all(|(i, ch)| match i {
        0 => ch == '#',
        _ => HEX_CHARS.iter().any(|hex_char| *hex_char == ch),
    })
}

const EYE_COLORS: &[&str] = &["amb", "blu", "brn", "gry", "grn", "hzl", "oth"];

fn v_ecl(value: &str) -> bool {
    EYE_COLORS.iter().any(|eye_color| value == *eye_color)
}

fn v_pid(value: &str) -> bool {
    if value.len() != 9 {
        return false;
    }
    value.parse::<usize>().is_ok()
}

fn v_cid(_value: &str) -> bool {
    true
}

type PassportFieldValidator = fn(&str) -> bool;

const PASSPORT_FIELDS: &[(&str, PassportFieldValidator)] = &[
    ("byr", v_byr),
    ("iyr", v_iyr),
    ("eyr", v_eyr),
    ("hgt", v_hgt),
    ("hcl", v_hcl),
    ("ecl", v_ecl),
    ("pid", v_pid),
    ("cid", v_cid),
];

fn validator(i: usize) -> PassportFieldValidator {
    PASSPORT_FIELDS.get(i).unwrap().1
}

fn parse(input: &str) -> Vec<Passport> {
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
                let (i, _) = PASSPORT_FIELDS
                    .iter()
                    .enumerate()
                    .find(|(_, (key, _))| *key == this_key)
                    .unwrap();
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
                    .enumerate()
                    .filter(|(i, value)| {
                        if let Some(value) = value {
                            !validator(*i)(value)
                        } else {
                            true
                        }
                    })
                    .count()
                    == 0
            })
            .count(),
    )
}

const ITERATIONS: usize = 1000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
