#include "mem.h"
#include "nh35a_test.h"

typedef struct
{
  nhchar space[64];
} TestFoo;

NH_TEST (dbg_mem)
{
  const NHDbgMem *dbgmen = nh_dbg_mem_get ();

  nhptr ptr = nh_new (TestFoo);

  nh_assert (ptr != NULL);
  nh_assert (dbgmen->allocs_count == 1);
  nh_assert (dbgmen->allocated_mem == 64);
  nh_assert (dbgmen->freed_mem == 0);

  nh_free (ptr);

  nh_assert (dbgmen->allocs_count == 0);
  nh_assert (dbgmen->allocated_mem == 64);
  nh_assert (dbgmen->freed_mem == 64);
}

NH_TEST (schunk_new_and_free)
{
  const NHDbgMem *dbgmen = nh_dbg_mem_get ();
  nh_assert (dbgmen->allocs_count == 0);
  nh_dbg_mem_reset ();

  NHSChunk *chunk = nh_schunk_new (64, 192);

  nh_assert (chunk != NULL);
  nh_assert (dbgmen->allocs_count == 2);
  nh_assert (dbgmen->allocated_mem > 192);
  nh_assert (dbgmen->freed_mem == 0);

  nhsize allocated_mem = dbgmen->allocated_mem;

  nh_schunk_free (chunk);

  nh_assert (dbgmen->allocs_count == 0);
  nh_assert (dbgmen->allocated_mem == allocated_mem);
  nh_assert (dbgmen->freed_mem == allocated_mem);
}

NH_TEST (schunk_full_alloc)
{
  const NHDbgMem *dbgmen = nh_dbg_mem_get ();
  nh_assert (dbgmen->allocs_count == 0);
  nh_dbg_mem_reset ();

  NHSChunk *chunk = nh_schunk_new (64, 192);

  nhptr ptr1 = nh_schunk_alloc (chunk);

  nh_assert (ptr1 != NULL);
  nh_assert (!nh_schunk_full (chunk));

  nhptr ptr2 = nh_schunk_alloc (chunk);

  nh_assert (ptr2 != NULL);
  nh_assert (!nh_schunk_full (chunk));
  nh_assert (ptr2 == ptr1 + 64);

  nhptr ptr3 = nh_schunk_alloc (chunk);

  nh_assert (ptr3 != NULL);
  nh_assert (nh_schunk_full (chunk));
  nh_assert (ptr3 == ptr2 + 64);

  nh_assert (dbgmen->allocs_count == 2);
  nh_assert (dbgmen->allocated_mem > 192);
  nh_assert (dbgmen->freed_mem == 0);

  nhsize allocated_mem = dbgmen->allocated_mem;

  nh_schunk_free (chunk);

  nh_assert (dbgmen->allocs_count == 0);
  nh_assert (dbgmen->allocated_mem == allocated_mem);
  nh_assert (dbgmen->freed_mem == allocated_mem);
}

NH_TEST (schunk_deallocs)
{
  const NHDbgMem *dbgmen = nh_dbg_mem_get ();
  nh_assert (dbgmen->allocs_count == 0);
  nh_dbg_mem_reset ();

  NHSChunk *chunk = nh_schunk_new (64, 192);

  nhptr ptr1 = nh_schunk_alloc (chunk);
  nhptr ptr2 = nh_schunk_alloc (chunk);
  nhptr ptr3 = nh_schunk_alloc (chunk);

  nh_assert (!nh_schunk_allocable (chunk));

  nh_schunk_dealloc (chunk, ptr1);
  nh_assert (nh_schunk_allocable (chunk));

  nh_schunk_dealloc (chunk, ptr2);
  nh_assert (nh_schunk_allocable (chunk));

  nhptr ptr4 = nh_schunk_alloc (chunk);
  nh_assert (nh_schunk_allocable (chunk));

  nhptr ptr5 = nh_schunk_alloc (chunk);
  nh_assert (!nh_schunk_allocable (chunk));

  nh_assert (ptr2 == ptr4);
  nh_assert (ptr1 == ptr5);

  nh_schunk_free (chunk);
}
