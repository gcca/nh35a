#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mem.h"
#include "nh35a_test.h"

#define NH_TEST_MAXF 100
struct NHTest nhtests[NH_TEST_MAXF];
nhint nhtests_count = 0;

static void
print (const struct NHTest *nhtest)
{
  printf ("\033[32m[>>> BEGIN]\033[0m %s\n", nhtest->name);

  struct timespec start;
  clock_gettime (CLOCK_MONOTONIC, &start);
  nhtest->func ();

  struct timespec end;
  clock_gettime (CLOCK_MONOTONIC, &end);
  long seconds = end.tv_sec - start.tv_sec;
  long ns = end.tv_nsec - start.tv_nsec;
  if (start.tv_nsec > end.tv_nsec)
    {
      seconds -= 1;
      ns += 1000000000;
    }

  double elapsed_ms = seconds * 1000.0 + ns / 1000000.0;

#ifdef NH_DEBUG_MEM
  const NHDbgMem *dbgmem = nh_dbg_mem_get ();
  if (dbgmem->allocs_count)
    printf (
        "\033[33m[<<<   END]\033[0m %s (%.3f ms) \033[31m(allocs-count=%d)\n",
        nhtest->name, elapsed_ms, dbgmem->allocs_count);
  else
    printf (
        "\033[33m[<<<   END]\033[0m %s (%.3f ms) \033[34m(allocs-count=%d)\n",
        nhtest->name, elapsed_ms, dbgmem->allocs_count);
#else
  printf ("\033[33m[<<<   END]\033[0m %s (%.3f ms)\n", nhtest->name,
          elapsed_ms);
#endif
}

int
main (int argc, char *argv[])
{
  if (argc > 1)
    {
      for (nhint i = 0; i < nhtests_count; i++)
        {
          if (!strcmp (nhtests[i].name, argv[1]))
            {
              print (&nhtests[i]);
              return EXIT_SUCCESS;
            }
        }
      printf ("\033[31mTest not found: %s\033[0m\n", argv[1]);
      return EXIT_SUCCESS;
    }

  nhint i = 0;
  for (; i < nhtests_count - 1; i++) { print (&nhtests[i]); }
  print (&nhtests[i]);

  return EXIT_SUCCESS;
}
