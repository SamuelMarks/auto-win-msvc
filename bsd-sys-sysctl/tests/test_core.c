#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-sys-sysctl.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_bsd_sys_sysctl_init(void) {
  enum bsd_sys_sysctl_error_code rc;
  int status;

  status = 0;
  rc = bsd_sys_sysctl_init(NULL);
  if (rc != BSD_SYS_SYSCTL_ERROR_NULL_POINTER) {
    printf("Expected BSD_SYS_SYSCTL_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = bsd_sys_sysctl_init(&status);
  if (rc != BSD_SYS_SYSCTL_SUCCESS) {
    printf("bsd_sys_sysctl_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_bsd_sys_sysctl_get_ncpu(void) {
  enum bsd_sys_sysctl_error_code rc;
  int ncpu;

  ncpu = 0;
  rc = bsd_sys_sysctl_get_ncpu(NULL);
  if (rc != BSD_SYS_SYSCTL_ERROR_NULL_POINTER) {
    printf("Expected BSD_SYS_SYSCTL_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = bsd_sys_sysctl_get_ncpu(&ncpu);
  if (rc != BSD_SYS_SYSCTL_SUCCESS) {
    printf("bsd_sys_sysctl_get_ncpu failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT(ncpu > 0);
  PASS();
}

TEST test_sysctl(void) {
#if defined(_WIN32)
  int mib[4];
  int int_val;
  size_t len;
  error_type_t err;
#if defined(_MSC_VER) || defined(__WATCOMC__)
  unsigned __int64 u64_val;
#else
  unsigned long long u64_val;
#endif
  unsigned int u32_val;

  /* NULL / 0 namelen */
  err = sysctl(NULL, 0, &int_val, &len, NULL, 0);
  ASSERT_EQ(EINVAL, err);

  mib[0] = CTL_HW;
  mib[1] = HW_NCPU;

  /* query len only */
  len = 0;
  err = sysctl(mib, 2, NULL, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);
  ASSERT_EQ(sizeof(int), len);

  /* buffer too small */
  len = 1;
  err = sysctl(mib, 2, &int_val, &len, NULL, 0);
  ASSERT_EQ(ENOMEM, err);

  /* valid query */
  len = sizeof(int_val);
  err = sysctl(mib, 2, &int_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);
  ASSERT(int_val > 0);

  /* HW_PAGESIZE */
  mib[1] = HW_PAGESIZE;
  len = 0;
  err = sysctl(mib, 2, NULL, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);
  len = 1;
  err = sysctl(mib, 2, &int_val, &len, NULL, 0);
  ASSERT_EQ(ENOMEM, err);
  len = sizeof(int_val);
  err = sysctl(mib, 2, &int_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);
  ASSERT(int_val > 0);

  /* HW_PHYSMEM */
  mib[1] = HW_PHYSMEM;
  len = 0;
  err = sysctl(mib, 2, NULL, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);
  len = 1;
  err = sysctl(mib, 2, &int_val, &len, NULL, 0);
  ASSERT_EQ(ENOMEM, err);
  len = sizeof(u32_val);
  err = sysctl(mib, 2, &u32_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);
  len = sizeof(u64_val);
  err = sysctl(mib, 2, &u64_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);

  /* CTL_VM / VM_OVERCOMMIT */
  mib[0] = CTL_VM;
  mib[1] = VM_OVERCOMMIT;
  len = 0;
  err = sysctl(mib, 2, NULL, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);
  len = 1;
  err = sysctl(mib, 2, &int_val, &len, NULL, 0);
  ASSERT_EQ(ENOMEM, err);
  len = sizeof(int_val);
  err = sysctl(mib, 2, &int_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);
  ASSERT_EQ(0, int_val);

  /* CTL_KERN / KERN_IPC / KIPC_SOMAXCONN */
  mib[0] = CTL_KERN;
  mib[1] = KERN_IPC;
  mib[2] = KIPC_SOMAXCONN;
  len = 0;
  err = sysctl(mib, 3, NULL, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);
  len = 1;
  err = sysctl(mib, 3, &int_val, &len, NULL, 0);
  ASSERT_EQ(ENOMEM, err);
  len = sizeof(int_val);
  err = sysctl(mib, 3, &int_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);
  ASSERT_EQ(0x7FFFFFFF, int_val);

  /* Unknown MIB */
  mib[0] = 999;
  err = sysctl(mib, 1, &int_val, &len, NULL, 0);
  ASSERT_EQ(ENOENT, err);
#endif
  PASS();
}

TEST test_sysctlbyname(void) {
#if defined(_WIN32)
  int int_val;
  size_t len;
  error_type_t err;

  /* NULL name */
  err = sysctlbyname(NULL, &int_val, &len, NULL, 0);
  ASSERT_EQ(EINVAL, err);

  /* Unknown name */
  err = sysctlbyname("unknown.metric", &int_val, &len, NULL, 0);
  ASSERT_EQ(ENOENT, err);

  len = sizeof(int_val);
  err = sysctlbyname("hw.ncpu", &int_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);

  len = sizeof(int_val);
  err = sysctlbyname("kern.smp.cpus", &int_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);

  len = sizeof(int_val);
  err = sysctlbyname("hw.pagesize", &int_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);

  len = sizeof(int_val);
  err = sysctlbyname("hw.physmem", &int_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);

  len = sizeof(int_val);
  err = sysctlbyname("hw.realmem", &int_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);

  len = sizeof(int_val);
  err = sysctlbyname("hw.usermem", &int_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);

  len = sizeof(int_val);
  err = sysctlbyname("hw.memsize", &int_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);

  len = sizeof(int_val);
  err = sysctlbyname("vm.overcommit", &int_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);

  len = sizeof(int_val);
  err = sysctlbyname("kern.ipc.somaxconn", &int_val, &len, NULL, 0);
  ASSERT_EQ(ERR_NONE, err);
#endif
  PASS();
}

SUITE(suite_bsd_sys_sysctl_core) {
  RUN_TEST(test_bsd_sys_sysctl_init);
  RUN_TEST(test_bsd_sys_sysctl_get_ncpu);
  RUN_TEST(test_sysctl);
  RUN_TEST(test_sysctlbyname);
}
