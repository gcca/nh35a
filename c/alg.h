#include "types.h"

typedef nhint (*NHAlgInsertionSortCmp) (const nhptr a, const nhptr b);

void nh_alg_insertion_sort (nhptr A, nhuint s, nhuint n,
                            NHAlgInsertionSortCmp cmp);
