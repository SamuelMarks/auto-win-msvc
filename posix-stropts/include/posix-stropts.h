#ifndef POSIX_STROPTS_H
#define POSIX_STROPTS_H

/**
 * @file posix-stropts.h
 * @brief Polyfill for POSIX <stropts.h> STREAMS interface.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Buffer structure for STREAMS operations.
 */
struct strbuf {
  /** @brief Maximum buffer length. */
  int maxlen;
  /** @brief Length of data. */
  int len;
  /** @brief Pointer to buffer data. */
  char *buf;
};

/**
 * @brief Error codes returned by posix-stropts functions.
 */
enum posix_stropts_error_code {
  /** @brief Operation completed successfully. */
  POSIX_STROPTS_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_STROPTS_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on stropts polyfill availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_STROPTS_SUCCESS on success, or an error code on failure.
 */
enum posix_stropts_error_code posix_stropts_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_STROPTS_H */
