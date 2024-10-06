#include <math.h>
#include <string.h>

#include "heapq.h"

struct heapq_t *
heapq_create (size_t capacity)
{
  struct heapq_t *heapq = malloc (sizeof (struct heapq_t));
  heapq->heap = malloc (sizeof (size_t) * capacity);
  heapq->last = heapq->heap - 1;
  heapq->size = 0;
  heapq->capacity = capacity;
  return heapq;
}

void
heapq_release (struct heapq_t *heapq)
{
  free (heapq->heap);
  free (heapq);
}

void
_heapq_siftdown (struct heapq_t *heapq, size_t startpos, size_t pos)
{
  size_t newitem = heapq->heap[pos];
  while (pos > startpos)
    {
      size_t parentpos = (pos - 1) >> 1;
      size_t parent = heapq->heap[parentpos];
      if (newitem < parent)
        {
          heapq->heap[pos] = parent;
          pos = parentpos;
          continue;
        }
      break;
    }
  heapq->heap[pos] = newitem;
}

void
_heapq_siftup (struct heapq_t *heapq, size_t pos)
{
  size_t endpos = heapq->size;
  size_t startpos = pos;
  size_t newitem = heapq->heap[pos];
  size_t lpos = 2 * pos + 1;

  while (lpos < endpos)
    {
      size_t rpos = lpos + 1;
      if (rpos < endpos && !(heapq->heap[lpos] < heapq->heap[rpos]))
        {
          lpos = rpos;
        }
      heapq->heap[pos] = heapq->heap[lpos];
      pos = lpos;
      lpos = 2 * pos + 1;
    }

  heapq->heap[pos] = newitem;
  _heapq_siftdown (heapq, startpos, pos);
}

void
heapq_push (struct heapq_t *heapq, size_t value)
{
  *++heapq->last = value;
  _heapq_siftdown (heapq, 0, heapq->size++);
}

size_t
heapq_pop (struct heapq_t *heapq)
{
  size_t last = *heapq->last--;
  heapq->size--;

  if (heapq->heap < heapq->last)
    {
      size_t returnitem = *heapq->heap;
      *heapq->heap = last;
      _heapq_siftup (heapq, 0);
      return returnitem;
    }

  return last;
}

void
heapq_print (struct heapq_t *heapq)
{
  size_t line = 1, count = 0;

  size_t level = 4 * (size_t)log2 (heapq->size);
  char indent[level];
  memset (indent, ' ', level);
  indent[level - 1] = '\0';

  for (size_t i = 0; i < heapq->size; i++)
    {
      if (i == line)
        {
          line = (1 << (++count + 1)) - 1;
          printf ("\n");
          level -= 4;
          indent[level] = '\0';
        }
      printf ("%s", indent);
      printf ("%4zu", heapq->heap[i]);
    }
}

void
_heapq_traverse_ld (struct heapq_t *heapq, size_t pos)
{
  if (pos >= heapq->size)
    return;
  size_t cpos = 2 * pos + 1;
  _heapq_traverse_ld (heapq, cpos);
  _heapq_traverse_ld (heapq, cpos + 1);
  printf ("%zu  ", heapq->heap[pos]);
}

void
heapq_traverse_ld (struct heapq_t *heapq)
{
  _heapq_traverse_ld (heapq, 0);
}
