#ifndef LINUX_EXECINFO_H
#define LINUX_EXECINFO_H

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
typedef int error_type_t;
#define ERR_NONE 0
#endif

#if defined(_MSC_VER)
/* clang-format off */
#include <stddef.h>
/* clang-format on */
#endif /* _MSC_VER */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)

/** \brief backtrace function. */
error_type_t backtrace(void **buffer, int size, int *captured);
char **backtrace_symbols(void *const *buffer, int size);
/** \brief backtrace_symbols_fd function. */
error_type_t backtrace_symbols_fd(void *const *buffer, int size, int fd);

#endif /* _MSC_VER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_EXECINFO_H */
