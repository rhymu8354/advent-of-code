use std::{
    collections::HashSet,
    io::{
        stdin,
        Read,
    },
};

fn main() {
    let mut input = String::new();
    stdin().read_to_string(&mut input).unwrap();
    let nums = input
        .lines()
        .map(|line| line.trim().parse().unwrap())
        .collect::<HashSet<u32>>();
    for num in &nums {
        let complement = 2020 - num;
        if nums.contains(&complement) {
            println!("{}", num * complement);
            return;
        }
    }
}
