/* clang-format off */
#include "bsd/stdlib.h"
#include "greatest.h"
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
static int cmp_int(const void *a, const void *b) {
  int ia = *(const int *)a;
  int ib = *(const int *)b;
  return (ia > ib) - (ia < ib);
}
#endif

TEST test_bsd_stdlib_functions(void) {
#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
  const char *errstr = NULL;
  void *p;
  void *ms;
  int arr[4] = {4, 1, 3, 2};
  const unsigned char *sarr[3];
  int hlen = 0;
  long bsize = 0;
  uint32_t rndbuf;

  p = reallocarray(NULL, 4, sizeof(int));
  ASSERT(p != NULL);
  p = recallocarray(p, 4, 8, sizeof(int));
  ASSERT(p != NULL);
  freezero(p, 8 * sizeof(int));

  setprogname("/usr/bin/myprog.exe");
  ASSERT_STR_EQ("myprog", getprogname());

  ASSERT_EQ(42, strtonum("42", 0, 100, &errstr));
  ASSERT(errstr == NULL);
  ASSERT_EQ(0, strtonum("200", 0, 100, &errstr));
  ASSERT(errstr != NULL);

  ms = bsd_setmode("0755");
  ASSERT(ms != NULL);
  ASSERT_EQ(0755, getmode(ms, 0));
  free(ms);

  ms = bsd_setmode("u+x");
  ASSERT(ms != NULL);
  ASSERT_EQ(0700, getmode(ms, 0600));
  free(ms);

  ASSERT_EQ(0, getbsize(&hlen, &bsize));
  ASSERT_EQ(512L, bsize);

  (void)arc4random();
  arc4random_buf(&rndbuf, sizeof(rndbuf));
  ASSERT(arc4random_uniform(10) < 10);

  ASSERT_EQ(0, mergesort(arr, 4, sizeof(int), cmp_int));
  ASSERT_EQ(1, arr[0]);
  ASSERT_EQ(4, arr[3]);

  arr[0] = 9;
  arr[1] = 2;
  ASSERT_EQ(0, heapsort(arr, 4, sizeof(int), cmp_int));
  ASSERT_EQ(2, arr[0]);

  sarr[0] = (const unsigned char *)"zeta";
  sarr[1] = (const unsigned char *)"alpha";
  sarr[2] = (const unsigned char *)"beta";
  ASSERT_EQ(0, radixsort(sarr, 3, NULL, 0));
  ASSERT_STR_EQ("alpha", (const char *)sarr[0]);
  ASSERT_EQ(0, sradixsort(sarr, 3, NULL, 0));

  ASSERT_EQ(1, rpmatch("yes"));
  ASSERT_EQ(0, rpmatch("no"));
  ASSERT_EQ(-1, rpmatch("maybe"));

  setproctitle("test");
  setproctitle_fast("test");
  ASSERT_EQ(0, daemon(1, 1));
  ASSERT(devname(0, 0) != NULL);
#endif
  (void)bsd_stdlib_dummy_for_coverage();
  PASS();
}

SUITE(suite_bsd_stdlib_core) { RUN_TEST(test_bsd_stdlib_functions); }
