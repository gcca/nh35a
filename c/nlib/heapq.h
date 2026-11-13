#include "types.h"

typedef struct NHHeapq_ NHHeapq;

struct NHHeapq_
{
  nhuint *heap;
  nhuint *last;
  nhuint size;
  nhuint capacity;
};

NHHeapq *nh_heapq_create (nhuint capacity);
void nh_heapq_release (NHHeapq *heapq);

void nh_heapq_push (NHHeapq *heapq, nhuint value);
nhuint nh_heapq_pop (NHHeapq *heapq);

void nh_heapq_print (NHHeapq *heapq);
void nh_heapq_traverse_ld (NHHeapq *heapq);
