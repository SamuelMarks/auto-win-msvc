#ifndef BSD_MALLOC_NP_H
#define BSD_MALLOC_NP_H

/**
 * @file bsd-malloc-np.h
 * @brief Polyfill for BSD/jemalloc malloc_np.h.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
/** @brief Generic error type code. */
typedef int error_type_t;
/** @brief Success indicator for error_type_t. */
#define ERR_NONE 0
#endif

/**
 * @brief Error codes returned by bsd-malloc-np functions.
 */
enum bsd_malloc_np_error_code {
  /** @brief Successful operation. */
  BSD_MALLOC_NP_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  BSD_MALLOC_NP_ERROR_NULL_POINTER = 1,
  /** @brief I/O or formatting error occurred. */
  BSD_MALLOC_NP_ERROR_IO = 2
};

/**
 * @brief Validates or initializes the bsd-malloc-np module.
 * @param[out] out_status Pointer to an integer that receives the initialized
 * status.
 * @return BSD_MALLOC_NP_SUCCESS on success, or an error code on failure.
 */
enum bsd_malloc_np_error_code bsd_malloc_np_init(int *out_status);

/**
 * @brief jemalloc stats print polyfill function.
 * @param write_cb Callback function to write output strings.
 * @param cbopaque Opaque context pointer passed to callback.
 * @param opts Formatting options string.
 * @return ERR_NONE on success, or a negative/non-zero error code.
 */
error_type_t je_malloc_stats_print(void (*write_cb)(void *, const char *),
                                   void *cbopaque, const char *opts);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_MALLOC_NP_H */
