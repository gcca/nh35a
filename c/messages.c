#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

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
