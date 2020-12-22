use std::collections::VecDeque;

use aoc_2020::benchmarked_main;

enum ParsePhase {
    Header,
    Card,
}

fn parse(input: &str) -> Vec<VecDeque<usize>> {
    let mut data = vec![];
    let mut cards = VecDeque::new();
    let mut parse_phase = ParsePhase::Header;
    for line in input.lines().map(|line| line.trim()) {
        if line.is_empty() {
            data.push(cards);
            cards = VecDeque::new();
            parse_phase = ParsePhase::Header;
        } else {
            match parse_phase {
                ParsePhase::Header => parse_phase = ParsePhase::Card,
                ParsePhase::Card => {
                    cards.push_back(line.parse().unwrap());
                },
            }
        }
    }
    data.push(cards);
    data
}

fn non_empty_deck_index(decks: &[VecDeque<usize>]) -> usize {
    decks
        .iter()
        .enumerate()
        .find_map(|(i, deck)| {
            if deck.is_empty() {
                None
            } else {
                Some(i)
            }
        })
        .unwrap()
}

fn decide_hand_winner_no_sub_game(hand: &[usize]) -> usize {
    let mut hand = hand.iter().copied().enumerate();
    let leader_and_card = hand.next().unwrap();
    hand.fold(leader_and_card, |leader_and_card, (player, card)| {
        if card > leader_and_card.1 {
            (player, card)
        } else {
            leader_and_card
        }
    })
    .0
}

fn solve_already_parsed<T: AsRef<[VecDeque<usize>]>>(
    input: &T
) -> Option<usize> {
    let mut decks = input.as_ref().to_vec();
    while decks.iter().all(|deck| !deck.is_empty()) {
        let hand = decks
            .iter_mut()
            .map(|deck| deck.pop_front().unwrap())
            .collect::<Vec<_>>();
        let hand_winner = decide_hand_winner_no_sub_game(&hand);
        decks[hand_winner].push_back(hand[hand_winner]);
        decks[hand_winner].push_back(hand[1 - hand_winner]);
    }
    let game_winner = non_empty_deck_index(&decks);
    Some(
        decks[game_winner]
            .iter()
            .enumerate()
            .map(|(i, card)| card * (decks[game_winner].len() - i))
            .sum(),
    )
}

const ITERATIONS: usize = 10000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
