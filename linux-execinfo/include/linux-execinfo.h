#ifndef LINUX_EXECINFO_H
#define LINUX_EXECINFO_H
#include "auto_win_msvc_export.h"

#if defined(_MSC_VER)
/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** \brief backtrace function. */
AUTO_WIN_MSVC_EXPORT int backtrace(void **buffer, int size);
AUTO_WIN_MSVC_EXPORT char **backtrace_symbols(void *const *buffer, int size);
/** \brief backtrace_symbols_fd function. */
AUTO_WIN_MSVC_EXPORT void backtrace_symbols_fd(void *const *buffer, int size,
                                               int fd);

#endif /* _MSC_VER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_EXECINFO_H */
