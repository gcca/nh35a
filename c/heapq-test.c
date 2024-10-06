#include "nh35a_test.h"

NH_TEST (heapq)
{
  NHHeapq *heapq = nh_heapq_create (50);

  for (nhsize i = 30; i > 0; i--) nh_heapq_push (heapq, i);
  nh_heapq_push (heapq, 0);

  nh_print ("print:\n");
  nh_heapq_print (heapq);

  nh_print ("\n\ntraverse ld:\n");
  nh_heapq_traverse_ld (heapq);

  nh_print ("\n\npop all:\n");
  for (nhsize i = 0; i < 30; i++) nh_print ("%u ", nh_heapq_pop (heapq));
  nh_print ("\n");

  nh_heapq_release (heapq);
}
