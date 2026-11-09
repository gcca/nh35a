import CryptoKit

func hashS(_ s: String, _ ns: UInt64) -> UInt64 {
  precondition(ns > 0, "zero ring size")

  let d = Array(Insecure.MD5.hash(data: Array(s.utf8)))
  let dl = d[0..<8].reduce(UInt64(0)) { ($0 << 8) | UInt64($1) }
  let dh = d[8..<16].reduce(UInt64(0)) { ($0 << 8) | UInt64($1) }

  return (dh ^ dl) % ns
}

struct Nodes {
  let ns: UInt64
  var nodes: [(name: String, hash: UInt64)] = []

  init(_ ns: UInt64) {
    precondition(ns > 0, "zero ring size")
    self.ns = ns
  }

  mutating func add(_ s: String) {
    nodes.append((s, hashS(s, ns)))
  }

  mutating func done() {
    nodes.sort { $0.hash < $1.hash }
  }

  func bs(_ s: String) -> (String, UInt64) {
    precondition(!nodes.isEmpty, "no elements")
    let sh = hashS(s, ns)
    var lo = 0
    var hi = nodes.count

    while lo < hi {
      let mid = lo + (hi - lo) / 2
      if nodes[mid].hash < sh {
        lo = mid + 1
      } else {
        hi = mid
      }
    }

    let i = lo == nodes.count ? 0 : lo
    return (nodes[i].name, sh)
  }
}

#if TEST
func testHashS() {
  let values: [(String, UInt64)] = [
    ("A", 14321),
    ("B", 8630),
    ("C", 35588),
    ("1", 21135),
    ("10", 41300),
    ("100", 19522),
  ]

  for (s, sh) in values {
    assert(hashS(s, 65535) == sh, "unexpected hash for \(s)")
  }
}

func testDone() {
  var ns = Nodes(65535)
  ns.done()
  assert(ns.nodes.isEmpty)

  for name in ["A", "B", "C"] { ns.add(name) }
  ns.done()

  assert(ns.nodes.map { $0.name } == ["B", "A", "C"])
  assert(ns.nodes.map { $0.hash } == [8630, 14321, 35588])
}

func testBS() {
  var ns = Nodes(65535)
  for name in ["A", "B", "C"] { ns.add(name) }
  ns.done()

  let values: [(String, String, UInt64)] = [
    ("1", "C", 21135),
    ("10", "B", 41300),
    ("100", "C", 19522),
    ("", "B", 5970),
    ("A", "A", 14321),
    ("B", "B", 8630),
    ("C", "C", 35588),
  ]

  for (s, name, sh) in values {
    assert(ns.bs(s) == (name, sh), "unexpected lookup for \(s)")
  }
}

func testUnsignedRing() {
  var ns = Nodes(UInt64.max)
  for name in ["A", "B", "C"] { ns.add(name) }
  ns.done()

  assert(ns.nodes.map { $0.name } == ["C", "A", "B"])
  assert(hashS("B", ns.ns) == 18185419767222700475)
  for s in ["1", "10", "100"] {
    assert(ns.bs(s).0 == "B", "unexpected lookup for \(s)")
  }
  for name in ["A", "B", "C"] {
    assert(ns.bs(name).0 == name)
  }
}

func testSingleNodeAndCollisions() {
  var single = Nodes(65535)
  single.add("A")
  single.done()

  for s in ["", "A", "1", "10", "100"] {
    assert(single.bs(s).0 == "A", "unexpected lookup for \(s)")
  }

  var collisions = Nodes(1)
  for name in ["A", "B", "C"] { collisions.add(name) }
  collisions.done()

  assert(collisions.bs("10") == ("A", 0))
}

testHashS()
testDone()
testBS()
testUnsignedRing()
testSingleNodeAndCollisions()
print("All tests passed")
#else
var ns = Nodes(65535)
for name in ["A", "B", "C"] { ns.add(name) }
ns.done()

for (name, hash) in ns.nodes {
  print("\(name): \(hash)")
}
print("ns: \(ns.ns)\n")

for s in ["1", "10", "100"] {
  print(ns.bs(s))
}
#endif
