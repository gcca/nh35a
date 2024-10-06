#pragma once

#include "types.h"

#ifdef NH_DEBUG_MEM
nhptr nh_dbg_malloc (nhsize size);
void nh_dbg_free (nhptr ptr);
void nh_dbg_meminfo_show ();
#  define nh_new(type) ((type *)nh_dbg_malloc (sizeof (type)))
#  define nh_malloc(size) nh_dbg_malloc (size)
#  define nh_free(ptr) nh_dbg_free (ptr)
#else
#  define nh_new(type) ((type *)malloc (sizeof (type)))
#  define nh_malloc(size) malloc (size)
#  define nh_free(ptr) free (ptr)
#endif

typedef struct NHChunk_ NHChunk;
typedef struct NHAllocator_ NHAllocator;

struct NHChunk_
{
};

struct NHAllocator_
{
};

NHChunk *nh_chunk_new (nhuint atom_size, nhsize area_size);
void nh_chunk_free (NHChunk *chunk);
nhptr nh_chunk_alloc (NHChunk *chunk);

NHAllocator *nh_allocator_new (NHChunk *chunk);
void nh_allocator_free (NHAllocator *allocator);
nhptr nh_allocator_alloc (NHAllocator *allocator);
void nh_allocator_dealloc (NHAllocator *allocator, nhptr ptr);
