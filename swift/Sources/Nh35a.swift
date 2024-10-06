public func InsertionSort(_ A: inout [Int]) {
  for j in 1..<A.count {
    let k = A[j]
    var i = j - 1
    while i >= 0, A[i] > k {
      A[i + 1] = A[i]
      i -= 1
    }
    A[i + 1] = k
  }
}
