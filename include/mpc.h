#include <mpfr.h>
#define MPC_VERSION_MAJOR 1
#define MPC_VERSION_MINOR 3
#define MPC_VERSION_PATCHLEVEL 1
#define MPC_VERSION_STRING "1.3.1"
#define MPC_VERSION_NUM(a,b,c) (((a) << 16L) | ((b) << 8) | (c))
#define MPC_VERSION                                                     \
  MPC_VERSION_NUM(MPC_VERSION_MAJOR,MPC_VERSION_MINOR,MPC_VERSION_PATCHLEVEL)

typedef int mpc_t;
#define MPC_RNDNN 0
#define mpc_init2(a,b)
#define mpc_set_ui_ui(a,b,c,d)
#define mpc_cosh(a,b,c)
#define mpc_pow(a,b,c,d)
#define mpc_acosh(a,b,c)
#define mpc_clear(a)
