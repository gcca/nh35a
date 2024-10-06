#include "nh35a.h"

typedef void (*NHTestFunc) ();

struct NHTest
{
  NHTestFunc func;
  char *name;
};

#define NH_TEST_MAXF 100
extern struct NHTest nhtests[NH_TEST_MAXF];
extern nhint nhtests_count;

#define NH_TEST(tname)                                                        \
  void test_##tname ();                                                       \
  __attribute__ ((constructor)) void nh_test_register_##tname ()              \
  {                                                                           \
    if (nhtests_count < NH_TEST_MAXF)                                         \
      nhtests[nhtests_count++]                                                \
          = (struct NHTest){ .func = test_##tname, .name = #tname };          \
  }                                                                           \
  void test_##tname ()
