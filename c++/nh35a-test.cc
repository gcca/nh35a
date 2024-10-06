#include <gtest/gtest.h>
#include <vector>

#include "algorithms.hpp"

TEST(AlgorithmsTests, InsertionSort) {
  std::vector<int> A = {4, 6, 3, 1, 5, 2};
  InsertionSort(A);
  const std::vector<int> B = {1, 2, 3, 4, 5, 6};
  ASSERT_EQ(A, B);
}
