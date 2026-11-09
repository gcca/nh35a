package main

import (
	"slices"
	"testing"
)

func TestHashSMatchesMainValues(t *testing.T) {
	vs := []struct {
		s  string
		sh uint64
	}{
		{s: "A", sh: 14321},
		{s: "B", sh: 8630},
		{s: "C", sh: 35588},
		{s: "1", sh: 21135},
		{s: "10", sh: 41300},
		{s: "100", sh: 19522},
	}

	for _, v := range vs {
		t.Run(v.s, func(t *testing.T) {
			if sh := hashS(v.s, 65535); sh != v.sh {
				t.Errorf("hashS(%q, 65535) = %d, want %d", v.s, sh, v.sh)
			}
		})
	}
}

func TestDoneSortsHashesAndKeepsNamesAligned(t *testing.T) {
	dns := newDNodes(65535)
	dns.add("A").add("B").add("C").done()

	if ss := []uint64{8630, 14321, 35588}; !slices.Equal(dns.ss, ss) {
		t.Errorf("hashes = %v, want %v", dns.ss, ss)
	}
	if ds := []string{"B", "A", "C"}; !slices.Equal(dns.ds, ds) {
		t.Errorf("names = %v, want %v", dns.ds, ds)
	}
}

func TestBSMatchesMainLookupsIncludingWraparound(t *testing.T) {
	dns := newDNodes(65535)
	dns.add("A").add("B").add("C").done()

	vs := []struct {
		s  string
		n  string
		sh uint64
	}{
		{s: "1", n: "C", sh: 21135},
		{s: "10", n: "B", sh: 41300},
		{s: "100", n: "C", sh: 19522},
	}

	for _, v := range vs {
		t.Run(v.s, func(t *testing.T) {
			n, sh := dns.bs(v.s)
			if n != v.n || sh != v.sh {
				t.Errorf("bs(%q) = (%q, %d), want (%q, %d)", v.s, n, sh, v.n, v.sh)
			}
		})
	}
}
