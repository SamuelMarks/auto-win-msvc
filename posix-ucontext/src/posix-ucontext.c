/* clang-format off */
#include <posix-ucontext.h>

#if defined(_WIN32) && !defined(__CYGWIN__)

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
/* clang-format on */

/** \brief getcontext function. */
int getcontext(ucontext_t *ucp) {
  void *fiber;

  if (!ucp) {
    errno = EINVAL;
    return -1;
  }

  fiber = GetCurrentFiber();
  if (fiber == (void *)0x1e00 || fiber == NULL) {
    fiber = ConvertThreadToFiber(NULL);
  }

  ucp->uc_mcontext.gregs[0] = (unsigned __int64)(size_t)fiber;
  ucp->uc_mcontext.gregs[1] = 0; /* Indicates not created by makecontext */

  return 0;
}

/** \brief setcontext function. */
int setcontext(const ucontext_t *ucp) {
  void *fiber;

  if (!ucp) {
    errno = EINVAL;
    return -1;
  }

  fiber = (void *)(size_t)ucp->uc_mcontext.gregs[0];
  if (!fiber) {
    errno = EINVAL;
    return -1;
  }

  SwitchToFiber(fiber);

  return 0;
}

#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#define THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
#define THREAD_LOCAL __thread
#else
#define THREAD_LOCAL
#endif

#define MAX_DEAD_FIBERS 16
static THREAD_LOCAL void *dead_fibers[MAX_DEAD_FIBERS];
static THREAD_LOCAL int num_dead_fibers = 0;

static void cleanup_dead_fibers(void) {
  int i;
  for (i = 0; i < num_dead_fibers; ++i) {
    if (dead_fibers[i] != GetCurrentFiber()) {
      DeleteFiber(dead_fibers[i]);
    }
  }
  num_dead_fibers = 0;
}

struct posix_makecontext_args {
  void (*func)(void);
  int argc;
  int args[8];
  ucontext_t *uc_link;
  void *my_fiber;
};

static VOID WINAPI posix_fiber_start(LPVOID lpParameter) {
  struct posix_makecontext_args *margs =
      (struct posix_makecontext_args *)lpParameter;
  void (*func)(void) = margs->func;
  int argc = margs->argc;
  int args[8];
  ucontext_t *uc_link = margs->uc_link;
  void *my_fiber = margs->my_fiber;
  int i;

  for (i = 0; i < 8 && i < argc; ++i) {
    args[i] = margs->args[i];
  }

  free(margs);

  switch (argc) {
  case 0:
    func();
    break;
  case 1:
    ((void (*)(int))func)(args[0]);
    break;
  case 2:
    ((void (*)(int, int))func)(args[0], args[1]);
    break;
  case 3:
    ((void (*)(int, int, int))func)(args[0], args[1], args[2]);
    break;
  case 4:
    ((void (*)(int, int, int, int))func)(args[0], args[1], args[2], args[3]);
    break;
  case 5:
    ((void (*)(int, int, int, int, int))func)(args[0], args[1], args[2],
                                              args[3], args[4]);
    break;
  case 6:
    ((void (*)(int, int, int, int, int, int))func)(args[0], args[1], args[2],
                                                   args[3], args[4], args[5]);
    break;
  case 7:
    ((void (*)(int, int, int, int, int, int, int))func)(
        args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    break;
  case 8:
  default:
    ((void (*)(int, int, int, int, int, int, int, int))func)(
        args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
    break;
  }

  if (num_dead_fibers < MAX_DEAD_FIBERS) {
    dead_fibers[num_dead_fibers++] = my_fiber;
  }

  if (uc_link) {
    setcontext(uc_link);
  }

  /* POSIX spec says: If uc_link is NULL, thread exits. */
  ExitThread(0);
}

/** \brief makecontext function. */
void makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...) {
  struct posix_makecontext_args *margs;
  va_list ap;
  void *fiber;
  int i;
  SIZE_T stack_size;

  cleanup_dead_fibers();

  if (!ucp || argc < 0 || argc > 8) {
    return;
  }

  margs = (struct posix_makecontext_args *)malloc(
      sizeof(struct posix_makecontext_args));
  if (!margs) {
    return;
  }

  margs->func = func;
  margs->argc = argc;
  margs->uc_link = ucp->uc_link;

  va_start(ap, argc);
  for (i = 0; i < argc && i < 8; ++i) {
    margs->args[i] = va_arg(ap, int);
  }
  va_end(ap);

  stack_size = (SIZE_T)ucp->uc_stack.ss_size;
  if (stack_size == 0) {
    /* Typical default if uninitialized or zero */
    stack_size = 0;
  }

  fiber = CreateFiber(stack_size, posix_fiber_start, margs);
  margs->my_fiber = fiber;

  ucp->uc_mcontext.gregs[0] = (unsigned __int64)(size_t)fiber;
  ucp->uc_mcontext.gregs[1] = 1; /* Indicates created by makecontext */
}

/** \brief swapcontext function. */
int swapcontext(ucontext_t *oucp, const ucontext_t *ucp) {
  void *fiber;

  cleanup_dead_fibers();

  if (!oucp || !ucp) {
    errno = EINVAL;
    return -1;
  }

  fiber = (void *)(size_t)ucp->uc_mcontext.gregs[0];
  if (!fiber) {
    errno = EINVAL;
    return -1;
  }

  if (getcontext(oucp) != 0) {
    return -1;
  }

  SwitchToFiber(fiber);

  return 0;
}

#endif /* _WIN32 && !__CYGWIN__ */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;
/* Dummy function to prevent empty translation unit */
int dummy_posix_ucontext(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_ucontext;
