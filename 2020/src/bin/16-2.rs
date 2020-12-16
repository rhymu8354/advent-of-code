use aoc_2020::benchmarked_main;
use std::{
    collections::{
        HashMap,
        HashSet,
    },
    ops::RangeInclusive,
};

#[derive(Debug, Default)]
struct Rule {
    departure: bool,
    ranges: Vec<RangeInclusive<usize>>,
}

type Ticket = Vec<usize>;

#[derive(Debug, Default)]
struct Data {
    rules: Vec<Rule>,
    our_ticket: Ticket,
    other_tickets: Vec<Ticket>,
}

enum ParsePhase {
    Rules,
    OurTicketHeader,
    OurTicketBody,
    OtherTicketsHeader,
    OtherTicketsBody,
}

fn parse_ticket(line: &str) -> Vec<usize> {
    line.trim()
        .split(',')
        .map(str::parse)
        .map(Result::unwrap)
        .collect::<Vec<_>>()
}

fn parse(input: &str) -> Data {
    let mut data = Data::default();
    let mut parse_phase = ParsePhase::Rules;
    for line in input.lines().map(|line| line.trim()) {
        if line.is_empty() {
            parse_phase = match parse_phase {
                ParsePhase::Rules => ParsePhase::OurTicketHeader,
                ParsePhase::OurTicketBody => ParsePhase::OtherTicketsHeader,
                _ => panic!("bad input"),
            };
        } else {
            match parse_phase {
                ParsePhase::Rules => {
                    let departure = line.starts_with("departure ");
                    let ranges =
                        &line[line.find(|ch: char| ch.is_digit(10)).unwrap()..];
                    data.rules.push(Rule {
                        departure,
                        ranges: ranges
                            .split(" or ")
                            .map(|range| {
                                let mut range_parts = range
                                    .split('-')
                                    .map(str::parse)
                                    .map(Result::unwrap);
                                let start = range_parts.next().unwrap();
                                let end = range_parts.next().unwrap();
                                start..=end
                            })
                            .collect::<Vec<_>>(),
                    });
                },
                ParsePhase::OurTicketHeader => {
                    parse_phase = ParsePhase::OurTicketBody;
                },
                ParsePhase::OurTicketBody => {
                    data.our_ticket = parse_ticket(line);
                },
                ParsePhase::OtherTicketsHeader => {
                    parse_phase = ParsePhase::OtherTicketsBody;
                },
                ParsePhase::OtherTicketsBody => {
                    data.other_tickets.push(parse_ticket(line));
                },
            };
        }
    }
    data
}

fn solve_already_parsed(input: &Data) -> Option<usize> {
    let mut rule_col_possibilities =
        vec![(0..input.rules.len()).collect::<HashSet<_>>(); input.rules.len()];
    for ticket in input.other_tickets.iter().filter(|ticket| {
        ticket.iter().all(|number| {
            input.rules.iter().any(|rule| {
                rule.ranges.iter().any(|range| range.contains(number))
            })
        })
    }) {
        for (col, number) in ticket.iter().enumerate() {
            for (rule_num, possible_cols) in
                rule_col_possibilities.iter_mut().enumerate()
            {
                if possible_cols.len() > 1
                    && possible_cols.contains(&col)
                    && input.rules[rule_num]
                        .ranges
                        .iter()
                        .all(|range| !range.contains(number))
                {
                    possible_cols.remove(&col);
                }
            }
        }
    }
    let mut rule_col = HashMap::new();
    loop {
        let mut cols_solved = HashSet::new();
        for (rule_num, possible_cols) in
            rule_col_possibilities.iter().enumerate()
        {
            if possible_cols.len() == 1 {
                let col = possible_cols.iter().next().unwrap();
                if rule_col.insert(rule_num, *col).is_none() {
                    cols_solved.insert(*col);
                }
            }
        }
        if cols_solved.is_empty() {
            break;
        }
        for rule in &mut rule_col_possibilities {
            if rule.len() == 1 {
                continue;
            }
            for col_solved in &cols_solved {
                rule.remove(&col_solved);
            }
        }
    }
    Some(
        input
            .rules
            .iter()
            .enumerate()
            .filter(|(_, rule)| rule.departure)
            .map(|(rule_num, _)| rule_col[&rule_num])
            .map(|col| input.our_ticket[col])
            .product(),
    )
}

const ITERATIONS: usize = 100;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
