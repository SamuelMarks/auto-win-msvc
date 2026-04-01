#ifndef LINUX_EXECINFO_H
#define LINUX_EXECINFO_H

#if defined(_MSC_VER)
/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** \brief backtrace function. */
int backtrace(void **buffer, int size);
char **backtrace_symbols(void *const *buffer, int size);
/** \brief backtrace_symbols_fd function. */
void backtrace_symbols_fd(void *const *buffer, int size, int fd);

#ifdef __cplusplus
}
#endif

#endif /* _MSC_VER */

#endif /* LINUX_EXECINFO_H */
