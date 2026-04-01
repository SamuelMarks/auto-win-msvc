#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
#include <wepoll.h>
#include <windows.h>
#include <io.h>

static HANDLE epoll_handles[1024];
static int next_epoll_fd = 100;

int posix_epoll_create(int size) {
  HANDLE h = epoll_create(size);
  if (h == NULL) return -1;
  int fd = next_epoll_fd++;
  epoll_handles[fd] = h;
  return fd;
}

int posix_epoll_create1(int flags) {
  HANDLE h = epoll_create1(flags);
  if (h == NULL) return -1;
  int fd = next_epoll_fd++;
  epoll_handles[fd] = h;
  return fd;
}

int posix_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) {
  HANDLE hephnd = epoll_handles[epfd];
  SOCKET s = (SOCKET)(unsigned int)fd;
  return epoll_ctl(hephnd, op, s, event);
}

int posix_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout) {
  HANDLE hephnd = epoll_handles[epfd];
  return epoll_wait(hephnd, events, maxevents, timeout);
}

int posix_epoll_close(int epfd) {
  HANDLE hephnd = epoll_handles[epfd];
  int res = epoll_close(hephnd);
  epoll_handles[epfd] = NULL;
  return res;
}
#endif
