#include <string.h>

#include "alg.h"

void
nh_alg_insertion_sort (nhptr A, nhuint s, nhuint n, NHAlgInsertionSortCmp cmp)
{
  nhchar k[s];
  for (nhuint j = 1; j < n; j++)
    {
      memcpy (k, A + s * j, s);
      nhuint i = j - 1;
      while ((i >= 0) && cmp (A + s * i, k) > 0)
        {
          memcpy (A + s * (i + 1), A + s * i, s);
          --i;
        }
      memcpy (A + s * (i + 1), k, s);
    }
}
