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
    for &num1 in &nums {
        let remainder = 2020 - num1;
        for &num2 in &nums {
            if num2 == num1 || num2 > remainder {
                continue;
            }
            let num3 = 2020 - num1 - num2;
            if nums.contains(&num3) {
                println!("{}", num1 * num2 * num3);
                return;
            }
        }
    }
}
