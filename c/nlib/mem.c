#include "mem.h"
#include "btree.h"

#ifdef NH_DEBUG_MEM
#  include <stdio.h>
#  include <stdlib.h>

NHDbgMem dbg_mem = { 0, 0, 0 };

typedef struct
{
  nhsize size;
} Info;

nhptr
nh_dbg_malloc (nhsize size)
{
  nhsize dbg_size = size + sizeof (Info);
  nhptr ptr = malloc (dbg_size);

  if (ptr)
    {
      Info *info = (Info *)ptr;
      info->size = size;

      dbg_mem.allocs_count++;
      dbg_mem.allocated_mem += size;
    }

  return ptr + sizeof (Info);
}

void
nh_dbg_free (nhptr ptr)
{
  Info *info = (Info *)(ptr - sizeof (Info));

  dbg_mem.allocs_count--;
  dbg_mem.freed_mem += info->size;

  free (info);
}

void
nh_dbg_mem_reset ()
{
  dbg_mem.allocs_count = 0;
  dbg_mem.allocated_mem = 0;
  dbg_mem.freed_mem = 0;
}

const NHDbgMem *
nh_dbg_mem_get ()
{
  return &dbg_mem;
}
#endif

typedef struct NH_FreeAtom_ NH_FreeAtom;
typedef struct NH_SChunk_ NH_SChunk;
typedef struct NH_Chunk_ NH_Chunk;
typedef struct NH_Area_ NH_Area;

struct NH_FreeAtom_
{
  NH_FreeAtom *next;
};

struct NH_SChunk_
{
  nhuint atom_size;
  nhsize area_size;
  nhuint allocated;
  nhsize free;
  nhptr area;
  NH_FreeAtom *free_atoms;
};

#if SIZEOF_VOID_P > SIZEOF_LONG
#  define ALIGN SIZEOF_VOID_P
#else
#  define ALIGN SIZEOF_LONG
#endif

#define AREA_SIZE 4L
#define AREA_MAXSIZE 65536L

struct NH_Area_
{
  NH_Area *next;
  nhuint index;
  nhsize free;
  nhuint allocated;
  nhbool mark;
  nhchar mem[AREA_SIZE];
};

struct NH_Chunk_
{
  nhuint areas_len;
  nhuint atom_size;
  nhsize area_size;
  NH_Area *area;
  NH_Area *areas;
  nhuint marks_len;
  NH_FreeAtom *free_atoms;
  NHBTree *tree;
};

static nhint
chunk_area_cmp (const NH_Area *a, const NH_Area *b)
{
  return a->mem - b->mem;
}

static nhint
chunk_area_search (const NH_Area *area, const nhchar *addr)
{
  if (area->mem <= addr)
    {
      if (addr < &area->mem[area->index]) return 0;
      return 1;
    }
  return -1;
}

NHChunk *
nh_chunk_new (nhuint atom_size, nhsize area_size)
{
  NH_Chunk *chunk = nh_new (NH_Chunk);

  chunk->atom_size
      = atom_size % ALIGN ? atom_size + ALIGN - atom_size % ALIGN : atom_size;
  chunk->area_size = area_size;
  chunk->areas_len = 0;
  chunk->area = nullptr;
  chunk->areas = nullptr;
  chunk->marks_len = 0;
  chunk->free_atoms = nullptr;
  chunk->tree = nh_btree_new ((NHBTreeCmp)chunk_area_cmp);

  return (NHChunk *)chunk;
}

void
nh_chunk_free (NHChunk *chunk_)
{
  NH_Chunk *chunk = (NH_Chunk *)chunk_;

  nh_btree_free (chunk->tree);
  while (chunk->areas)
    {
      NH_Area *area = chunk->areas;
      chunk->areas = area->next;
      nh_free (area);
    }
  nh_free (chunk);
}

nhptr
nh_chunk_alloc (NHChunk *chunk_)
{
  NH_Chunk *chunk = (NH_Chunk *)chunk_;

  if (chunk->free_atoms)
    {
      NH_FreeAtom *atom = chunk->free_atoms;
      chunk->free_atoms = atom->next;
      return atom;
    }

  if (!chunk->area
      || (chunk->area->index + chunk->atom_size) > chunk->area_size)
    {
      chunk->area
          = nh_malloc (sizeof (NH_Area) - AREA_SIZE + chunk->area_size);
      chunk->areas_len++;
      chunk->area->next = chunk->areas;
      chunk->areas = chunk->area;
      nh_btree_insert (chunk->tree, chunk->area, chunk->area);

      chunk->area->index = 0;
      chunk->area->free = chunk->area_size;
      chunk->area->allocated = 0;
      chunk->area->mark = false;
    }

  nhptr ptr = &chunk->area->mem[chunk->area->index];
  chunk->area->index += chunk->atom_size;
  chunk->area->free -= chunk->atom_size;
  chunk->area->allocated++;

  return ptr;
}

void
nh_chunk_dealloc (NHChunk *chunk_, nhptr ptr)
{
  NH_Chunk *chunk = (NH_Chunk *)chunk_;

  NH_FreeAtom *atom = (NH_FreeAtom *)ptr;
  atom->next = chunk->free_atoms;
  chunk->free_atoms = atom;

  NH_Area *area = nh_btree_search_data (chunk->tree,
                                        (NHBTreeSearch)chunk_area_search, ptr);
  if (!--area->allocated)
    {
      area->mark = true;
      chunk->marks_len++;
    }
}

void
nh_chunk_gc (NHChunk *chunk_)
{
  NH_Chunk *chunk = (NH_Chunk *)chunk_;

  NH_Area *area = chunk->areas;
  while (area)
    {
      NH_Area *tmp = area->next;
      area = area->next;
      if (tmp->mark)
        {
          nh_btree_remove (chunk->tree, tmp);
          nh_free (tmp);
          chunk->areas_len--;
          chunk->marks_len--;
        }
    }
}

NHSChunk *
nh_schunk_new (nhuint atom_size, nhsize area_size)
{
  NH_SChunk *chunk = nh_new (NH_SChunk);

  chunk->atom_size = atom_size;
  chunk->area_size = area_size;
  chunk->allocated = 0;
  chunk->free = area_size;
  chunk->area = nh_malloc (area_size);
  chunk->free_atoms = nullptr;

  return (NHSChunk *)chunk;
}

void
nh_schunk_free (NHSChunk *chunk)
{
  NH_SChunk *schunk = (NH_SChunk *)chunk;

  nh_free (schunk->area);
  nh_free (schunk);
}

nhptr
nh_schunk_alloc (NHSChunk *chunk_)
{
  NH_SChunk *chunk = (NH_SChunk *)chunk_;

  if (chunk->free_atoms)
    {
      NH_FreeAtom *atom = chunk->free_atoms;
      chunk->free_atoms = atom->next;
      return atom;
    }

  if (chunk->free < chunk->atom_size) return nullptr;

  chunk->free -= chunk->atom_size;

  return chunk->area + chunk->allocated++ * chunk->atom_size;
}

void
nh_schunk_dealloc (NHSChunk *chunk_, nhptr ptr)
{
  NH_SChunk *chunk = (NH_SChunk *)chunk_;
  NH_FreeAtom *atom = (NH_FreeAtom *)ptr;
  atom->next = chunk->free_atoms;
  chunk->free_atoms = atom;
}

nhbool
nh_schunk_full (NHSChunk *chunk_)
{
  NH_SChunk *chunk = (NH_SChunk *)chunk_;
  return chunk->free < chunk->atom_size;
}

nhbool
nh_schunk_allocable (NHSChunk *chunk_)
{
  NH_SChunk *chunk = (NH_SChunk *)chunk_;
  return chunk->free_atoms || chunk->free >= chunk->atom_size;
}
