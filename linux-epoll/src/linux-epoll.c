#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)

#include <windows.h>
#include <wepoll.h>

int posix_epoll_create(int size) {
  HANDLE h = epoll_create(size);
  if (h == NULL) return -1;
  return (int)(intptr_t)h;
}

int posix_epoll_create1(int flags) {
  HANDLE h = epoll_create1(flags);
  if (h == NULL) return -1;
  return (int)(intptr_t)h;
}

int posix_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) {
  HANDLE hephnd = (HANDLE)(intptr_t)epfd;
  return epoll_ctl(hephnd, op, (SOCKET)(intptr_t)fd, event);
}

int posix_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout) {
  HANDLE hephnd = (HANDLE)(intptr_t)epfd;
  return epoll_wait(hephnd, events, maxevents, timeout);
}

#endif
