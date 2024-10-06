const std = @import("std");

fn isMultipleOf3(n: usize) bool {
    return n % 3 == 0;
}

fn isMultipleOf5(n: usize) bool {
    return n % 5 == 0;
}

fn sumMulitplesOf3Or5Below(n: usize) usize {
    var sum: usize = 0;

    for (1..n) |i| {
        if (isMultipleOf3(i) or isMultipleOf5(i)) {
            sum += i;
        }
    }

    return sum;
}

fn minmaxK(n: usize, k: usize) usize {
    return (n - 1) / k;
}

fn sumFirstN(n: usize) usize {
    return n * (n + 1) / 2;
}

fn partialSum(n: usize, k: usize) usize {
    return k * sumFirstN(minmaxK(n, k));
}

fn sumMulitplesOf3Or5BelowP(n: usize) usize {
    return partialSum(n, 3) + partialSum(n, 5) - partialSum(n, 15);
}

fn main() void {}

test "below 10" {
    try std.testing.expectEqual(23, sumMulitplesOf3Or5Below(10));
    try std.testing.expectEqual(23, sumMulitplesOf3Or5BelowP(10));
}

test "below 1000" {
    try std.testing.expectEqual(233_168, sumMulitplesOf3Or5Below(1000));
    try std.testing.expectEqual(233_168, sumMulitplesOf3Or5BelowP(1000));
}

test "below 100000" {
    try std.testing.expectEqual(2_333_316_668, sumMulitplesOf3Or5Below(100_000));
    try std.testing.expectEqual(2_333_316_668, sumMulitplesOf3Or5BelowP(100_000));
}
