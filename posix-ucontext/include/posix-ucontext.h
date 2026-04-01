#ifndef POSIX_UCONTEXT_H
#define POSIX_UCONTEXT_H

#if defined(_MSC_VER)

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
int getcontext(ucontext_t *ucp);
/** \brief setcontext function. */
int setcontext(const ucontext_t *ucp);

#ifdef __cplusplus
}
#endif

#endif /* _MSC_VER */

#endif /* POSIX_UCONTEXT_H */
