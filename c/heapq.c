#include <math.h>
#include <stdio.h>
#include <string.h>

#include "mem.h"

#include "heapq.h"

NHHeapq *
nh_heapq_create (nhuint capacity)
{
  NHHeapq *heapq = nh_malloc (sizeof (NHHeapq));
  heapq->heap = nh_malloc (sizeof (nhuint) * capacity);
  heapq->last = heapq->heap - 1;
  heapq->size = 0;
  heapq->capacity = capacity;
  return heapq;
}

void
nh_heapq_release (NHHeapq *heapq)
{
  nh_free (heapq->heap);
  nh_free (heapq);
}

void
_heapq_siftdown (NHHeapq *heapq, nhuint startpos, nhuint pos)
{
  nhuint newitem = heapq->heap[pos];
  while (pos > startpos)
    {
      nhuint parentpos = (pos - 1) >> 1;
      nhuint parent = heapq->heap[parentpos];
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
_heapq_siftup (NHHeapq *heapq, nhuint pos)
{
  nhuint endpos = heapq->size;
  nhuint startpos = pos;
  nhuint newitem = heapq->heap[pos];
  nhuint lpos = 2 * pos + 1;

  while (lpos < endpos)
    {
      nhuint rpos = lpos + 1;
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
nh_heapq_push (NHHeapq *heapq, nhuint value)
{
  *++heapq->last = value;
  _heapq_siftdown (heapq, 0, heapq->size++);
}

nhuint
nh_heapq_pop (NHHeapq *heapq)
{
  nhuint last = *heapq->last--;
  heapq->size--;

  if (heapq->heap < heapq->last)
    {
      nhuint returnitem = *heapq->heap;
      *heapq->heap = last;
      _heapq_siftup (heapq, 0);
      return returnitem;
    }

  return last;
}

void
nh_heapq_print (NHHeapq *heapq)
{
  nhuint line = 1, count = 0;

  nhuint level = 4 * (nhuint)log2 (heapq->size);
  char indent[level];
  memset (indent, ' ', level);
  indent[level - 1] = '\0';

  for (nhuint i = 0; i < heapq->size; i++)
    {
      if (i == line)
        {
          line = (1 << (++count + 1)) - 1;
          printf ("\n");
          level -= 4;
          indent[level] = '\0';
        }
      printf ("%s", indent);
      printf ("%4u", heapq->heap[i]);
    }
}

void
_heapq_traverse_ld (NHHeapq *heapq, nhuint pos)
{
  if (pos >= heapq->size) return;
  nhuint cpos = 2 * pos + 1;
  _heapq_traverse_ld (heapq, cpos);
  _heapq_traverse_ld (heapq, cpos + 1);
  printf ("%u  ", heapq->heap[pos]);
}

void
nh_heapq_traverse_ld (NHHeapq *heapq)
{
  _heapq_traverse_ld (heapq, 0);
}
