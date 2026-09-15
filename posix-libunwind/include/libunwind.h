#ifndef POSIX_LIBUNWIND_LIBUNWIND_H
#define POSIX_LIBUNWIND_LIBUNWIND_H

/**
 * @file libunwind.h
 * @brief Strict C89 compatibility definitions for libunwind.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Unwind word type.
 */
typedef unsigned long unw_word_t;

/**
 * @brief Opaque unwind context structure.
 */
typedef struct unw_context {
  /** @brief Context data storage */
  void *data[16];
} unw_context_t;

/**
 * @brief Opaque unwind cursor structure.
 */
typedef struct unw_cursor {
  /** @brief Cursor data storage */
  void *opaque[16];
} unw_cursor_t;

/**
 * @brief Initializes the unwind context.
 * @param ucp Pointer to the context structure to initialize.
 * @return 0 on success, or negative on failure.
 */
int unw_getcontext(unw_context_t *ucp);

/**
 * @brief Initializes a cursor for local stack unwinding.
 * @param cp Pointer to cursor structure.
 * @param ucp Pointer to context structure.
 * @return 0 on success, or negative on failure.
 */
int unw_init_local(unw_cursor_t *cp, unw_context_t *ucp);

/**
 * @brief Advances cursor to next stack frame.
 * @param cp Pointer to cursor structure.
 * @return > 0 if next frame exists, 0 if end of stack, or < 0 on error.
 */
int unw_step(unw_cursor_t *cp);

/**
 * @brief Gets register value from cursor.
 * @param cp Pointer to cursor structure.
 * @param regnum Register number.
 * @param valp Pointer to variable receiving register value.
 * @return 0 on success, or negative on failure.
 */
int unw_get_reg(unw_cursor_t *cp, int regnum, unw_word_t *valp);

/**
 * @brief Gets procedure name for the current frame.
 * @param cp Pointer to cursor structure.
 * @param bufp Pointer to buffer receiving procedure name.
 * @param len Buffer length in bytes.
 * @param offp Pointer to variable receiving offset.
 * @return 0 on success, or negative on failure.
 */
int unw_get_proc_name(unw_cursor_t *cp, char *bufp, size_t len,
                      unw_word_t *offp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_LIBUNWIND_LIBUNWIND_H */
