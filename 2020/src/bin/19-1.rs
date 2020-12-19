use aoc_2020::benchmarked_main;
use std::collections::HashMap;

#[derive(Debug)]
enum Rule {
    Letter(char),
    Sequence(Vec<usize>),
    Alternatives(Vec<Vec<usize>>),
}

#[derive(Debug, Default)]
struct Data {
    rules: HashMap<usize, Rule>,
    messages: Vec<String>,
}

enum ParsePhase {
    Rules,
    Messages,
}

fn chomp(
    line: &str,
    delimiter: char,
) -> (&str, &str) {
    let mut parts = line.splitn(2, delimiter);
    let left = parts.next().unwrap().trim();
    let right = parts.next().unwrap().trim();
    (left, right)
}

fn parse(input: &str) -> Data {
    let mut data = Data::default();
    let mut parse_phase = ParsePhase::Rules;
    for line in input.lines().map(|line| line.trim()) {
        if line.is_empty() {
            parse_phase = match parse_phase {
                ParsePhase::Rules => ParsePhase::Messages,
                _ => panic!("bad input"),
            };
        } else {
            match parse_phase {
                ParsePhase::Rules => {
                    let (rule_num, rule) = chomp(line, ':');
                    data.rules.insert(
                        rule_num.parse().unwrap(),
                        if rule.starts_with('"') {
                            Rule::Letter(
                                rule.split('"')
                                    .nth(1)
                                    .unwrap()
                                    .chars()
                                    .next()
                                    .unwrap(),
                            )
                        } else if rule.contains('|') {
                            Rule::Alternatives(
                                rule.split('|')
                                    .map(str::trim)
                                    .map(|rules| {
                                        rules
                                            .split(' ')
                                            .map(str::parse)
                                            .map(Result::unwrap)
                                            .collect::<Vec<_>>()
                                    })
                                    .collect::<Vec<_>>(),
                            )
                        } else {
                            Rule::Sequence(
                                rule.split(' ')
                                    .map(str::parse)
                                    .map(Result::unwrap)
                                    .collect::<Vec<_>>(),
                            )
                        },
                    );
                },
                ParsePhase::Messages => {
                    data.messages.push(String::from(line));
                },
            };
        }
    }
    data
}

fn solve_already_parsed(input: &Data) -> Option<usize> {
    // println!("Rules: {:?}", input.rules);
    Some(
        input
            .messages
            .iter()
            .filter(|message| {
                let mut possibilities = vec![(vec![0], 0)];
                let message = message.chars().collect::<Vec<_>>();
                while !possibilities.is_empty() {
                    let mut new_possibilities = vec![];
                    for (possibility, index) in possibilities.into_iter() {
                        let mut possibility = possibility.into_iter();
                        if let Some(rule_num) = possibility.next() {
                            match input.rules.get(&rule_num).unwrap() {
                                Rule::Letter(ch) => {
                                    if *ch == message[index] {
                                        let new_possibility = (
                                            possibility.collect::<Vec<_>>(),
                                            index + 1,
                                        );
                                        if new_possibility.0.is_empty()
                                            && new_possibility.1
                                                == message.len()
                                        {
                                            return true;
                                        }
                                        if index + 1 < message.len() {
                                            new_possibilities
                                                .push(new_possibility);
                                        }
                                    }
                                },
                                Rule::Sequence(seq) => {
                                    new_possibilities.push((
                                        seq.iter()
                                            .copied()
                                            .chain(possibility)
                                            .collect::<Vec<_>>(),
                                        index,
                                    ));
                                },
                                Rule::Alternatives(alts) => {
                                    for alt in alts {
                                        new_possibilities.push((
                                            alt.iter()
                                                .copied()
                                                .chain(possibility.clone())
                                                .collect::<Vec<_>>(),
                                            index,
                                        ));
                                    }
                                },
                            }
                        }
                    }
                    possibilities = new_possibilities;
                }
                false
            })
            .count(),
    )
}

const ITERATIONS: usize = 10;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
