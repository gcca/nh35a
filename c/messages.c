#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "mem.h"
#include "messages.h"

void
nh_print (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vprintf (fmt, args);
  va_end (args);
}

void
nh_fail (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vprintf (fmt, args);
  va_end (args);
  abort ();
}

NHSStream *
nh_sstream_new (nhsize cap)
{
  NHSStream *s = nh_malloc (sizeof (NHSStream));
  s->buf = nh_malloc (cap);
  s->ptr = s->buf;
  s->len = 0;
  return s;
}

void
nh_sstream_free (NHSStream *s)
{
  free (s->buf);
  free (s);
}

void
nh_sstream_char (NHSStream *s, nhchar c)
{
  *(nhchar *)s->ptr++ = c;
}

void
nh_sstream_charptr (NHSStream *s, const nhchar *c)
{
  nhsize len = strlen (c);
  strncpy (s->ptr, c, len);
  s->ptr += len;
}

void
nh_sstream_int (NHSStream *s, nhint z)
{
  nhchar *p = s->ptr;
  if (z == 0) { *p++ = '0'; }
  else
    {
      if (z < 0)
        {
          *p++ = '-';
          z = -z;
        }

      do {
          *p++ = '0' + (z % 10);
          z /= 10;
        }
      while (z);
      for (nhchar *q = s->ptr, *r = p - 1; q < r; q++, r--)
        {
          char c = *q;
          *q = *r;
          *r = c;
        }
    }
  s->ptr = p;
}

void
nh_sstream_uint (NHSStream *s, nhuint z)
{
  nhchar *p = s->ptr;
  if (z == 0) { *p++ = '0'; }
  else
    {
      do {
          *p++ = '0' + (z % 10);
          z /= 10;
        }
      while (z);
      for (nhchar *q = s->ptr, *r = p - 1; q < r; q++, r--)
        {
          char c = *q;
          *q = *r;
          *r = c;
        }
    }
  s->ptr = p;
}

void
nh_sstream_double (NHSStream *s, double r)
{
  nhchar *p = s->ptr;
  if (r < 0)
    {
      *p++ = '-';
      r = -r;
    }
  nhint z = r;
  s->ptr = p;
  nh_sstream_uint (s, z);
  p = s->ptr;
  *p++ = '.';
  r -= z;
  for (nhint i = 0; i < 3; i++)
    {
      r *= 10;
      z = r;
      *p++ = '0' + z;
      r -= z;
    }
  s->ptr = p;
}

void
nh_sstream_cout (NHSStream *s)
{
  write (STDOUT_FILENO, s->buf, (nhchar *)s->ptr - s->buf);
  s->ptr = s->buf;
}

void
nh_sstream_cout_ln (NHSStream *s)
{
  *(nhchar *)s->ptr++ = '\n';
  write (STDOUT_FILENO, s->buf, (nhchar *)s->ptr - s->buf);
  s->ptr = s->buf;
}
