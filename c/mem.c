#include <stdlib.h>

#include "mem.h"

#include "types.h"

#ifdef NH_DEBUG_MEM
#  include <stdio.h>

NHDbgMem nh_dbg_mem = { 0 };

nhptr
nh_dbg_malloc (nhsize size)
{
  nhptr ptr = malloc (size);

  if (ptr) nh_dbg_mem.allocs_count++;

  return ptr;
}

void
nh_dbg_free (nhptr ptr)
{
  nh_dbg_mem.allocs_count--;
  free (ptr);
}

const NHDbgMem *
nh_dbg_mem_get ()
{
  return &nh_dbg_mem;
}
#endif

typedef struct NHActualChunk_ NHActualChunk;
typedef struct NHFreeAtom_ NHFreeAtom;
typedef struct NHArea_ NHArea;
typedef struct NHActualAllocator_ NHActualAllocator;

struct NHArea_
{
  NHArea *next;
};

struct NHActualChunk_
{
  nhuint atom_size;
  nhsize area_size;
  nhuint allocated;
  nhsize free;
  nhptr area;
};

struct NHFreeAtom_
{
  NHFreeAtom *next;
};

struct NHActualAllocator_
{
  NHActualChunk *chunk;
  NHFreeAtom *free_atom;
};

NHChunk *
nh_chunk_new (nhuint atom_size, nhsize area_size)
{
  NHActualChunk *chunk = nh_new (NHActualChunk);
  chunk->atom_size = atom_size;
  chunk->area_size = area_size;
  chunk->allocated = 0;
  chunk->free = area_size;
  chunk->area = nh_malloc (area_size);
  return (NHChunk *)chunk;
}

void
nh_chunk_free (NHChunk *chunk_)
{
  NHActualChunk *chunk = (NHActualChunk *)chunk_;
  nh_free (chunk->area);
  nh_free (chunk);
}

nhptr
nh_chunk_alloc (NHChunk *chunk_)
{
  NHActualChunk *chunk = (NHActualChunk *)chunk_;

  if (chunk->free == 0) return NULL;

  chunk->free -= chunk->atom_size;
  chunk->allocated++;
  return chunk->area + chunk->allocated * chunk->atom_size;
}

NHAllocator *
nh_allocator_new (NHChunk *chunk)
{
  NHActualAllocator *allocator = nh_new (NHActualAllocator);
  allocator->chunk = (NHActualChunk *)chunk;
  allocator->free_atom = NULL;
  return (NHAllocator *)allocator;
}

void
nh_allocator_free (NHAllocator *allocator)
{
  NHActualAllocator *allocator_ = (NHActualAllocator *)allocator;
  nh_chunk_free ((NHChunk *)allocator_->chunk);
  nh_free (allocator_);
}

nhptr
nh_allocator_alloc (NHAllocator *allocator)
{
  NHActualAllocator *allocator_ = (NHActualAllocator *)allocator;
  while (allocator_->free_atom)
    {
      NHFreeAtom *atom = allocator_->free_atom;
      allocator_->free_atom = atom->next;
      return atom;
    }
  return nh_chunk_alloc ((NHChunk *)allocator_->chunk);
}

void
nh_allocator_dealloc (NHAllocator *allocator, nhptr ptr)
{
  NHActualAllocator *allocator_ = (NHActualAllocator *)allocator;
  NHFreeAtom *free_atom = ptr;
  free_atom->next = allocator_->free_atom;
  allocator_->free_atom = free_atom;
}
