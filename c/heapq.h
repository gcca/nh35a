#include "types.h"

typedef struct NHHeapq_ NHHeapq;

struct NHHeapq_
{
  nhuint *heap;
  nhuint *last;
  nhuint size;
  nhuint capacity;
};

NHHeapq *heapq_create (nhuint capacity);
void heapq_release (NHHeapq *heapq);

void heapq_push (NHHeapq *heapq, nhuint value);
nhuint heapq_pop (NHHeapq *heapq);

void heapq_print (NHHeapq *heapq);
void heapq_traverse_ld (NHHeapq *heapq);
