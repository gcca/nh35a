void nh_print (const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
void nh_fail (const char *fmt, ...) __attribute__ ((format (printf, 1, 2)))
__attribute__ ((noreturn));
