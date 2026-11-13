#include <stdlib.h>
#include <string.h>

#include "mem.h"
#include "nh35a_test.h"

#define NH_TEST_MAXF 100
struct NHTest nhtests[NH_TEST_MAXF];
nhint nhtests_count = 0;

static nhchar stream_buf[128];
static NHSStream stream = { stream_buf, stream_buf, 0 };
static nhchar timer_buf[40];
static NHTimer *timer = (NHTimer *)timer_buf;

static void
print (const struct NHTest *nhtest)
{
  nh_sstream (&stream, "\033[32m[>>> BEGIN]\033[0m ");
  nh_sstream_charptr (&stream, nhtest->name);
  nh_sstream_cout_ln (&stream);

  nh_timer_reset (timer);
  nhtest->func ();
  nhdouble elapsed_ms = nh_timer_elapsed_ms (timer);

#ifdef NH_DEBUG_MEM
  const NHDbgMem *dbgmem = nh_dbg_mem_get ();
  nh_sstream (&stream, "\033[33m[<<<   END]\033[0m ");
  nh_sstream (&stream, nhtest->name);
  nh_sstream (&stream, " (");
  nh_sstream (&stream, elapsed_ms);
  if (dbgmem->allocs_count)
    nh_sstream (&stream, " ms) \033[31m(allocs-count=");
  else
    nh_sstream (&stream, " ms) \033[34m(allocs-count=");
  nh_sstream (&stream, dbgmem->allocs_count);
  nh_sstream (&stream, ")\n");
#else
  nh_sstream (&stream, "\033[33m[<<<   END]\033[0m ");
  nh_sstream_charptr (&stream, nhtest->name);
  nh_sstream (&stream, " (");
  nh_sstream (&stream, elapsed_ms);
  nh_sstream (&stream, " ms)\n");
#endif
  nh_sstream_cout (&stream);
}

int
main (int argc, char *argv[])
{
  nh_timer_start (timer);
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
      nh_sstream (&stream, "\033[31mTest not found: ");
      nh_sstream (&stream, argv[1]);
      nh_sstream (&stream, "\033[0m\n");
      nh_sstream_cout (&stream);
    }
  else
    {
      nhint i = 0;
      for (; i < nhtests_count - 1; i++) { print (&nhtests[i]); }
      print (&nhtests[i]);
    }

  return EXIT_SUCCESS;
}
