package algorithms

func InsertionSort(A []int) {
	for i, k := range A[1:] {
		for i >= 0 && A[i] > k {
			A[i+1] = A[i]
			i--
		}
		A[i+1] = k
	}
}
