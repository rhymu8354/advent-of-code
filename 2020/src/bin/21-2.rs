use aoc_2020::benchmarked_main;
use std::collections::{
    hash_map::Entry,
    HashMap,
    HashSet,
};

#[derive(Debug)]
struct Food {
    ingredients: HashSet<String>,
    allergens: HashSet<String>,
}

fn parse(input: &str) -> Vec<Food> {
    input
        .lines()
        .map(str::trim)
        .map(|line| {
            let mut parts = line[..line.len() - 1].splitn(2, " (contains ");
            Food {
                ingredients: parts
                    .next()
                    .unwrap()
                    .split(' ')
                    .map(String::from)
                    .collect::<HashSet<_>>(),
                allergens: parts
                    .next()
                    .unwrap()
                    .split(", ")
                    .map(String::from)
                    .collect::<HashSet<_>>(),
            }
        })
        .collect::<Vec<_>>()
}

fn solve_already_parsed<T: AsRef<[Food]>>(input: &T) -> Option<String> {
    let input = input.as_ref();
    let mut allergen_ingredients = HashMap::<String, HashSet<String>>::new();
    let mut allergens = HashMap::<String, String>::new();
    let mut ingredients = HashMap::<String, usize>::new();
    for food in input {
        for ingredient in &food.ingredients {
            *ingredients.entry(ingredient.clone()).or_insert(0) += 1;
        }
        for allergen in &food.allergens {
            match allergen_ingredients.entry(allergen.clone()) {
                Entry::Occupied(mut entry) => {
                    let entry = entry.get_mut();
                    *entry = entry
                        .intersection(&food.ingredients)
                        .cloned()
                        .collect();
                    if entry.len() == 1 {
                        allergens.insert(
                            entry.iter().next().unwrap().clone(),
                            allergen.clone(),
                        );
                    }
                },
                Entry::Vacant(entry) => {
                    entry.insert(food.ingredients.clone());
                },
            }
        }
    }
    loop {
        let mut found_allergens = false;
        for (allergen, ingredients) in allergen_ingredients.iter_mut() {
            if ingredients.len() == 1 {
                continue;
            }
            *ingredients = ingredients
                .iter()
                .filter(|ingredient| !allergens.contains_key(*ingredient))
                .cloned()
                .collect();
            if ingredients.len() == 1 {
                found_allergens = true;
                let ingredient = ingredients.iter().next().unwrap().clone();
                allergens.insert(ingredient, allergen.clone());
            }
        }
        if !found_allergens {
            break;
        }
    }
    let mut dangerous = allergens.keys().cloned().collect::<Vec<_>>();
    dangerous.sort_unstable_by(|a, b| {
        allergens.get(a).unwrap().cmp(allergens.get(b).unwrap())
    });
    Some(dangerous.join(","))
}

const ITERATIONS: usize = 1000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
