use aoc_2020::benchmarked_main;
use std::collections::{
    HashMap,
    HashSet,
};

type Bag = HashMap<String, usize>;

fn chomp<'a>(
    line: &'a str,
    delimiter: &str,
) -> (&'a str, &'a str) {
    let mut parts = line.splitn(2, delimiter);
    let left = parts.next().unwrap();
    let right = parts.next().unwrap();
    (left, right)
}

fn chomp_bag(line: &str) -> (String, &str) {
    let (color, rest) = chomp(line, "bag");
    let color = String::from(color.trim());
    (color, rest)
}

fn split_on_next_number(input: &str) -> Option<(usize, &str)> {
    input.find(|ch: char| ch.is_digit(10)).map(|start| {
        let input = &input[start..];
        let end = input.find(|ch: char| !ch.is_digit(10)).unwrap();
        (input[..end].parse().unwrap(), &input[end..])
    })
}

fn parse(input: &str) -> HashMap<String, Bag> {
    input
        .lines()
        .map(|line| {
            let (color, mut line) = chomp_bag(line);
            let mut bag = Bag::default();
            while let Some((quantity, rest)) = split_on_next_number(line) {
                let (color, rest) = chomp_bag(rest);
                *bag.entry(color).or_default() += quantity;
                line = rest;
            }
            (color, bag)
        })
        .collect::<HashMap<_, _>>()
}

// These functions were used in my original solution, but they proved
// to be very slow.  I used them nonetheless to get my rankings,
// poor as they are.  I am preserving them here commented out for posterity.
//
// fn make_bag<T: Into<String>>(
//     color: T,
//     quantity: usize,
// ) -> Bag {
//     std::iter::once((color.into(), quantity)).collect::<Bag>()
// }
//
// fn expand_bag(
//     rules: &HashMap<String, Bag>,
//     outer_color: &str,
//     outer_quantity: usize,
//     target_color: &str,
// ) -> Bag {
//     let bag = &rules[outer_color];
//     if bag.is_empty() {
//         make_bag(outer_color, outer_quantity)
//     } else {
//         bag.iter()
//             .map(|(color, quantity)| {
//                 let inner_quantity = quantity * outer_quantity;
//                 if color == target_color {
//                     make_bag(color, inner_quantity)
//                 } else {
//                     expand_bag(rules, color, inner_quantity, target_color)
//                 }
//             })
//             .fold(Bag::default(), |mut acc, bag| {
//                 for (color, quantity) in bag {
//                     *acc.entry(color).or_default() += quantity;
//                 }
//                 acc
//             })
//     }
// }
//
// fn solve_already_parsed(rules: &HashMap<String, Bag>) -> Option<usize> {
//     Some(
//         rules
//             .keys()
//             .filter(|color| {
//                 let expansion = expand_bag(rules, color, 1, "shiny gold");
//                 expansion.contains_key("shiny gold")
//             })
//             .count(),
//     )
// }

fn solve_already_parsed(rules: &HashMap<String, Bag>) -> Option<usize> {
    let mut matching_colors = HashSet::new();
    matching_colors.insert("shiny gold");
    loop {
        let mut new_matches_found = false;
        for (candidate_color, bag) in rules {
            if matching_colors.contains(candidate_color.as_str()) {
                continue;
            }
            for color in bag.keys() {
                if matching_colors.contains(color.as_str()) {
                    matching_colors.insert(candidate_color);
                    new_matches_found = true;
                    break;
                }
            }
        }
        if !new_matches_found {
            break;
        }
    }
    Some(matching_colors.len() - 1)
}

const ITERATIONS: usize = 1000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
