/**
 * @file posix-libunwind.c
 * @brief Implementation of libunwind compatibility functions.
 */

/* clang-format off */
#include "posix-libunwind.h"
#include <string.h>
/* clang-format on */

/**
 * @brief Retrieves information on posix-libunwind availability.
 */
enum posix_libunwind_error_code posix_libunwind_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_LIBUNWIND_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_LIBUNWIND_SUCCESS;
}

/**
 * @brief Initializes the unwind context.
 */
int unw_getcontext(unw_context_t *ucp) {
  if (ucp == NULL) {
    return -1;
  }
  memset(ucp, 0, sizeof(*ucp));
  return 0;
}

/**
 * @brief Initializes a cursor for local stack unwinding.
 */
int unw_init_local(unw_cursor_t *cp, unw_context_t *ucp) {
  if (cp == NULL || ucp == NULL) {
    return -1;
  }
  memset(cp, 0, sizeof(*cp));
  return 0;
}

/**
 * @brief Advances cursor to next stack frame.
 */
int unw_step(unw_cursor_t *cp) {
  if (cp == NULL) {
    return -1;
  }
  return 0;
}

/**
 * @brief Gets register value from cursor.
 */
int unw_get_reg(unw_cursor_t *cp, int regnum, unw_word_t *valp) {
  if (cp == NULL || valp == NULL) {
    return -1;
  }
  (void)regnum;
  *valp = 0;
  return 0;
}

/**
 * @brief Gets procedure name for the current frame.
 */
int unw_get_proc_name(unw_cursor_t *cp, char *bufp, size_t len,
                      unw_word_t *offp) {
  if (cp == NULL || bufp == NULL || len == 0) {
    return -1;
  }
  if (offp != NULL) {
    *offp = 0;
  }
#if defined(_MSC_VER)
  strcpy_s(bufp, len, "unknown");
#else
  strncpy(bufp, "unknown", len - 1);
  bufp[len - 1] = '\0';
#endif
  return 0;
}

typedef int make_iso_compilers_happy_tu_posix_libunwind;
