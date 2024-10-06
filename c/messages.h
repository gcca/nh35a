#include <stddef.h>

#include "types.h"

#define nh_sstream(s, x)                                                      \
  _Generic ((x),                                                              \
      nhchar: nh_sstream_char,                                                \
      nhchar *: nh_sstream_charptr,                                           \
      const nhchar *: nh_sstream_charptr,                                     \
      nhint: nh_sstream_int,                                                  \
      nhuint: nh_sstream_uint,                                                \
      double: nh_sstream_double) ((s), (x))

typedef struct NHSStream NHSStream;

struct NHSStream
{
  nhchar *buf;
  nhptr ptr;
  nhsize len;
};

[[nodiscard]] NHSStream *nh_sstream_new (nhsize cap);
void nh_sstream_free (NHSStream *s);
void nh_sstream_char (NHSStream *s, nhchar c);
void nh_sstream_charptr (NHSStream *s, const nhchar *c);
void nh_sstream_int (NHSStream *s, nhint z);
void nh_sstream_uint (NHSStream *s, nhuint z);
void nh_sstream_double (NHSStream *s, double r);
void nh_sstream_cout (NHSStream *s);
void nh_sstream_cout_ln (NHSStream *s);

void nh_print (const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
void nh_fail (const char *fmt, ...) __attribute__ ((format (printf, 1, 2)))
__attribute__ ((noreturn));
