#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "CoreFoundation/CoreFoundation.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

#ifndef ENOSYS
#define ENOSYS 38
#endif

TEST test_macos_corefoundation_init(void) {
  enum macos_corefoundation_error_code rc;
  int status;

  status = 0;
  rc = macos_corefoundation_init(NULL);
  if (rc != MACOS_COREFOUNDATION_ERROR_NULL_POINTER) {
    printf("Expected MACOS_COREFOUNDATION_ERROR_NULL_POINTER, got %d\n",
           (int)rc);
    FAIL();
  }

  rc = macos_corefoundation_init(&status);
  if (rc != MACOS_COREFOUNDATION_SUCCESS) {
    printf("macos_corefoundation_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

static void dummy_cf_cb(CFSocketRef s, int type, CFDataRef address,
                        const void *data, void *info) {
  (void)s;
  (void)type;
  (void)address;
  (void)data;
  (void)info;
}

TEST test_macos_corefoundation_operations(void) {
  CFRunLoopRef rl;
  CFSocketRef s;
  error_type_t err;

  rl = NULL;
  err = CFRunLoopGetCurrent(&rl);
  ASSERT_EQ(ERR_NONE, err);
#if defined(_WIN32)
  ASSERT(rl != NULL);
#endif

  err = CFRunLoopStop(rl);
  ASSERT_EQ(ERR_NONE, err);

  s = NULL;
  err = CFSocketCreateWithNative(NULL, 0, kCFSocketReadCallBack, dummy_cf_cb,
                                 NULL, &s);
#if defined(_WIN32)
  {
    CFRunLoopSourceRef src;
    ASSERT_EQ(ERR_NONE, err);
    ASSERT(s != NULL);

    src = NULL;
    err = CFSocketCreateRunLoopSource(NULL, s, 0, &src);
    ASSERT_EQ(ERR_NONE, err);
    ASSERT(src != NULL);

    err = CFRunLoopAddSource(rl, src, NULL);
    ASSERT_EQ(ERR_NONE, err);

    err = CFSocketEnableCallBacks(s, kCFSocketWriteCallBack);
    ASSERT_EQ(ERR_NONE, err);

    err = CFSocketDisableCallBacks(s, kCFSocketWriteCallBack);
    ASSERT_EQ(ERR_NONE, err);

    err = CFSocketInvalidate(s);
    ASSERT_EQ(ERR_NONE, err);

    CFRelease(src);
    CFRelease(s);
  }
#else
  ASSERT_EQ(ENOSYS, err);
#endif

  PASS();
}

SUITE(suite_macos_corefoundation_core) {
  RUN_TEST(test_macos_corefoundation_init);
  RUN_TEST(test_macos_corefoundation_operations);
}
