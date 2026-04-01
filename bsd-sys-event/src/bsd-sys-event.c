/* clang-format off */
#include "bsd-sys-event.h"
#include <errno.h>
/* clang-format on */
#if defined(_MSC_VER) && !defined(__clang__)
/** \brief kqueue function. */
int kqueue(void) {
  errno = EINVAL;
  return -1;
}
/** \brief kevent function. */
int kevent(int kq, const struct kevent *changelist, int nchanges,
           struct kevent *eventlist, int nevents,
           const struct timespec *timeout) {
  (void)kq;
  (void)changelist;
  (void)nchanges;
  (void)eventlist;
  (void)nevents;
  (void)timeout;
  errno = EINVAL;
  return -1;
}
#endif

/* Dummy function to prevent empty translation unit */
int dummy_bsd_sys_event(void) { return 0; }

typedef int make_iso_compilers_happy_tu_bsd_sys_event;
