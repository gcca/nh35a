#pragma once

#include <cstddef>
#include <vector>

static void InsertionSort(std::vector<int> &A) {
  for (std::size_t j = 1; j < A.size(); j++) {
    const int k = A[j];
    std::size_t i = j - 1;
    while ((i >= 0) and (A[i] > k)) {
      A[i-- + 1] = A[i];
    }
    A[i + 1] = k;
  }
}
