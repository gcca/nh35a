#include "nh35a_test.h"

nhchar stream_buf[256];
NHSStream stream = { stream_buf, stream_buf, 0 };

//:{{{ insertion_sort
nhint
test_alg_insertion_sort_cmp (const nhint *a, const nhint *b)
{
  return *a - *b;
}

NH_TEST (insertion_sort)
{
  const nhuint len = 40;
  nhint A[len];

  {
    nhint v = len;
    for (nhuint i = 0; i < len; i++) A[i] = v--;
  }

  nh_alg_insertion_sort (A, sizeof (nhint), len,
                         (NHAlgInsertionSortCmp)test_alg_insertion_sort_cmp);

  nh_sstream (&stream, "insertion_sort:");
  for (nhuint i = 0; i < len; i++)
    {
      nh_sstream (&stream, (char)' ');
      nh_sstream (&stream, A[i]);
    }
  nh_sstream_cout_ln (&stream);

  for (nhuint i = 0; i < len; i++) nh_assert (A[i] == i + 1);
}
//:}}} insertion_sort
