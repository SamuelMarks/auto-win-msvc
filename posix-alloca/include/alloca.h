/* alloca.h - Strict C89 Implementation */
#ifndef POSIX_ALLOCA_H
#define POSIX_ALLOCA_H

#if defined(_MSC_VER)
# include <malloc.h>
# ifndef alloca
#  define alloca _alloca
# endif
#else
# include <stdlib.h>
#endif

#endif /* POSIX_ALLOCA_H */
