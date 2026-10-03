#ifndef BSD_SYS_EVENT_H
#define BSD_SYS_EVENT_H

/**
 * @file bsd-sys-event.h
 * @brief Polyfill for BSD <sys/event.h>.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#include <stddef.h>
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <sys/event.h>
#include <sys/time.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieves whether kqueue is natively supported on the host platform.
 * @param[out] out_supported Pointer to an integer receiving 1 if supported, 0
 * otherwise.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or an error code on failure.
 */
auto_win_msvc_error_t bsd_sys_event_get_support(int *out_supported);

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)

/**
 * @brief Structure describing an event in kqueue.
 */
struct kevent {
  /** @brief Identifier for this event. */
  unsigned int ident;
  /** @brief Filter for event. */
  short filter;
  /** @brief Action flags for kqueue. */
  unsigned short flags;
  /** @brief Filter-specific flag bits. */
  unsigned int fflags;
  /** @brief Filter-specific data. */
  int data;
  /** @brief Opaque user data. */
  void *udata;
};

struct timespec;

/**
 * @brief Creates a new kernel event queue.
 * @return File descriptor of new kqueue, or -1 on error with errno set.
 */
int kqueue(void);

/**
 * @brief Registers events with queue and returns pending events.
 * @param kq Kernel event queue descriptor.
 * @param changelist Array of kevent structures to change.
 * @param nchanges Number of entries in changelist.
 * @param eventlist Array of kevent structures for returned events.
 * @param nevents Maximum number of events to return.
 * @param timeout Maximum time to wait.
 * @return Number of events placed in eventlist, or -1 on error with errno set.
 */
int kevent(int kq, const struct kevent *changelist, int nchanges,
           struct kevent *eventlist, int nevents,
           const struct timespec *timeout);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_EVENT_H */
