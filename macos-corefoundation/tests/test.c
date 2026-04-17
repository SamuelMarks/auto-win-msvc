/* test.c - Strict C89 Implementation */

/* clang-format off */
#include "CoreFoundation/CoreFoundation.h"
#include "greatest.h"
#include <string.h>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
/* clang-format on */

static int callback_fired = 0;

static void test_callback(CFSocketRef s, int type, CFDataRef address,
                          const void *data, void *info) {
  (void)s;
  (void)address;
  (void)data;

  if (type == kCFSocketReadCallBack) {
    if (info && strcmp((const char *)info, "test_info") == 0) {
      callback_fired = 1;
    }
  }
}

TEST test_cfsocket(void) {
#if defined(_WIN32)
  CFSocketContext ctx;
  CFSocketRef sock;
  CFRunLoopSourceRef src;
  CFRunLoopRef rl;
  SOCKET s[2];
  struct sockaddr_in addr;
  int addrlen = sizeof(addr);

  /* Create simple connected sockets (socketpair-like) for testing */
  s[0] = socket(AF_INET, SOCK_DGRAM, 0);
  s[1] = socket(AF_INET, SOCK_DGRAM, 0);

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;
  bind(s[0], (struct sockaddr *)&addr, sizeof(addr));
  getsockname(s[0], (struct sockaddr *)&addr, &addrlen);

  /* Create CFSocket for s[0] */
  memset(&ctx, 0, sizeof(ctx));
  ctx.info = (void *)"test_info";

  sock = CFSocketCreateWithNative(NULL, (CFSocketNativeHandle)s[0],
                                  kCFSocketReadCallBack, test_callback, &ctx);
  ASSERT(sock != NULL);

  src = CFSocketCreateRunLoopSource(NULL, sock, 0);
  ASSERT(src != NULL);

  rl = CFRunLoopGetCurrent();
  ASSERT(rl != NULL);

  CFRunLoopAddSource(rl, src, NULL);

  /* Send data to s[0] from s[1] */
  sendto(s[1], "hi", 2, 0, (struct sockaddr *)&addr, addrlen);

  /* Run the loop. It should fire the callback and we will stop it.
     But wait, our runloop blocks until stopped. So we must stop it inside the
     callback, or after a timeout. For this test, let's stop it inside the
     callback. */
#endif
  PASS();
}

static void test_callback_stop(CFSocketRef s, int type, CFDataRef address,
                               const void *data, void *info) {
  (void)s;
  (void)address;
  (void)data;
  (void)info;

  if (type == kCFSocketReadCallBack) {
    callback_fired = 1;
    CFRunLoopStop(CFRunLoopGetCurrent());
  }
}

TEST test_cfrunloop_run(void) {
#if defined(_WIN32)
  CFSocketContext ctx;
  CFSocketRef sock;
  CFRunLoopSourceRef src;
  CFRunLoopRef rl;
  SOCKET s[2];
  struct sockaddr_in addr;
  int addrlen = sizeof(addr);

  s[0] = socket(AF_INET, SOCK_DGRAM, 0);
  s[1] = socket(AF_INET, SOCK_DGRAM, 0);

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;
  bind(s[0], (struct sockaddr *)&addr, sizeof(addr));
  getsockname(s[0], (struct sockaddr *)&addr, &addrlen);

  memset(&ctx, 0, sizeof(ctx));
  callback_fired = 0;

  sock =
      CFSocketCreateWithNative(NULL, (CFSocketNativeHandle)s[0],
                               kCFSocketReadCallBack, test_callback_stop, &ctx);
  src = CFSocketCreateRunLoopSource(NULL, sock, 0);
  rl = CFRunLoopGetCurrent();
  CFRunLoopAddSource(rl, src, NULL);

  sendto(s[1], "hi", 2, 0, (struct sockaddr *)&addr, addrlen);

  CFRunLoopRun();

  ASSERT_EQ(1, callback_fired);

  CFSocketInvalidate(sock);
  CFRelease(src);
  CFRelease(sock);

  closesocket(s[0]);
  closesocket(s[1]);
#endif
  PASS();
}

SUITE(main_suite) {
  RUN_TEST(test_cfsocket);
  RUN_TEST(test_cfrunloop_run);
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
