const std = @import("std");

fn hashS(s: []const u8, ns: u64) u64 {
    var d: [16]u8 = undefined;
    std.crypto.hash.Md5.hash(s, &d, .{});

    const dl = std.mem.readInt(u64, d[0..8], .big);
    const dh = std.mem.readInt(u64, d[8..16], .big);

    return (dh ^ dl) % ns;
}

const Nodes = struct {
    alloc: std.mem.Allocator,
    ds: std.ArrayList([]const u8) = .empty,
    ss: std.ArrayList(u64) = .empty,
    ns: u64,

    fn init(alloc: std.mem.Allocator, ns: u64, c: usize) !Nodes {
        var ds = try std.ArrayList([]const u8).initCapacity(alloc, c);
        errdefer ds.deinit(alloc);

        const ss = try std.ArrayList(u64).initCapacity(alloc, c);

        return .{ .alloc = alloc, .ds = ds, .ss = ss, .ns = ns };
    }

    fn deinit(self: *Nodes) void {
        for (self.ds.items) |d| self.alloc.free(d);
        self.ds.deinit(self.alloc);
        self.ss.deinit(self.alloc);
    }

    fn add(self: *Nodes, s: []const u8) !void {
        const d = try self.alloc.dupe(u8, s);
        errdefer self.alloc.free(d);

        try self.ds.appendBounded(d);
        errdefer self.ds.items.len -= 1;

        try self.ss.appendBounded(hashS(s, self.ns));
    }

    pub fn lessThan(self: *Nodes, i: usize, j: usize) bool {
        return self.ss.items[i] < self.ss.items[j];
    }

    pub fn swap(self: *Nodes, i: usize, j: usize) void {
        std.mem.swap([]const u8, &self.ds.items[i], &self.ds.items[j]);
        std.mem.swap(u64, &self.ss.items[i], &self.ss.items[j]);
    }

    fn done(self: *Nodes) void {
        std.sort.heapContext(0, self.ss.items.len, self);
    }

    fn cmpHash(k: u64, v: u64) std.math.Order {
        return std.math.order(k, v);
    }

    fn bs(self: *const Nodes, s: []const u8) struct { []const u8, u64 } {
        if (self.ss.items.len == 0) @panic("no elements");

        const sh = hashS(s, self.ns);
        var i = std.sort.lowerBound(u64, self.ss.items, sh, cmpHash);

        if (i == self.ss.items.len) {
            i = 0;
        }

        return .{ self.ds.items[i], sh };
    }
};

pub fn main() !void {
    var ns = try Nodes.init(std.heap.page_allocator, 65535, 3);
    defer ns.deinit();

    try ns.add("A");
    try ns.add("B");
    try ns.add("C");
    ns.done();

    for (ns.ds.items, ns.ss.items) |n, s|
        std.debug.print("{s}: {d} ", .{ n, s });
    std.debug.print(" - {d}\n", .{ns.ns});

    for ([_][]const u8{ "1", "10", "100" }) |s|
        std.debug.print("({s}, {d})\n", ns.bs(s));
}

test "hashS matches the values used in main" {
    const vs = [_]struct { s: []const u8, sh: u64 }{
        .{ .s = "A", .sh = 14321 },
        .{ .s = "B", .sh = 8630 },
        .{ .s = "C", .sh = 35588 },
        .{ .s = "1", .sh = 21135 },
        .{ .s = "10", .sh = 41300 },
        .{ .s = "100", .sh = 19522 },
    };

    for (vs) |v| {
        try std.testing.expectEqual(v.sh, hashS(v.s, 65535));
    }
}

test "done sorts hashes and keeps node names aligned" {
    var ns = try Nodes.init(std.testing.allocator, 65535, 3);
    defer ns.deinit();

    try ns.add("A");
    try ns.add("B");
    try ns.add("C");
    ns.done();

    try std.testing.expectEqualSlices(u64, &.{ 8630, 14321, 35588 }, ns.ss.items);

    const ds = [_][]const u8{ "B", "A", "C" };
    try std.testing.expectEqual(ds.len, ns.ds.items.len);
    for (ds, ns.ds.items) |d, n| {
        try std.testing.expectEqualStrings(d, n);
    }
}

test "bs matches main lookups including wraparound" {
    var ns = try Nodes.init(std.testing.allocator, 65535, 3);
    defer ns.deinit();

    try ns.add("A");
    try ns.add("B");
    try ns.add("C");
    ns.done();

    const vs = [_]struct { s: []const u8, n: []const u8, sh: u64 }{
        .{ .s = "1", .n = "C", .sh = 21135 },
        .{ .s = "10", .n = "B", .sh = 41300 },
        .{ .s = "100", .n = "C", .sh = 19522 },
    };

    for (vs) |v| {
        const n, const sh = ns.bs(v.s);
        try std.testing.expectEqualStrings(v.n, n);
        try std.testing.expectEqual(v.sh, sh);
    }
}
