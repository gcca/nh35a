package algorithms

import (
  "reflect"
  "testing"
)

func TestInsertionSort(t *testing.T) {
  A := []int{4, 6, 2, 1, 5, 3}
  InsertionSort(A)

  if !reflect.DeepEqual(A, []int{1, 2, 3, 4, 5, 6}) {
    t.Errorf("%v", A)
  }
}
