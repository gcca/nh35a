#include <stdio.h>

#include "nh35a_test.h"

NH_TEST (heapq)
{
  NHHeapq *heapq = heapq_create (50);

  for (nhsize i = 30; i > 0; i--) { heapq_push (heapq, i); }
  heapq_push (heapq, 0);

  printf ("print:\n");
  heapq_print (heapq);

  printf ("\n\ntraverse ld:\n");
  heapq_traverse_ld (heapq);

  printf ("\n\npop all:\n");
  for (nhsize i = 0; i < 30; i++) { printf ("%u ", heapq_pop (heapq)); }
  printf ("\n");

  heapq_release (heapq);
}
