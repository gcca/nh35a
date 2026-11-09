package main

import (
	"crypto/md5"
	"encoding/binary"
	"fmt"
	"slices"
	"sort"
)

func hashS(s string, ns uint64) uint64 {
	d := md5.Sum([]byte(s))

	dl := binary.BigEndian.Uint64(d[:8])
	dh := binary.BigEndian.Uint64(d[8:16])

	return (dh ^ dl) % ns
}

type Nodes struct {
	ds []string
	ss []uint64
	ns uint64
}

func (n *Nodes) add(s string) *Nodes {
	n.ds = append(n.ds, s)
	n.ss = append(n.ss, hashS(s, n.ns))
	return n
}

func (n *Nodes) Len() int { return len(n.ds) }

func (n *Nodes) Less(i, j int) bool { return n.ss[i] < n.ss[j] }

func (n *Nodes) Swap(i, j int) {
	n.ds[i], n.ds[j] = n.ds[j], n.ds[i]
	n.ss[i], n.ss[j] = n.ss[j], n.ss[i]
}

func (n *Nodes) done() {
	sort.Sort(n)
}

func (n *Nodes) bs(s string) (string, uint64) {
	if n.Len() < 1 {
		panic("no elements")
	}

	sh := hashS(s, n.ns)
	si, _ := slices.BinarySearch(n.ss, sh)

	if si == n.Len() {
		si = 0
	}

	return n.ds[si], sh
}

func newDNodes(ns uint64) *Nodes {
	return &Nodes{ns: ns}
}

func main() {
	dns := newDNodes(65535)

	dns.add("A").add("B").add("C").done()

	fmt.Println(dns)
	fmt.Println(dns.bs("1"))
	fmt.Println(dns.bs("10"))
	fmt.Println(dns.bs("100"))
}
