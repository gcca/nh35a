#include <stdio.h>

#include "nh35a_test.h"

NH_TEST (heapq)
{
  NHHeapq *heapq = nh_heapq_create (50);

  for (nhsize i = 30; i > 0; i--) { nh_heapq_push (heapq, i); }
  nh_heapq_push (heapq, 0);

  printf ("print:\n");
  nh_heapq_print (heapq);

  printf ("\n\ntraverse ld:\n");
  nh_heapq_traverse_ld (heapq);

  printf ("\n\npop all:\n");
  for (nhsize i = 0; i < 30; i++) { printf ("%u ", nh_heapq_pop (heapq)); }
  printf ("\n");

  nh_heapq_release (heapq);
}
