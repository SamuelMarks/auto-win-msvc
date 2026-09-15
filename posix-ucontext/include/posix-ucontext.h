#ifndef POSIX_UCONTEXT_H
#define POSIX_UCONTEXT_H

/**
 * @file posix-ucontext.h
 * @brief POSIX user context switching interface.
 */

/* clang-format off */
#if !defined(_WIN32) || defined(__CYGWIN__)
#if !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 700
#endif
#include <ucontext.h>
#endif
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by posix-ucontext functions.
 */
enum posix_ucontext_error_code {
  /** @brief Operation completed successfully. */
  POSIX_UCONTEXT_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  POSIX_UCONTEXT_ERROR_NULL_POINTER = 1
};

/**
 * @brief Retrieves information on posix-ucontext availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return POSIX_UCONTEXT_SUCCESS on success, or
 * POSIX_UCONTEXT_ERROR_NULL_POINTER on NULL pointer.
 */
enum posix_ucontext_error_code posix_ucontext_get_info(int *out_available);

#if defined(_WIN32) && !defined(__CYGWIN__)

/**
 * @brief Structure representing a stack for context execution.
 */
typedef struct {
  void *ss_sp;
  int ss_flags;
  size_t ss_size;
} posix_stack_t;

/**
 * @brief Signal mask representation for ucontext.
 */
typedef unsigned long posix_sigset_t;

/**
 * @brief Machine context holding register values.
 */
typedef struct {
  unsigned __int64 gregs[23];
} posix_mcontext_t;

/**
 * @brief User-level execution context structure.
 */
typedef struct posix_ucontext {
  struct posix_ucontext *uc_link;
  posix_sigset_t uc_sigmask;
  posix_stack_t uc_stack;
  posix_mcontext_t uc_mcontext;
} ucontext_t;

/**
 * @brief Saves current execution context into ucp.
 * @param ucp Pointer to ucontext_t structure to populate.
 * @return 0 on success, or -1 on error.
 */
int getcontext(ucontext_t *ucp);

/**
 * @brief Restores execution context from ucp.
 * @param ucp Pointer to ucontext_t structure to activate.
 * @return 0 on success, or -1 on error.
 */
int setcontext(const ucontext_t *ucp);

/**
 * @brief Modifies a context to invoke func with the specified arguments.
 * @param ucp Pointer to context to initialize.
 * @param func Function to invoke when context is activated.
 * @param argc Number of integer arguments to pass to func.
 */
void makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...);

/**
 * @brief Saves current context in oucp and activates context ucp.
 * @param oucp Pointer to structure where current context is saved.
 * @param ucp Pointer to structure of context to activate.
 * @return 0 on success, or -1 on error.
 */
int swapcontext(ucontext_t *oucp, const ucontext_t *ucp);

#endif /* defined(_WIN32) && !defined(__CYGWIN__) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_UCONTEXT_H */
