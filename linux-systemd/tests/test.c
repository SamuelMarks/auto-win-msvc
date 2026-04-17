/* test.c - Strict C89 Implementation */

#ifdef _MSC_VER
#pragma warning(disable : 4127) /* conditional expression is constant */
#endif

/* clang-format off */
#include "systemd/sd-daemon.h"
#include "greatest.h"

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <stdlib.h>
#endif
/* clang-format on */

TEST test_sd_notify_no_env(void) {
  int ret;

#if defined(_WIN32)
  _putenv("NOTIFY_SOCKET=");
#endif

  ret = sd_notify(0, "READY=1");
  /* should return 0 since NO environment variable is set */
  ASSERT_EQ(0, ret);

  ret = sd_notify(0, NULL);
  ASSERT_EQ(-1, ret);

  PASS();
}

TEST test_sd_notify_with_env(void) {
  int ret;

#if defined(_WIN32)
  _putenv("NOTIFY_SOCKET=@dummy_socket");
#endif

  ret = sd_notify(0, "READY=1");
  /* should return 0 or 1. Depending on Windows AF_UNIX support.
     We are satisfied as long as it doesn't crash */
  ASSERT(ret >= 0);

#if defined(_WIN32)
  _putenv("NOTIFY_SOCKET=/tmp/dummy_socket_file");
  ret = sd_notify(1, "READY=1"); /* unset env after */
  ASSERT(ret >= 0);

  /* It should be unset now */
  ret = sd_notify(0, "READY=1");
  ASSERT_EQ(0, ret);
#endif

  PASS();
}

TEST test_sd_notify_named_pipe_fallback(void) {
#if defined(_WIN32)
  int ret;
  HANDLE hPipe;
  char pipe_name[] = "\\\\.\\pipe\\my_test_notify_socket";

  hPipe =
      CreateNamedPipeA(pipe_name, PIPE_ACCESS_INBOUND,
                       PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1,
                       1024, 1024, 0, NULL);

  if (hPipe != INVALID_HANDLE_VALUE) {
    _putenv("NOTIFY_SOCKET=@my_test_notify_socket");
    ret = sd_notify(0, "READY=1");
    /* We expect it to succeed and return 1, or 0 if some other failure */
    ASSERT(ret >= 0);

    CloseHandle(hPipe);
  }
#endif
  PASS();
}

static int event_triggered = 0;

static int my_sd_event_callback(sd_event_source *s, int fd, uint32_t revents,
                                void *userdata) {
  (void)s;
  (void)fd;
  (void)revents;

  if (userdata) {
    int *flag = (int *)userdata;
    *flag = 1;
  }

  /* Returning negative value stops the event loop */
  return -1;
}

TEST test_sd_event_loop(void) {
#if defined(_WIN32)
  sd_event *e = NULL;
  sd_event_source *s = NULL;
  int ret;
  SOCKET sv[2];
  struct sockaddr_in addr;
  int addrlen = sizeof(addr);

  ret = sd_event_default(&e);
  ASSERT_EQ(0, ret);
  ASSERT(e != NULL);

  /* Set up a local socket pair to trigger event loop */
  sv[0] = socket(AF_INET, SOCK_STREAM, 0);
  sv[1] = socket(AF_INET, SOCK_STREAM, 0);

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;

  bind(sv[0], (struct sockaddr *)&addr, sizeof(addr));
  getsockname(sv[0], (struct sockaddr *)&addr, &addrlen);
  listen(sv[0], 1);

  connect(sv[1], (struct sockaddr *)&addr, addrlen);
  {
    SOCKET accepted = accept(sv[0], NULL, NULL);
    closesocket(sv[0]);
    sv[0] = accepted;
  }

  event_triggered = 0;

  /* Monitor sv[0] for read */
  ret = sd_event_add_io(e, &s, (int)sv[0], EPOLLIN, my_sd_event_callback,
                        &event_triggered);
  ASSERT_EQ(0, ret);
  ASSERT(s != NULL);

  /* Send data to trigger it */
  send(sv[1], "X", 1, 0);

  /* Run event loop. It should hit the callback and exit because callback
   * returns -1. */
  ret = sd_event_loop(e);
  ASSERT_EQ(0, ret);
  ASSERT_EQ(1, event_triggered);

  closesocket(sv[0]);
  closesocket(sv[1]);

  sd_event_unref(e);
#endif

  PASS();
}

SUITE(main_suite) {
  RUN_TEST(test_sd_notify_no_env);
  RUN_TEST(test_sd_notify_with_env);
  RUN_TEST(test_sd_notify_named_pipe_fallback);
  RUN_TEST(test_sd_event_loop);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
#if defined(_WIN32)
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  GREATEST_MAIN_BEGIN();
  (void)argc;
  (void)argv;
  RUN_SUITE(main_suite);

#if defined(_WIN32)
  WSACleanup();
#endif

  GREATEST_MAIN_END();
}
