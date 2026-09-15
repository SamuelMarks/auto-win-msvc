#ifndef BSD_SYS_EVENT_H
#define BSD_SYS_EVENT_H

/**
 * @file bsd-sys-event.h
 * @brief Polyfill for BSD <sys/event.h>.
 */

/* clang-format off */
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
 * @brief Error codes returned by bsd-sys-event functions.
 */
enum bsd_sys_event_error_code {
  /** @brief Successful operation. */
  BSD_SYS_EVENT_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  BSD_SYS_EVENT_ERROR_NULL_POINTER = 1,
  /** @brief Operation not supported on this platform. */
  BSD_SYS_EVENT_ERROR_NOT_SUPPORTED = 2
};

/**
 * @brief Retrieves whether kqueue is natively supported on the host platform.
 * @param[out] out_supported Pointer to an integer receiving 1 if supported, 0
 * otherwise.
 * @return BSD_SYS_EVENT_SUCCESS on success, or an error code on failure.
 */
enum bsd_sys_event_error_code bsd_sys_event_get_support(int *out_supported);

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
