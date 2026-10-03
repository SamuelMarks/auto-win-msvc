
#ifndef POSIX_LIBGEN_H
#define POSIX_LIBGEN_H

/**
 * @file posix-libgen.h
 * @brief POSIX libgen.h implementation for MSVC and ISO C89 environments.
 *
 * This header provides the POSIX basename() and dirname() functions,
 * implemented using safe Microsoft CRT extensions when available.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieves information on posix-libgen availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_libgen_get_info(int *out_available);

/**
 * @brief Returns the last component of a pathname.
 *
 * The basename() function takes the pathname pointed to by path and returns a
 * pointer to the final component of the pathname, deleting any trailing
 * '/' or '\\' characters.
 *
 * If the string consists entirely of the '/' or '\\' character, basename()
 * returns a pointer to the string "/" or "\\".
 * If path is a null pointer or points to an empty string, basename() returns
 * a pointer to the string ".".
 *
 * @param path The pathname to parse. This string may be modified.
 * @return A pointer to the final component of the path.
 */
char *basename(char *path);

/**
 * @brief Returns the directory name of a pathname.
 *
 * The dirname() function takes a pointer to a character string that contains
 * a pathname, and returns a pointer to a string that is a pathname of the
 * parent directory of that file. Trailing '/' or '\\' characters in the path
 * are not counted as part of the path.
 *
 * If path does not contain a '/' or '\\', then dirname() returns a pointer to
 * the string ".". If path is a null pointer or points to an empty string,
 * dirname() returns a pointer to the string ".".
 *
 * @param path The pathname to parse. This string may be modified.
 * @return A pointer to the parent directory of the path.
 */
char *dirname(char *path);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_LIBGEN_H */
