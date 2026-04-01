#ifndef POSIX_STRINGS_H
#define POSIX_STRINGS_H

#if defined(_MSC_VER)

/* clang-format off */
#include <string.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define strcasecmp _stricmp
#define strncasecmp _strnicmp

/** \brief bzero macro. */
#define bzero(b, len) (memset((b), '\0', (len)), (void)0)
/** \brief bcopy macro. */
#define bcopy(src, dest, len) (memmove((dest), (src), (len)), (void)0)
/** \brief bcmp macro. */
#define bcmp(b1, b2, len) memcmp((b1), (b2), (len))

/** \brief ffs function. */
int ffs(int i);
/** \brief ffsl function. */
int ffsl(long i);
/** \brief ffsll function. */
int ffsll(long long i);

#ifdef __cplusplus
}
#endif

#endif /* _MSC_VER */

#endif /* POSIX_STRINGS_H */
