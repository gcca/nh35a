fn is_multiple_of_3(n: u32) -> bool {
    n % 3 == 0
}

fn is_multiple_of_5(n: u32) -> bool {
    n % 5 == 0
}

fn sum_multiples_of_3_or_5_below(n: u32) -> u32 {
    let mut multiples = Vec::new();

    for i in 1..n {
        if is_multiple_of_3(i) || is_multiple_of_5(i) {
            multiples.push(i);
        }
    }

    multiples.iter().sum()
}

fn minmax_k(n: u32, k: u32) -> u32 {
    (n - 1) / k
}

fn sum_first_n(n: u32) -> u32 {
    n * (n + 1) / 2
}

fn partial_sum(n: u32, k: u32) -> u32 {
    k * sum_first_n(minmax_k(n, k))
}

fn sum_multiples_of_3_or_5_below_p(n: u32) -> u32 {
    partial_sum(n, 3) + partial_sum(n, 5) - partial_sum(n, 15)
}

fn main() {}

#[test]
fn below_10() {
    assert_eq!(23, sum_multiples_of_3_or_5_below(10));
    assert_eq!(23, sum_multiples_of_3_or_5_below_p(10));
}

#[test]
fn below_1000() {
    assert_eq!(233_168, sum_multiples_of_3_or_5_below(1000));
    assert_eq!(233_168, sum_multiples_of_3_or_5_below_p(1000));
}

#[test]
fn below_100000() {
    // assert_eq!(1, sum_multiples_of_3_or_5_below(1_000_000));
    assert_eq!(2_333_316_668, sum_multiples_of_3_or_5_below_p(100_000));
}
