pub fn insertion_sort(arr: &mut [i32]) {
    for j in 1..arr.len() {
        let k = arr[j];
        let mut l = 0;
        for i in (0..j).rev() {
            if arr[i] <= k {
                l = i + 1;
                break;
            }
            arr[i + 1] = arr[i];
        }
        arr[l] = k;
    }
}
