import XCTest

@testable import Algorithm

final class AlgorithmTests: XCTestCase {
  func testInsertionSort() {
    var A = [6, 4, 3, 2, 5, 1]
    InsertionSort(&A)
    XCTAssertEqual(A, [1, 2, 3, 4, 5, 6])
  }
}
