#include "posix-sys-ioctl.h"
#include <errno.h>
#include <stdarg.h>

#if defined(_MSC_VER) || defined(_WIN32)

/** \brief posix_ioctl function. */
int posix_ioctl(int fd, unsigned long request, ...) {
  int ret;
  va_list args;
  void *argp;

  va_start(args, request);
  argp = va_arg(args, void *);
  va_end(args);

  ret = ioctlsocket((SOCKET)fd, request, (u_long *)argp);
  if (ret != SOCKET_ERROR) {
    return 0;
  } else {
    int wsaErr = WSAGetLastError();
    if (wsaErr == WSAENOTSOCK) {
      errno = ENOTTY;
      return -1;
    } else if (wsaErr == WSAEFAULT) {
      errno = EFAULT;
      return -1;
    } else {
      errno = EINVAL;
      return -1;
    }
  }
}

#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_sys_ioctl(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_sys_ioctl;
