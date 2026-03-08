#ifndef BSD_SYS_EVENT_H
#define BSD_SYS_EVENT_H
#if defined(_MSC_VER) && !defined(__clang__)
struct kevent { unsigned int ident; short filter; unsigned short flags; unsigned int fflags; int data; void *udata; };
int kqueue(void);
struct timespec;
int kevent(int kq, const struct kevent *changelist, int nchanges, struct kevent *eventlist, int nevents, const struct timespec *timeout);
#endif
#endif
