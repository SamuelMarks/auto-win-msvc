
#ifdef __cplusplus
extern "C" {
#endif

#ifndef BSD_SYS_EVENT_H
#define BSD_SYS_EVENT_H
#include "auto_win_msvc_export.h"

#if defined(_MSC_VER) && !defined(__clang__)
struct kevent {
  unsigned int ident;
  short filter;
  unsigned short flags;
  unsigned int fflags;
  int data;
  void *udata;
};
/** \brief kqueue function. */
AUTO_WIN_MSVC_EXPORT int kqueue(void);
struct timespec;
/** \brief kevent function. */
AUTO_WIN_MSVC_EXPORT int kevent(int kq, const struct kevent *changelist,
                                int nchanges, struct kevent *eventlist,
                                int nevents, const struct timespec *timeout);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
