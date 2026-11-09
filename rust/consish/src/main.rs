use std::fmt;

use md5::{Digest, Md5};

fn hash_s(s: &str, ns: u64) -> u64 {
    let d = Md5::digest(s.as_bytes());

    let dl = u64::from_be_bytes(d[0..8].try_into().unwrap());
    let dh = u64::from_be_bytes(d[8..16].try_into().unwrap());

    (dh ^ dl) % ns
}

struct Nodes {
    ds: Vec<String>,
    ss: Vec<u64>,
    ns: u64,
}

impl Nodes {
    fn add(&mut self, n: &str) -> &mut Self {
        let s = hash_s(n, self.ns);
        self.ds.push(n.to_string());
        self.ss.push(s);
        self
    }

    fn done(&mut self) -> () {
        let mut idx: Vec<usize> = (0..self.ss.len()).collect();
        idx.sort_by_key(|&i| self.ss[i]);

        self.ds = idx
            .iter()
            .map(|&i| std::mem::take(&mut self.ds[i]))
            .collect();
        self.ss = idx.iter().map(|&i| self.ss[i]).collect();
    }

    fn bs(&self, s: &str) -> (&str, u64) {
        assert!(!self.ds.is_empty(), "no elements");

        let sh = hash_s(s, self.ns);

        let mut i = self.ss.partition_point(|&h| h < sh);

        if i == self.ss.len() {
            i = 0;
        }

        (self.ds[i].as_str(), sh)
    }

    fn new(ns: u64) -> Self {
        Nodes {
            ds: Vec::new(),
            ss: Vec::new(),
            ns: ns,
        }
    }
}

impl std::fmt::Display for Nodes {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        for (n, s) in self.ds.iter().zip(self.ss.iter()) {
            writeln!(f, "{}: {}", n, s)?;
        }
        writeln!(f, "ns: {}", self.ns)?;
        Ok(())
    }
}

fn main() {
    let mut ns = Nodes::new(65535);

    ns.add("A").add("B").add("C").done();

    println!("{}", ns);
    println!("{:?}", ns.bs("1"));
    println!("{:?}", ns.bs("10"));
    println!("{:?}", ns.bs("100"));
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn hash_s_matches_main_values() {
        let vs = [
            ("A", 14321),
            ("B", 8630),
            ("C", 35588),
            ("1", 21135),
            ("10", 41300),
            ("100", 19522),
        ];

        for (s, sh) in vs {
            assert_eq!(hash_s(s, 65535), sh, "input: {s}");
        }
    }

    #[test]
    fn done_sorts_hashes_and_keeps_names_aligned() {
        let mut ns = Nodes::new(65535);
        ns.add("A").add("B").add("C").done();

        assert_eq!(ns.ss, [8630, 14321, 35588]);
        assert_eq!(ns.ds, ["B", "A", "C"]);
    }

    #[test]
    fn bs_matches_main_lookups_including_wraparound() {
        let mut ns = Nodes::new(65535);
        ns.add("A").add("B").add("C").done();

        let vs = [("1", "C", 21135), ("10", "B", 41300), ("100", "C", 19522)];

        for (s, n, sh) in vs {
            assert_eq!(ns.bs(s), (n, sh), "key: {s}");
        }
    }
}
