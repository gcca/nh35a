#include <stdio.h>
#include <stdlib.h>

struct heapq_t
{
  size_t *heap;
  size_t *last;
  size_t size;
  size_t capacity;
};

struct heapq_t *heapq_create (size_t capacity);
void heapq_release (struct heapq_t *heapq);

void heapq_push (struct heapq_t *heapq, size_t value);
size_t heapq_pop (struct heapq_t *heapq);

void heapq_print (struct heapq_t *heapq);
void heapq_traverse_ld (struct heapq_t *heapq);
