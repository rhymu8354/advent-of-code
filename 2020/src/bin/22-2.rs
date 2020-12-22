use std::collections::HashSet;

use aoc_2020::benchmarked_main;

enum ParsePhase {
    Header,
    Card,
}

fn parse(input: &str) -> Vec<Vec<usize>> {
    let mut data = vec![];
    let mut cards = vec![];
    let mut parse_phase = ParsePhase::Header;
    for line in input.lines().map(|line| line.trim()) {
        if line.is_empty() {
            data.push(cards);
            cards = vec![];
            parse_phase = ParsePhase::Header;
        } else {
            match parse_phase {
                ParsePhase::Header => parse_phase = ParsePhase::Card,
                ParsePhase::Card => {
                    cards.push(line.parse().unwrap());
                },
            }
        }
    }
    data.push(cards);
    data
}

fn should_sub_game_be_played(
    hand: &[usize],
    decks: &[Vec<usize>],
) -> bool {
    hand.iter().zip(decks).all(|(card, deck)| deck.len() >= *card)
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

fn decide_hand_winner_with_sub_game(
    hand: &[usize],
    decks: &[Vec<usize>],
) -> usize {
    let sub_decks = decks
        .iter()
        .zip(hand)
        // Rules say sub game decks have as many cards as top card values.
        .map(|(deck, card)| deck[0..*card].to_vec())
        .collect::<Vec<_>>();
    play_game(&sub_decks).0
}

fn non_empty_deck_index(decks: &[Vec<usize>]) -> usize {
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

fn detect_repeated_decks(
    previous_rounds: &mut HashSet<Vec<Vec<usize>>>,
    decks: &[Vec<usize>],
) -> Option<(usize, Vec<usize>)> {
    if previous_rounds.contains(decks) {
        // Explicit rule says player 1 wins if a deck configuration is
        // repeated.
        Some((0, decks[0].clone()))
    } else {
        previous_rounds.insert(decks.to_vec());
        None
    }
}

fn play_game<T: AsRef<[Vec<usize>]>>(input: &T) -> (usize, Vec<usize>) {
    let mut decks = input.as_ref().to_vec();
    let mut previous_rounds = HashSet::new();
    while decks.iter().all(|deck| !deck.is_empty()) {
        if let Some(game_result) =
            detect_repeated_decks(&mut previous_rounds, &decks)
        {
            return game_result;
        }
        let hand =
            decks.iter_mut().map(|deck| deck.remove(0)).collect::<Vec<_>>();
        let hand_winner = if should_sub_game_be_played(&hand, &decks) {
            decide_hand_winner_with_sub_game(&hand, &decks)
        } else {
            decide_hand_winner_no_sub_game(&hand)
        };
        decks[hand_winner].push(hand[hand_winner]);
        decks[hand_winner].push(hand[1 - hand_winner]);
    }
    let game_winner = non_empty_deck_index(&decks);
    (game_winner, decks[game_winner].clone())
}

fn solve_already_parsed<T: AsRef<[Vec<usize>]>>(input: &T) -> Option<usize> {
    let deck = play_game(input).1;
    Some(deck.iter().enumerate().map(|(i, card)| card * (deck.len() - i)).sum())
}

const ITERATIONS: usize = 1;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
