#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nh35a_test.h"

#define NH_TEST_MAXF 100
struct NHTest nhtests[NH_TEST_MAXF];
nhint nhtests_count = 0;

int
main (int argc, char *argv[])
{
  if (argc > 1)
    {
      for (nhint i = 0; i < nhtests_count; i++)
        {
          if (!strcmp (nhtests[i].name, argv[1]))
            {
              printf ("\033[32m>>> BEGIN %s\033[0m\n", nhtests[i].name);
              nhtests[i].func ();
              printf ("\033[33m<<< END %s\033[0m\n", nhtests[i].name);
              return EXIT_SUCCESS;
            }
        }
      printf ("\033[31mTest not found: %s\033[0m\n", argv[1]);
      return EXIT_SUCCESS;
    }

  nhint i = 0;
  for (; i < nhtests_count - 1; i++)
    {
      printf ("\033[32m>>> BEGIN %s\033[0m\n", nhtests[i].name);
      nhtests[i].func ();
      printf ("\033[33m<<< END %s\033[0m\n\n", nhtests[i].name);
    }
  printf ("\033[32m>>> BEGIN %s\033[0m\n", nhtests[i].name);
  nhtests[i].func ();
  printf ("\033[33m<<< END %s\033[0m\n", nhtests[i].name);

  return EXIT_SUCCESS;
}
