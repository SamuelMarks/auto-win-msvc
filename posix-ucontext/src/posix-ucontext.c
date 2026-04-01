/* clang-format off */
#include <posix-ucontext.h>
/* clang-format on */

#if defined(_MSC_VER)

/** \brief getcontext function. */
int getcontext(ucontext_t *ucp) {
  if (!ucp)
    return -1;
  /* Does nothing, just an empty stub to satisfy compilation */
  return 0;
}

int setcontext(const ucontext_t *ucp) {
  if (!ucp)
    return -1;
  return 0;
}

#endif /* _MSC_VER */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;
/* Dummy function to prevent empty translation unit */
int dummy_posix_ucontext(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_ucontext;
