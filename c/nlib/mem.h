#include "types.h"

#ifdef NH_DEBUG_MEM
typedef struct
{
  nhuint allocs_count;
  nhsize allocated_mem;
  nhsize freed_mem;
} NHDbgMem;
[[nodiscard]] nhptr nh_dbg_malloc (nhsize size);
void nh_dbg_free (nhptr ptr);
void nh_dbg_mem_reset ();
const NHDbgMem *nh_dbg_mem_get ();
#  define nh_new(type) ((type *)nh_dbg_malloc (sizeof (type)))
#  define nh_malloc(size) nh_dbg_malloc (size)
#  define nh_free(ptr) nh_dbg_free (ptr)
#else
#  include <stdlib.h>
#  define nh_new(type) ((type *)malloc (sizeof (type)))
#  define nh_malloc(size) malloc (size)
#  define nh_free(ptr) free (ptr)
#endif

typedef struct NHSChunk_ NHSChunk;
typedef struct NHChunk_ NHChunk;

struct NHSChunk_
{
};

struct NHChunk_
{
};

[[nodiscard]] NHSChunk *nh_schunk_new (nhuint atom_size, nhsize area_size);
void nh_schunk_free (NHSChunk *chunk);
[[nodiscard]] nhptr nh_schunk_alloc (NHSChunk *chunk);
void nh_schunk_dealloc (NHSChunk *chunk, nhptr ptr);
[[nodiscard]] nhbool nh_schunk_full (NHSChunk *chunk);
[[nodiscard]] nhbool nh_schunk_allocable (NHSChunk *chunk);

[[nodiscard]] NHChunk *nh_chunk_new (nhuint atom_size, nhsize area_size);
void nh_chunk_free (NHChunk *chunk);
[[nodiscard]] nhptr nh_chunk_alloc (NHChunk *chunk);
void nh_chunk_dealloc (NHChunk *chunk, nhptr ptr);
void nh_chunk_gc (NHChunk *chunk);
