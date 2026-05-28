#ifndef POSIX_UCONTEXT_H
#define POSIX_UCONTEXT_H
#include "auto_win_msvc_export.h"

#if defined(_WIN32) && !defined(__CYGWIN__)

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void *ss_sp;
  int ss_flags;
  size_t ss_size;
} posix_stack_t;

typedef unsigned long posix_sigset_t;

typedef struct {
  unsigned __int64 gregs[23];
} posix_mcontext_t;

typedef struct posix_ucontext {
  struct posix_ucontext *uc_link;
  posix_sigset_t uc_sigmask;
  posix_stack_t uc_stack;
  posix_mcontext_t uc_mcontext;
} ucontext_t;

/** \brief getcontext function. */
AUTO_WIN_MSVC_EXPORT int getcontext(ucontext_t *ucp);
/** \brief setcontext function. */
AUTO_WIN_MSVC_EXPORT int setcontext(const ucontext_t *ucp);
/** \brief makecontext function. */
AUTO_WIN_MSVC_EXPORT void makecontext(ucontext_t *ucp, void (*func)(void),
                                      int argc, ...);
/** \brief swapcontext function. */
AUTO_WIN_MSVC_EXPORT int swapcontext(ucontext_t *oucp, const ucontext_t *ucp);

#endif /* _WIN32 && !__CYGWIN__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_UCONTEXT_H */
