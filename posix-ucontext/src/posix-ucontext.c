/**
 * @file posix-ucontext.c
 * @brief Implementation of posix-ucontext polyfills.
 */

/* clang-format off */
#include "posix-ucontext.h"
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#if defined(_MSC_VER)
#include <intrin.h>
#endif
/* clang-format on */

#if defined(_WIN32) && !defined(__CYGWIN__)

#if defined(_M_AMD64) || defined(__x86_64__)
#if defined(_MSC_VER)
#define WIN_GET_CURRENT_FIBER() ((void *)__readgsqword(0x20))
#elif defined(__GNUC__) || defined(__clang__)
static void *win_get_current_fiber(void) {
  void *res;
  __asm__("movq %%gs:0x20, %0" : "=r"(res));
  return res;
}
#define WIN_GET_CURRENT_FIBER() win_get_current_fiber()
#else
#define WIN_GET_CURRENT_FIBER() NULL
#endif
#elif defined(_M_IX86) || defined(__i386__)
#if defined(_MSC_VER)
#define WIN_GET_CURRENT_FIBER() ((void *)(size_t)__readfsdword(0x10))
#elif defined(__GNUC__) || defined(__clang__)
static void *win_get_current_fiber(void) {
  void *res;
  __asm__("movl %%fs:0x10, %0" : "=r"(res));
  return res;
}
#define WIN_GET_CURRENT_FIBER() win_get_current_fiber()
#else
#define WIN_GET_CURRENT_FIBER() NULL
#endif
#else
#define WIN_GET_CURRENT_FIBER() NULL
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void *WIN_LPVOID;
typedef void(__stdcall *WIN_PFIBER_START_ROUTINE)(WIN_LPVOID lpFiberParameter);

__declspec(dllimport) void *__stdcall ConvertThreadToFiber(void *lpParameter);
__declspec(dllimport) void __stdcall SwitchToFiber(void *lpFiber);
__declspec(dllimport) void __stdcall DeleteFiber(void *lpFiber);
__declspec(dllimport) void *__stdcall CreateFiber(
    size_t dwStackSize, WIN_PFIBER_START_ROUTINE lpStartAddress,
    void *lpParameter);
__declspec(dllimport) void __stdcall ExitThread(unsigned long dwExitCode);

#ifdef __cplusplus
}
#endif

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
    if (dead_fibers[i] != WIN_GET_CURRENT_FIBER()) {
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

static void __stdcall posix_fiber_start(void *lpParameter) {
  struct posix_makecontext_args *margs;
  void (*func)(void);
  int argc;
  int args[8];
  ucontext_t *uc_link;
  void *my_fiber;
  int i;

  margs = (struct posix_makecontext_args *)lpParameter;
  func = margs->func;
  argc = margs->argc;
  uc_link = margs->uc_link;
  my_fiber = margs->my_fiber;

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

  if (uc_link != NULL) {
    setcontext(uc_link);
  }

  ExitThread(0);
}

/**
 * @brief Saves current execution context into ucp.
 */
int getcontext(ucontext_t *ucp) {
  void *fiber;

  if (ucp == NULL) {
    errno = EINVAL;
    return -1;
  }

  fiber = WIN_GET_CURRENT_FIBER();
  if (fiber == (void *)(size_t)0x1e00 || fiber == NULL) {
    fiber = ConvertThreadToFiber(NULL);
  }

  ucp->uc_mcontext.gregs[0] = (unsigned __int64)(size_t)fiber;
  ucp->uc_mcontext.gregs[1] = 0;

  return 0;
}

/**
 * @brief Restores execution context from ucp.
 */
int setcontext(const ucontext_t *ucp) {
  void *fiber;

  if (ucp == NULL) {
    errno = EINVAL;
    return -1;
  }

  fiber = (void *)(size_t)ucp->uc_mcontext.gregs[0];
  if (fiber == NULL) {
    errno = EINVAL;
    return -1;
  }

  SwitchToFiber(fiber);
  return 0;
}

/**
 * @brief Modifies a context to invoke func with the specified arguments.
 */
void makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...) {
  struct posix_makecontext_args *margs;
  va_list ap;
  void *fiber;
  int i;
  size_t stack_size;

  cleanup_dead_fibers();

  if (ucp == NULL || argc < 0 || argc > 8) {
    return;
  }

  margs = (struct posix_makecontext_args *)malloc(
      sizeof(struct posix_makecontext_args));
  if (margs == NULL) {
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

  stack_size = ucp->uc_stack.ss_size;
  if (stack_size == 0) {
    stack_size = 65536;
  }

  fiber = CreateFiber(stack_size, posix_fiber_start, margs);
  margs->my_fiber = fiber;

  ucp->uc_mcontext.gregs[0] = (unsigned __int64)(size_t)fiber;
  ucp->uc_mcontext.gregs[1] = 1;
}

/**
 * @brief Saves current context in oucp and activates context ucp.
 */
int swapcontext(ucontext_t *oucp, const ucontext_t *ucp) {
  void *fiber;

  cleanup_dead_fibers();

  if (oucp == NULL || ucp == NULL) {
    errno = EINVAL;
    return -1;
  }

  fiber = (void *)(size_t)ucp->uc_mcontext.gregs[0];
  if (fiber == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (getcontext(oucp) != 0) {
    return -1;
  }

  SwitchToFiber(fiber);
  return 0;
}

#endif /* defined(_WIN32) && !defined(__CYGWIN__) */

/**
 * @brief Retrieves information on posix-ucontext availability.
 */
enum posix_ucontext_error_code posix_ucontext_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_UCONTEXT_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_UCONTEXT_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_ucontext;
