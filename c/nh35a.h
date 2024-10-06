#include "heapq.h"
#include "slist.h"
#include "types.h"

#if defined(__GNUC__) && !defined(__cplusplus)
#  define NH_BEGIN (void)({
#  define NH_END                                                              \
    })
#else
#  if (defined(sun) || defined(__sun__))
#    define NH_BEGIN                                                          \
      if (0)                                                                  \
        {
#    define NH_END                                                            \
      }                                                                       \
      else (void) 0
#  else
#    define NH_BEGIN                                                          \
      do {
#    define NH_END                                                            \
      }                                                                       \
      while (0)
#  endif
#endif

#ifdef __GNUC__
#  define nh_assert(expr)                                                     \
    NH_BEGIN                                                                  \
    if (!(expr))                                                              \
      printf ("%s:%d [%s]: \"%s\"\n", __FILE__, __LINE__,                     \
              __PRETTY_FUNCTION__, #expr);                                    \
    NH_END
#else
#  define nh_assert(expr)                                                     \
    NH_BEGIN                                                                  \
    if (!(expr))                                                              \
      printf ("%s:%d: \"%s\"\n", __FILE__, __LINE__, #expr);                  \
    NH_END
#endif

#define NH_TEST(name) void test_##name ()
#define NH_TEST_CALL(name) test_##name ()
