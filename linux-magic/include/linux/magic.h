#ifndef LINUX_MAGIC_H
#define LINUX_MAGIC_H

/**
 * @file magic.h
 * @brief Polyfill for Linux <linux/magic.h>.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TMPFS_MAGIC
/** @brief Magic number for tmpfs filesystem. */
#define TMPFS_MAGIC 0x01021994UL
#endif

/**
 * @brief Error codes returned by linux-magic functions.
 */
enum linux_magic_error_code {
  /** @brief Successful operation. */
  LINUX_MAGIC_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  LINUX_MAGIC_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves the TMPFS magic number.
 * @param[out] out_magic Pointer to an unsigned long that receives the TMPFS
 * magic number.
 * @return LINUX_MAGIC_SUCCESS on success, or an error code on failure.
 */
enum linux_magic_error_code
linux_magic_get_tmpfs_magic(unsigned long *out_magic);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_MAGIC_H */
