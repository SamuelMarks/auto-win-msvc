/* clang-format off */
#include "bsd-sys-event.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
/** @brief Creates a new kernel event queue stub. */
int kqueue(void) {
  errno = ENOSYS;
  return -1;
}

/** @brief Registers events with queue stub. */
int kevent(int kq, const struct kevent *changelist, int nchanges,
           struct kevent *eventlist, int nevents,
           const struct timespec *timeout) {
  (void)kq;
  (void)changelist;
  (void)nchanges;
  (void)eventlist;
  (void)nevents;
  (void)timeout;
  errno = ENOSYS;
  return -1;
}
#endif

/** @brief Retrieves whether kqueue is natively supported on the host platform.
 */
enum bsd_sys_event_error_code bsd_sys_event_get_support(int *out_supported) {
  if (out_supported == NULL) {
    return BSD_SYS_EVENT_ERROR_NULL_POINTER;
  }
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) ||      \
    defined(__NetBSD__)
  *out_supported = 1;
#else
  *out_supported = 0;
#endif
  return BSD_SYS_EVENT_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_bsd_sys_event;
