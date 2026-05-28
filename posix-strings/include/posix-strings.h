#ifndef POSIX_STRINGS_H
#define POSIX_STRINGS_H
#include "auto_win_msvc_export.h"

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
AUTO_WIN_MSVC_EXPORT int ffs(int i);
/** \brief ffsl function. */
AUTO_WIN_MSVC_EXPORT int ffsl(long i);
/** \brief ffsll function. */
AUTO_WIN_MSVC_EXPORT int ffsll(long long i);

#endif /* _MSC_VER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_STRINGS_H */
