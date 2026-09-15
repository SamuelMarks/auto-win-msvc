#ifndef LINUX_BACKTRACE_H
#define LINUX_BACKTRACE_H

/**
 * @file linux-backtrace.h
 * @brief Polyfill for libbacktrace <backtrace.h>.
 */

/* clang-format off */
#include <stddef.h>
#if defined(_MSC_VER) && _MSC_VER < 1600
#ifndef _UINTPTR_T_DEFINED
#ifdef _WIN64
typedef unsigned __int64 uintptr_t;
#else
typedef unsigned int uintptr_t;
#endif
#define _UINTPTR_T_DEFINED
#endif
#else
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by linux-backtrace functions.
 */
enum linux_backtrace_error_code {
  /** @brief Successful operation. */
  LINUX_BACKTRACE_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  LINUX_BACKTRACE_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument passed to function. */
  LINUX_BACKTRACE_ERROR_INVALID_ARGUMENT = 2
};

/**
 * @brief Initializes and validates the linux-backtrace module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return LINUX_BACKTRACE_SUCCESS on success, or an error code on failure.
 */
enum linux_backtrace_error_code linux_backtrace_init(int *out_status);

/** @brief Opaque state structure for backtrace operations. */
struct backtrace_state;

/**
 * @brief Callback function type for backtrace errors.
 * @param data User data pointer.
 * @param msg Error message.
 * @param errnum System error number.
 */
typedef void (*backtrace_error_callback)(void *data, const char *msg,
                                         int errnum);

/**
 * @brief Callback function type for full backtrace symbol resolution.
 * @param data User data pointer.
 * @param pc Program counter value.
 * @param filename File name of the source location.
 * @param lineno Line number in the source file.
 * @param function Function name.
 * @return 0 to continue, non-zero to stop.
 */
typedef int (*backtrace_full_callback)(void *data, uintptr_t pc,
                                       const char *filename, int lineno,
                                       const char *function);

/**
 * @brief Creates a backtrace state.
 * @param filename File name of executable.
 * @param threaded Non-zero if multi-threaded.
 * @param error_callback Callback invoked on error.
 * @param data Opaque user data.
 * @return Pointer to backtrace state, or NULL on error.
 */
struct backtrace_state *
backtrace_create_state(const char *filename, int threaded,
                       backtrace_error_callback error_callback, void *data);

/**
 * @brief Retrieves symbol and file information for a program counter.
 * @param state Backtrace state.
 * @param pc Program counter address.
 * @param callback Callback invoked with line information.
 * @param error_callback Callback invoked on error.
 * @param data Opaque user data.
 * @return 0 on success, or non-zero on error.
 */
int backtrace_pcinfo(struct backtrace_state *state, uintptr_t pc,
                     backtrace_full_callback callback,
                     backtrace_error_callback error_callback, void *data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINUX_BACKTRACE_H */
