#pragma once

#if (__GNUC__ > 12 || (__GNUC__ == 12 && __GNUC_MINOR__ >= 1))                \
    || (__clang_major__ > 15) || (__STDC_VERSION__ >= 202311L)
#  define HAVE_C23
#endif

typedef char nhchar;
typedef short nhshort;
typedef long nhlong;
typedef int nhint;

typedef unsigned char nhuchar;
typedef unsigned short nhushort;
typedef unsigned long nhulong;
typedef unsigned int nhuint;

typedef float nhfloat;
typedef double nhdouble;

#ifdef HAVE_C23
typedef _Bool nhbool;
#else
typedef nhchar nhbool;
#endif

typedef void *nhptr;
typedef const void *nhcptr;

// Temporal definitions
#define SIZEOF_CHAR 1
#define SIZEOF_SHORT 2
#define SIZEOF_INT 4
#define SIZEOF_LONG 8
#define SIZEOF_LONG_LONG 8

#if (SIZEOF_CHAR == 1)
typedef signed char nhint8;
typedef unsigned char nhuint8;
#endif

#if (SIZEOF_SHORT == 2)
typedef signed short nhint16;
typedef unsigned short nhuint16;
#endif

#if (SIZEOF_INT == 4)
typedef signed int nhint32;
typedef unsigned int nhuint32;
#elif (SIZEOF_LONG == 4)
typedef signed long nhint32;
typedef unsigned long nhuint32;
#endif

#if (SIZEOF_LONG == 8)
#  define HAVE_NHINT64
typedef signed long nhint64;
typedef unsigned long nhuint64;
#elif (SIZEOF_LONG_LONG == 8)
#  define HAVE_NHINT64
typedef signed long long nhint64;
typedef unsigned long long nhuint64;
#else
#  undef HAVE_NHINT64
#endif

#if (SIZEOF_INT == SIZEOF_VOID_P)
#  define NHPTR_TO_INT(p) ((nhint)(p))
#  define NHPTR_TO_UINT(p) ((nhuint)(p))
#  define NHINT_TO_PTR(i) ((nhptr)(i))
#  define NHUINT_TO_PTR(u) ((nhptr)(u))
#elif (SIZEOF_LONG == SIZEOF_VOID_P)
#  define NHPTR_TO_INT(p) ((nhint)(nhlong)(p))
#  define NHPTR_TO_UINT(p) ((nhuint)(nhulong)(p))
#  define NHINT_TO_PTR(i) ((nhptr)(nhlong)(i))
#  define NHUINT_TO_PTR(u) ((nhptr)(nhulong)(u))
#else

#endif

#ifdef HAVE_NHINT64
typedef nhint64 nhssize;
typedef nhuint64 nhsize;
#else
typedef nhint32 nhssize;
typedef nhuint32 nhsize;
#endif

#ifdef HAVE_C23
#  ifndef NULL
#    define NULL nullptr
#  endif

#  ifndef FALSE
#    define FALSE false
#  endif

#  ifndef TRUE
#    define TRUE true
#  endif
#else
#  ifndef NULL
#    define NULL ((void *)0)
#  endif

#  ifndef FALSE
#    define FALSE (0)
#  endif

#  ifndef TRUE
#    define TRUE (!FALSE)
#  endif
#endif
