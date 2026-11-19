#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
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
static void *win_get_current_fiber_raw(void) {
  return (void *)__readgsqword(0x20);
}
#elif defined(__GNUC__) || defined(__clang__)
static void *win_get_current_fiber_raw(void) {
  void *res;
  __asm__("movq %%gs:0x20, %0" : "=r"(res));
  return res;
}
#else
static void *win_get_current_fiber_raw(void) { return NULL; }
#endif
#elif defined(_M_IX86) || defined(__i386__)
#if defined(_MSC_VER)
static void *win_get_current_fiber_raw(void) {
  return (void *)(size_t)__readfsdword(0x10);
}
#elif defined(__GNUC__) || defined(__clang__)
static void *win_get_current_fiber_raw(void) {
  void *res;
  __asm__("movl %%fs:0x10, %0" : "=r"(res));
  return res;
}
#else
static void *win_get_current_fiber_raw(void) { return NULL; }
#endif
#else
static void *win_get_current_fiber_raw(void) { return NULL; }
#endif

typedef void *WIN_LPVOID;
typedef void(__stdcall *WIN_PFIBER_START_ROUTINE)(WIN_LPVOID lpFiberParameter);

__declspec(dllimport) void *__stdcall ConvertThreadToFiber(void *lpParameter);
__declspec(dllimport) void __stdcall SwitchToFiber(void *lpFiber);
__declspec(dllimport) void __stdcall DeleteFiber(void *lpFiber);
__declspec(dllimport) void *__stdcall
CreateFiber(size_t dwStackSize, WIN_PFIBER_START_ROUTINE lpStartAddress,
            void *lpParameter);
#if defined(_MSC_VER)
__declspec(dllimport)
__declspec(noreturn) void __stdcall ExitThread(unsigned long dwExitCode);
#elif defined(__GNUC__) || defined(__clang__)
__declspec(dllimport) void
    __attribute__((noreturn)) __stdcall ExitThread(unsigned long dwExitCode);
#else
__declspec(dllimport) void __stdcall ExitThread(unsigned long dwExitCode);
#endif

static void *default_ucontext_malloc(size_t sz) { return malloc(sz); }
static void *(*g_ucontext_malloc)(size_t) = default_ucontext_malloc;
static void *(*g_ucontext_get_fiber)(void) = win_get_current_fiber_raw;

/**
 * @brief Sets custom allocator and fiber-probe hooks for unit testing.
 */
void posix_ucontext_set_hooks(void *(*malloc_fn)(size_t),
                              void *(*get_fiber_fn)(void)) {
  g_ucontext_malloc = malloc_fn;
  g_ucontext_get_fiber = get_fiber_fn;
}

/**
 * @brief Resets allocator and fiber-probe hooks to defaults.
 */
void posix_ucontext_reset_hooks(void) {
  g_ucontext_malloc = default_ucontext_malloc;
  g_ucontext_get_fiber = win_get_current_fiber_raw;
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
    DeleteFiber(dead_fibers[i]);
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

  for (i = 0; i < argc; ++i) {
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

  fiber = g_ucontext_get_fiber();
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

  margs = (struct posix_makecontext_args *)g_ucontext_malloc(
      sizeof(struct posix_makecontext_args));
  if (margs == NULL) {
    return;
  }

  margs->func = func;
  margs->argc = argc;
  margs->uc_link = ucp->uc_link;

  va_start(ap, argc);
  for (i = 0; i < argc; ++i) {
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

  (void)getcontext(oucp);

  SwitchToFiber(fiber);
  return 0;
}

#elif defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)

/** @brief Get user context stub for DOS. */
int getcontext(ucontext_t *ucp) {
  if (ucp == NULL) {
    errno = EINVAL;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}

/** @brief Restore user context stub for DOS. */
int setcontext(const ucontext_t *ucp) {
  if (ucp == NULL) {
    errno = EINVAL;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}

/** @brief Modify user context stub for DOS. */
void makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...) {
  (void)ucp;
  (void)func;
  (void)argc;
}

/** @brief Swap user context stub for DOS. */
int swapcontext(ucontext_t *oucp, const ucontext_t *ucp) {
  if (oucp == NULL || ucp == NULL) {
    errno = EINVAL;
    return -1;
  }
  errno = ENOSYS;
  return -1;
}

#endif /* defined(_WIN32) && !defined(__CYGWIN__) */

/**
 * @brief Retrieves information on posix-ucontext availability.
 */
auto_win_msvc_error_t posix_ucontext_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_ucontext;

#endif
typedef int make_iso_compilers_happy_mega_batch3_25;
