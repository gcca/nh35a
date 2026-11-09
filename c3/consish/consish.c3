module consish;

import std::io;
import std::sort;
import std::hash::md5;
import std::core::bitorder;
import std::core::mem::alloc;
import std::collections::list, std::collections::pair;

fn ulong hash_s(String s, ulong ns)
{
	assert(ns > 0, "zero ring size");

	char[16] d = md5::hash((char[])s);

	ulong dl = bitorder::read(d[:8], ULongBE);
	ulong dh = bitorder::read(d[8:8], ULongBE);

	return (dh ^ dl) % ns;
}

struct Nodes
{
	Allocator alloc;
	List{String} ds;
	List{ulong} ss;
	ulong ns;
}

fn void Nodes.init(&self, Allocator alloc, ulong ns, sz capacity)
{
	assert(ns > 0, "zero ring size");
	self.alloc = alloc;
	self.ns = ns;
	self.ds.init(alloc, capacity);
	self.ss.init(alloc, capacity);
}

fn void Nodes.deinit(&self)
{
	foreach (&name : self.ds) name.free(self.alloc);
	self.ds.free();
	self.ss.free();
}

fn void Nodes.add(&self, String s)
{
	ulong sh = hash_s(s, self.ns);
	self.ds.push(s.copy(self.alloc));
	self.ss.push(sh);
}

fn int compare_hashes(Pair{String, ulong} a, Pair{String, ulong} b)
{
	return a.second < b.second ? -1 : (a.second > b.second ? 1 : 0);
}

fn void Nodes.done(&self)
{
	if (self.ss.len() < 2) return;
	Pair{String, ulong}[] entries = alloc::new_array(self.alloc, Pair{String, ulong}, self.ss.len());
	defer alloc::free(self.alloc, entries.ptr);

	foreach (i, name : self.ds) entries[i] = { name, self.ss[i] };
	sort::quicksort(entries, &compare_hashes);

	foreach (i, entry : entries)
	{
		self.ds[i] = entry.first;
		self.ss[i] = entry.second;
	}
}

fn Pair{String, ulong} Nodes.bs(&self, String s)
{
	assert(self.ss.len() > 0, "no elements");
	ulong sh = hash_s(s, self.ns);

	sz i = sort::binarysearch(self.ss.array_view(), sh, fn int (ulong value, ulong key) => value < key ? -1 : 1);

	if (i == self.ss.len()) i = 0;

	return { self.ds[i], sh };
}

fn int main(String[] args)
{
	Nodes ns;
	ns.init(mem, 65535, 3);
	defer ns.deinit();

	ns.add("A");
	ns.add("B");
	ns.add("C");
	ns.done();

	foreach (i, name : ns.ds) io::printf("%s: %d ", name, ns.ss[i]);
	io::printfn(" - %d", ns.ns);

	String[3] keys = { "1", "10", "100" };
	foreach (key : keys)
	{
		Pair{String, ulong} result = ns.bs(key);
		io::printfn("(%s, %d)", result.first, result.second);
	}
	return 0;
}

fn void test_hash_s() @test
{
	String[6] keys  = { "A", "B", "C", "1", "10", "100" };
	ulong[6] hashes = { 14321, 8630, 35588, 21135, 41300, 19522 };
	foreach (i, key : keys) assert(hash_s(key, 65535) == hashes[i]);
}

fn void test_done() @test
{
	Nodes ns;
	ns.init(mem, 65535, 3);
	defer ns.deinit();
	ns.add("A");
	ns.add("B");
	ns.add("C");
	ns.done();

	String[3] names = { "B", "A", "C" };
	ulong[3] hashes = { 8630, 14321, 35588 };
	assert(ns.ds.len() == names.len && ns.ss.len() == hashes.len);
	foreach (i, name : names)
	{
		assert(ns.ds[i] == name);
		assert(ns.ss[i] == hashes[i]);
	}
}

fn void test_bs() @test
{
	Nodes ns;
	ns.init(mem, 65535, 3);
	defer ns.deinit();
	ns.add("A");
	ns.add("B");
	ns.add("C");
	ns.done();

	String[6] keys  = { "1", "10", "100", "A", "B", "C" };
	String[6] names = { "C", "B", "C", "A", "B", "C" };
	ulong[6] hashes = { 21135, 41300, 19522, 14321, 8630, 35588 };
	foreach (i, key : keys)
	{
		Pair{String, ulong} result = ns.bs(key);
		assert(result.first == names[i]);
		assert(result.second == hashes[i]);
	}
}

fn void test_done_empty_and_single() @test
{
	Nodes ns;
	ns.init(mem, 65535, 0);
	defer ns.deinit();
	ns.done();
	assert(ns.ds.len() == 0 && ns.ss.len() == 0);

	ns.add("A");
	ns.done();
	assert(ns.ds.len() == 1 && ns.ss.len() == 1);
	assert(ns.ds[0] == "A" && ns.ss[0] == 14321);
	assert(ns.bs("100").first == "A");
}

fn void test_bs_collisions() @test
{
	Nodes ns;
	ns.init(mem, 1, 3);
	defer ns.deinit();
	ns.add("A");
	ns.add("B");
	ns.add("C");
	ns.done();

	Pair{String, ulong} result = ns.bs("anything");
	assert(result.first == ns.ds[0]);
	assert(result.second == 0);
}
