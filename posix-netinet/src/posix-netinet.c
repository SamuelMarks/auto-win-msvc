#include "posix-netinet-in.h"
#include "posix-netinet-tcp.h"

#if defined(_MSC_VER) || defined(_WIN32)

/** \brief posix_netinet_dummy_link function. */
int posix_netinet_dummy_link(void) { return 0; }

#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_netinet(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_netinet;
