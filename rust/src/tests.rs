mod algorithm;

#[cfg(test)]
mod tests {
    use crate::algorithm::*;

    #[test]
    fn test_insertion_sort() {
        let arr = &mut [6, 4, 3, 1, 5, 2];
        insertion_sort(arr);
        assert_eq!(arr, &[1, 2, 3, 4, 5, 6]);
    }
}
