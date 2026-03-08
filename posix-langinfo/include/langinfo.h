/* posix-langinfo/include/langinfo.h - Strict C89 Implementation */
#ifndef POSIX_LANGINFO_H
#define POSIX_LANGINFO_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define POSIX_LANGINFO_MSVC 1
#endif

#if defined(POSIX_LANGINFO_MSVC) || defined(_WIN32)

typedef int nl_item;

#define CODESET 1

char *posix_langinfo(nl_item item);

#ifndef nl_langinfo
#define nl_langinfo posix_langinfo
#endif

#else
#include_next <langinfo.h>
#endif

#ifdef __cplusplus
}
#endif

#endif /* POSIX_LANGINFO_H */
