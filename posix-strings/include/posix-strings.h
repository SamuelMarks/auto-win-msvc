#ifndef POSIX_STRINGS_H
#define POSIX_STRINGS_H

#if defined(_MSC_VER)

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define strcasecmp _stricmp
#define strncasecmp _strnicmp

#define bzero(b, len) (memset((b), '\0', (len)), (void) 0)
#define bcopy(src, dest, len) (memmove((dest), (src), (len)), (void) 0)
#define bcmp(b1, b2, len) memcmp((b1), (b2), (len))

int ffs(int i);
int ffsl(long i);
int ffsll(long long i);

#ifdef __cplusplus
}
#endif

#endif /* _MSC_VER */

#endif /* POSIX_STRINGS_H */
