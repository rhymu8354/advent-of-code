use aoc_2020::benchmarked_main;
use std::collections::HashMap;

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

fn expand_bag(
    rules: &HashMap<String, Bag>,
    outer_color: &str,
    outer_quantity: usize,
) -> usize {
    let bag = &rules[outer_color];
    if bag.is_empty() {
        outer_quantity
    } else {
        outer_quantity
            + bag
                .iter()
                .map(|(color, quantity)| {
                    let inner_quantity = quantity * outer_quantity;
                    expand_bag(rules, color, inner_quantity)
                })
                .sum::<usize>()
    }
}

fn solve_already_parsed(rules: &HashMap<String, Bag>) -> Option<usize> {
    Some(expand_bag(rules, "shiny gold", 1) - 1)
}

const ITERATIONS: usize = 1000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
