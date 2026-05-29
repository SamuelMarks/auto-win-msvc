/* clang-format off */
#include <errno.h>
#include <stddef.h>
#ifndef ENOSYS
#define ENOSYS 38
#endif
/* linux-systemd.c - Strict C89 Implementation */

#include "systemd/sd-daemon.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ENOSYS
#define ENOSYS 38
#endif

/* linux-systemd.c - Strict C89 Implementation */

#include "systemd/sd-daemon.h"

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#include <winsock2.h>
#include <ws2tcpip.h>
/* clang-format on */
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

/* Windows 10 Insider Build 17063+ supports AF_UNIX */
#ifndef AF_UNIX
#define AF_UNIX 1
#endif

struct sockaddr_un {
  unsigned short sun_family;
  char sun_path[108];
};
#endif

struct sd_event_source {
  int fd;
  uint32_t events;
  sd_event_io_handler_t callback;
  void *userdata;
  int is_active;
};

struct sd_event {
  struct sd_event_source *sources;
  size_t sources_count;
  size_t sources_capacity;
  int run_flag;
};

/** \brief Notify service manager about state changes. */
int sd_notify(int unset_environment, const char *state) {
#if defined(_WIN32)
  char *sock_path;
  struct sockaddr_un un;
  SOCKET s;
  int ret;

  if (!state) {
    errno = EINVAL;
    return -1;
  }

  /* Get environment variable for NOTIFY_SOCKET */
#if defined(_MSC_VER)
  {
    size_t requiredSize;
    getenv_s(&requiredSize, NULL, 0, "NOTIFY_SOCKET");
    if (requiredSize == 0) {
      return 0;
    }
    sock_path = (char *)malloc(requiredSize);
    if (!sock_path) {
      errno = ENOMEM;
      return -1;
    }
    getenv_s(&requiredSize, sock_path, requiredSize, "NOTIFY_SOCKET");
  }
#else
  sock_path = getenv("NOTIFY_SOCKET");
#endif

  if (!sock_path || sock_path[0] == '\0') {
#if defined(_MSC_VER)
    if (sock_path)
      free(sock_path);
#endif
    /* If not set, systemd specifies we should return 0 */
    return 0;
  }

  if (unset_environment) {
    _putenv("NOTIFY_SOCKET=");
  }

  /* Basic check for socket paths */
  if (sock_path[0] != '@' && sock_path[0] != '/') {
#if defined(_MSC_VER)
    free(sock_path);
#endif
    errno = EINVAL;
    return -1;
  }

  s = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (s == INVALID_SOCKET) {
    /* Fallback to Named Pipe for older Windows */
    HANDLE hPipe;
    char pipe_path[256];
    DWORD written;

    if (sock_path[0] == '@') {
      /* Abstract namespace mapping */
#if defined(_MSC_VER)
      _snprintf_s(pipe_path, sizeof(pipe_path), _TRUNCATE, "\\\\.\\pipe\\%s",
                  sock_path + 1);
#else
      snprintf(pipe_path, sizeof(pipe_path), "\\\\.\\pipe\\%s", sock_path + 1);
#endif
    } else {
#if defined(_MSC_VER)
      _snprintf_s(pipe_path, sizeof(pipe_path), _TRUNCATE, "\\\\.\\pipe\\%s",
                  sock_path);
#else
      snprintf(pipe_path, sizeof(pipe_path), "\\\\.\\pipe\\%s", sock_path);
#endif
    }

    /* Convert forward slashes to backslashes if any */
    {
      char *p = pipe_path;
      while (*p) {
        if (*p == '/')
          *p = '\\';
        p++;
      }
    }

    hPipe =
        CreateFileA(pipe_path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPipe != INVALID_HANDLE_VALUE) {
      WriteFile(hPipe, state, (DWORD)strlen(state), &written, NULL);
      CloseHandle(hPipe);
#if defined(_MSC_VER)
      free(sock_path);
#endif
      return 1;
    }
#if defined(_MSC_VER)
    free(sock_path);
#endif
    /* Likely AF_UNIX and Named Pipe not supported/listening */
    return 0;
  }

  memset(&un, 0, sizeof(un));
  un.sun_family = AF_UNIX;
#if defined(_MSC_VER)
  strncpy_s(un.sun_path, sizeof(un.sun_path), sock_path, _TRUNCATE);
#else
  strncpy(un.sun_path, sock_path, sizeof(un.sun_path) - 1);
#endif
  if (un.sun_path[0] == '@') {
    un.sun_path[0] = '\0'; /* Abstract namespace */
  }

#if defined(_MSC_VER)
  free(sock_path);
#endif

  ret = sendto(s, state, (int)strlen(state), 0, (struct sockaddr *)&un,
               sizeof(un));
  closesocket(s);

  if (ret == SOCKET_ERROR) {
    return 0; /* systemd APIs usually return 0 on fail-to-notify due to env
                 issues */
  }

  return 1;
#else
  (void)unset_environment;
  (void)state;
  errno = ENOSYS;
  return -1;
#endif
}

/** \brief Acquire the default event loop object. */
int sd_event_default(sd_event **e) {
  if (!e) {
    errno = EINVAL;
    return -1;
  }

  *e = (sd_event *)malloc(sizeof(sd_event));
  if (!*e) {
    errno = ENOMEM;
    return -1;
  }

  (*e)->sources = NULL;
  (*e)->sources_count = 0;
  (*e)->sources_capacity = 0;
  (*e)->run_flag = 1;

  return 0;
}

/** \brief Add an I/O event source to an event loop. */
int sd_event_add_io(sd_event *e, sd_event_source **s, int fd, uint32_t events,
                    sd_event_io_handler_t callback, void *userdata) {
  struct sd_event_source *new_sources;

  if (!e || fd < 0 || !callback) {
    errno = EINVAL;
    return -1;
  }

  if (e->sources_count == e->sources_capacity) {
    size_t new_cap = e->sources_capacity == 0 ? 8 : e->sources_capacity * 2;
    new_sources = (struct sd_event_source *)realloc(
        e->sources, new_cap * sizeof(struct sd_event_source));
    if (!new_sources) {
      errno = ENOMEM;
      return -1;
    }
    e->sources = new_sources;
    e->sources_capacity = new_cap;
  }

  e->sources[e->sources_count].fd = fd;
  e->sources[e->sources_count].events = events;
  e->sources[e->sources_count].callback = callback;
  e->sources[e->sources_count].userdata = userdata;
  e->sources[e->sources_count].is_active = 1;

  if (s) {
    *s = &e->sources[e->sources_count];
  }

  e->sources_count++;

  return 0;
}

/** \brief Run the event loop. */
int sd_event_loop(sd_event *e) {
#if defined(_WIN32)
  fd_set readfds, writefds, exceptfds;
  int i, max_fd, ret;
  struct timeval tv;

  if (!e) {
    errno = EINVAL;
    return -1;
  }

  while (e->run_flag) {
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    max_fd = 0;

    for (i = 0; i < (int)e->sources_count; ++i) {
      if (!e->sources[i].is_active)
        continue;

      if (e->sources[i].events & EPOLLIN) {
        FD_SET((SOCKET)e->sources[i].fd, &readfds);
      }
      if (e->sources[i].events & EPOLLOUT) {
        FD_SET((SOCKET)e->sources[i].fd, &writefds);
      }
      FD_SET((SOCKET)e->sources[i].fd, &exceptfds);

      if (e->sources[i].fd > max_fd) {
        max_fd = e->sources[i].fd;
      }
    }

    tv.tv_sec = 1;
    tv.tv_usec = 0;

#undef select
    ret = select(max_fd + 1, &readfds, &writefds, &exceptfds, &tv);

    if (ret == SOCKET_ERROR) {
      errno = EINVAL;
      return -1;
    } else if (ret > 0) {
      for (i = 0; i < (int)e->sources_count; ++i) {
        uint32_t revents = 0;

        if (!e->sources[i].is_active)
          continue;

        if (FD_ISSET((SOCKET)e->sources[i].fd, &readfds)) {
          revents |= EPOLLIN;
        }
        if (FD_ISSET((SOCKET)e->sources[i].fd, &writefds)) {
          revents |= EPOLLOUT;
        }
        if (FD_ISSET((SOCKET)e->sources[i].fd, &exceptfds)) {
          revents |= EPOLLERR;
        }

        if (revents != 0) {
          int cb_ret = e->sources[i].callback(&e->sources[i], e->sources[i].fd,
                                              revents, e->sources[i].userdata);
          if (cb_ret < 0) {
            e->run_flag = 0;
            break;
          }
        }
      }
    }
  }

  return 0;
#else
  (void)e;
  errno = ENOSYS;
  return -1;
#endif
}

/** \brief Decrease the reference count of an event loop object. */
sd_event *sd_event_unref(sd_event *e) {
  if (e) {
    if (e->sources) {
      free(e->sources);
    }
    free(e);
  }
  return NULL;
}
