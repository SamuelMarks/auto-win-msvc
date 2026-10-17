#ifndef POSIX_LIBPROC_H
#define POSIX_LIBPROC_H

/**
 * @file posix-libproc.h
 * @brief Polyfill and process introspection functions for libproc and /proc.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Cross-platform process snapshot entry for /proc enumeration.
 */
struct posix_proc_entry {
  unsigned int pid;  /**< Process ID */
  unsigned int ppid; /**< Parent process ID */
  unsigned int pgid; /**< Process group ID */
  unsigned int uid;  /**< Owner user ID */
  unsigned int gid;  /**< Owner group ID */
  unsigned long vsz; /**< Virtual memory size in KiB */
  unsigned long rss; /**< Resident set size in KiB */
  char state[4];     /**< Process state string (e.g., "S  ") */
  char comm[32];     /**< Base executable name (without .exe suffix) */
  char cmdline[256]; /**< Command line or executable name */
};

/**
 * @brief Retrieves information on posix-libproc availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_libproc_get_info(int *out_available);

/**
 * @brief Retrieves the filesystem path for the given process ID.
 * @param pid Process ID (or 0 for current process on Windows).
 * @param buffer Output buffer to receive the path.
 * @param buffersize Size of output buffer in bytes.
 * @return Number of bytes written on success, or 0 on error.
 */
int proc_pidpath(int pid, void *buffer, unsigned int buffersize);

/**
 * @brief Opens a cross-platform process enumeration cursor.
 * @return Opaque cursor pointer on success, or NULL on failure.
 */
void *posix_proc_open(void);

/**
 * @brief Reads the next process entry from an open process cursor.
 * @param[in,out] cursor Opaque cursor pointer returned by posix_proc_open().
 * @param[out] out_entry Structure populated with the next process's metrics.
 * @return 1 if an entry was read, or 0 when enumeration is complete.
 */
int posix_proc_next(void *cursor, struct posix_proc_entry *out_entry);

/**
 * @brief Closes a process enumeration cursor and releases its resources.
 * @param[in] cursor Opaque cursor pointer returned by posix_proc_open().
 */
void posix_proc_close(void *cursor);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_LIBPROC_H */
