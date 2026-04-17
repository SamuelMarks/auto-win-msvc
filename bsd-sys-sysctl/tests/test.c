/* clang-format off */
#include "greatest.h"
#include "bsd-sys-sysctl.h"
#include <errno.h>
/* clang-format on */

TEST test_sysctl(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  int mib[4];
  int int_val = 0;
  size_t len = sizeof(int_val);
  unsigned char ull_val[8];
  size_t ull_len = sizeof(ull_val);

  /* Test NULL / invalid */
  ASSERT_EQ(-1, sysctl(NULL, 0, NULL, NULL, NULL, 0));
  ASSERT_EQ(EINVAL, errno);

  /* Test HW_NCPU */
  mib[0] = CTL_HW;
  mib[1] = HW_NCPU;
  ASSERT_EQ(0, sysctl(mib, 2, &int_val, &len, NULL, 0));
  ASSERT(int_val > 0);

  /* Test HW_PAGESIZE */
  mib[0] = CTL_HW;
  mib[1] = HW_PAGESIZE;
  ASSERT_EQ(0, sysctl(mib, 2, &int_val, &len, NULL, 0));
  ASSERT(int_val >= 4096);

  /* Test HW_MEMSIZE */
  mib[0] = CTL_HW;
  mib[1] = HW_MEMSIZE;
  ASSERT_EQ(0, sysctl(mib, 2, ull_val, &ull_len, NULL, 0));

  /* Test VM_OVERCOMMIT */
  mib[0] = CTL_VM;
  mib[1] = VM_OVERCOMMIT;
  ASSERT_EQ(0, sysctl(mib, 2, &int_val, &len, NULL, 0));
  ASSERT_EQ(0, int_val);

  /* Test KIPC_SOMAXCONN */
  mib[0] = CTL_KERN;
  mib[1] = KERN_IPC;
  mib[2] = KIPC_SOMAXCONN;
  ASSERT_EQ(0, sysctl(mib, 3, &int_val, &len, NULL, 0));
  ASSERT_EQ(0x7FFFFFFF, int_val);

  /* Test ENOMEM (buffer too small) */
  len = 1; /* int needs 4 */
  mib[0] = CTL_HW;
  mib[1] = HW_NCPU;
  ASSERT_EQ(-1, sysctl(mib, 2, &int_val, &len, NULL, 0));
  ASSERT_EQ(ENOMEM, errno);

  /* Test length query only */
  ASSERT_EQ(0, sysctl(mib, 2, NULL, &len, NULL, 0));
  ASSERT_EQ(sizeof(int), len);

  /* Test ENOENT (not found) */
  mib[0] = 9999;
  ASSERT_EQ(-1, sysctl(mib, 1, NULL, NULL, NULL, 0));
  ASSERT_EQ(ENOENT, errno);

#else
  PASS();
#endif
  PASS();
}

TEST test_sysctlbyname(void) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  int int_val = 0;
  size_t len = sizeof(int_val);
  unsigned char ull_val[8];
  size_t ull_len = sizeof(ull_val);

  /* Test NULL */
  ASSERT_EQ(-1, sysctlbyname(NULL, NULL, NULL, NULL, 0));
  ASSERT_EQ(EINVAL, errno);

  /* Test existing names */
  ASSERT_EQ(0, sysctlbyname("hw.ncpu", &int_val, &len, NULL, 0));
  ASSERT(int_val > 0);

  ASSERT_EQ(0, sysctlbyname("kern.smp.cpus", &int_val, &len, NULL, 0));
  ASSERT(int_val > 0);

  ASSERT_EQ(0, sysctlbyname("hw.pagesize", &int_val, &len, NULL, 0));
  ASSERT(int_val >= 4096);

  ASSERT_EQ(0, sysctlbyname("hw.physmem", ull_val, &ull_len, NULL, 0));

  ASSERT_EQ(0, sysctlbyname("hw.realmem", ull_val, &ull_len, NULL, 0));

  ASSERT_EQ(0, sysctlbyname("hw.usermem", ull_val, &ull_len, NULL, 0));

  ASSERT_EQ(0, sysctlbyname("hw.memsize", ull_val, &ull_len, NULL, 0));

  ASSERT_EQ(0, sysctlbyname("vm.overcommit", &int_val, &len, NULL, 0));
  ASSERT_EQ(0, int_val);

  ASSERT_EQ(0, sysctlbyname("kern.ipc.somaxconn", &int_val, &len, NULL, 0));
  ASSERT_EQ(0x7FFFFFFF, int_val);

  /* Test missing name */
  ASSERT_EQ(-1, sysctlbyname("missing.fake.name", NULL, NULL, NULL, 0));
  ASSERT_EQ(ENOENT, errno);
#else
  PASS();
#endif
  PASS();
}

SUITE(suite) {
  RUN_TEST(test_sysctl);
  RUN_TEST(test_sysctlbyname);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite);
  GREATEST_MAIN_END();
}
