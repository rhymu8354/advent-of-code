use aoc_2020::benchmarked_main;
use std::ops::RangeInclusive;

type Rule = Vec<RangeInclusive<usize>>;
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
                    let line =
                        &line[line.find(|ch: char| ch.is_digit(10)).unwrap()..];
                    data.rules.push(
                        line.split(" or ")
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
                    );
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
    Some(
        input
            .other_tickets
            .iter()
            .filter_map(|ticket| {
                ticket.iter().find(|number| {
                    !input.rules.iter().any(|rule| {
                        rule.iter().any(|range| range.contains(number))
                    })
                })
            })
            .sum(),
    )
}

const ITERATIONS: usize = 1000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
