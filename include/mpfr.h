#define MPFR_VERSION_MAJOR 4
#define MPFR_VERSION_MINOR 2
#define MPFR_VERSION_PATCHLEVEL 1
#define MPFR_VERSION_STRING "4.2.1"
/* Macros dealing with MPFR VERSION */
#define MPFR_VERSION_NUM(a,b,c) (((a) << 16L) | ((b) << 8) | (c))
#define MPFR_VERSION \
MPFR_VERSION_NUM(MPFR_VERSION_MAJOR,MPFR_VERSION_MINOR,MPFR_VERSION_PATCHLEVEL)

typedef int mpfr_t;
#define MPFR_RNDN 0
#define mpfr_init(a)
#define mpfr_atan2(a,b,c,d)
#define mpfr_erfc(a,b,c)
#define mpfr_subnormalize(a,b,c)
#define mpfr_clear(a)
