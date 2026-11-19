/* clang-format off */
#include "greatest.h"
#include "sys/sysctl.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
/* clang-format on */

#if defined(_WIN32) && defined(BSD_SYS_SYSCTL_SHARED)
__declspec(dllimport)
#endif
extern int g_mock_sysconf_fail;

TEST test_bsd_sys_sysctl_init(void) {
  int status = 0;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, bsd_sys_sysctl_init(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, bsd_sys_sysctl_init(&status));
  ASSERT_EQ(1, status);
  PASS();
}

TEST test_sysctlbyname_windows(void) {
#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)
  int val = 0;
  int mib[4];
  size_t len;
  size_t miblen;
  char strbuf[64];
  char tiny[2];

  /* sysctlbyname validation & hw.ncpu size/error/mock paths */
  ASSERT_EQ(-1, sysctlbyname(NULL, NULL, NULL, NULL, 0));
  ASSERT_EQ(-1, sysctlbyname("unknown.param", NULL, NULL, NULL, 0));

  /* sysctl_write_int: oldp==NULL && oldlenp==NULL, oldp==NULL && oldlenp!=NULL,
   * oldp!=NULL && oldlenp==NULL, oldp!=NULL && *oldlenp<sizeof(int), valid */
  ASSERT_EQ(0, sysctlbyname("hw.ncpu", NULL, NULL, NULL, 0));
  len = 0;
  ASSERT_EQ(0, sysctlbyname("hw.ncpu", NULL, &len, NULL, 0));
  ASSERT_EQ(sizeof(int), len);
  ASSERT_EQ(-1, sysctlbyname("hw.ncpu", &val, NULL, NULL, 0));
  len = 1;
  ASSERT_EQ(-1, sysctlbyname("hw.ncpu", &val, &len, NULL, 0));
  len = sizeof(val);
  ASSERT_EQ(0, sysctlbyname("hw.ncpu", &val, &len, NULL, 0));
  ASSERT(val > 0);

  g_mock_sysconf_fail = 1;
  len = sizeof(val);
  ASSERT_EQ(0, sysctlbyname("hw.ncpu", &val, &len, NULL, 0));
  g_mock_sysconf_fail = 0;

  /* sysctl_write_str: oldp==NULL && oldlenp==NULL, oldp==NULL && oldlenp!=NULL,
   * oldp!=NULL && oldlenp==NULL, oldp!=NULL && *oldlenp<slen, valid */
  ASSERT_EQ(0, sysctlbyname("kern.ostype", NULL, NULL, NULL, 0));
  len = 0;
  ASSERT_EQ(0, sysctlbyname("kern.ostype", NULL, &len, NULL, 0));
  ASSERT(len > 0);
  ASSERT_EQ(-1, sysctlbyname("kern.ostype", strbuf, NULL, NULL, 0));
  len = sizeof(tiny);
  ASSERT_EQ(-1, sysctlbyname("kern.ostype", tiny, &len, NULL, 0));
  len = sizeof(strbuf);
  ASSERT_EQ(0, sysctlbyname("kern.ostype", strbuf, &len, NULL, 0));
  ASSERT_STR_EQ("FreeBSD", strbuf);

  /* sysctlnametomib: all validation & named MIBs */
  miblen = 4;
  ASSERT_EQ(-1, sysctlnametomib(NULL, mib, &miblen));
  ASSERT_EQ(-1, sysctlnametomib("hw.ncpu", NULL, &miblen));
  ASSERT_EQ(-1, sysctlnametomib("hw.ncpu", mib, NULL));
  miblen = 1;
  ASSERT_EQ(-1, sysctlnametomib("hw.ncpu", mib, &miblen));

  miblen = 4;
  ASSERT_EQ(0, sysctlnametomib("hw.ncpu", mib, &miblen));
  miblen = 4;
  ASSERT_EQ(0, sysctlnametomib("hw.pagesize", mib, &miblen));
  miblen = 4;
  ASSERT_EQ(0, sysctlnametomib("kern.ostype", mib, &miblen));
  miblen = 4;
  ASSERT_EQ(0, sysctlnametomib("kern.osrelease", mib, &miblen));
  miblen = 4;
  ASSERT_EQ(0, sysctlnametomib("kern.osreldate", mib, &miblen));
  miblen = 4;
  ASSERT_EQ(-1, sysctlnametomib("kern.nonexistent", mib, &miblen));

  /* sysctl: invalid args + all CTL_HW and CTL_KERN leaves + unknown leaves */
  len = sizeof(val);
  ASSERT_EQ(-1, sysctl(NULL, 2, &val, &len, NULL, 0));
  ASSERT_EQ(-1, sysctl(mib, 1, &val, &len, NULL, 0));
  ASSERT_EQ(-1, sysctl(mib, CTL_MAXNAME + 1, &val, &len, NULL, 0));

  mib[0] = CTL_HW;
  mib[1] = HW_NCPU;
  len = sizeof(val);
  ASSERT_EQ(0, sysctl(mib, 2, &val, &len, NULL, 0));

  mib[1] = HW_PAGESIZE;
  len = sizeof(val);
  ASSERT_EQ(0, sysctl(mib, 2, &val, &len, NULL, 0));

  mib[1] = HW_BYTEORDER;
  len = sizeof(val);
  ASSERT_EQ(0, sysctl(mib, 2, &val, &len, NULL, 0));

  mib[1] = HW_MACHINE;
  len = sizeof(strbuf);
  ASSERT_EQ(0, sysctl(mib, 2, strbuf, &len, NULL, 0));

  mib[1] = HW_MACHINE_ARCH;
  len = sizeof(strbuf);
  ASSERT_EQ(0, sysctl(mib, 2, strbuf, &len, NULL, 0));

  mib[1] = HW_MODEL;
  len = sizeof(strbuf);
  ASSERT_EQ(0, sysctl(mib, 2, strbuf, &len, NULL, 0));

  mib[1] = 9999;
  len = sizeof(val);
  ASSERT_EQ(-1, sysctl(mib, 2, &val, &len, NULL, 0));

  mib[0] = CTL_KERN;
  mib[1] = KERN_OSTYPE;
  len = sizeof(strbuf);
  ASSERT_EQ(0, sysctl(mib, 2, strbuf, &len, NULL, 0));

  mib[1] = KERN_OSRELEASE;
  len = sizeof(strbuf);
  ASSERT_EQ(0, sysctl(mib, 2, strbuf, &len, NULL, 0));

  mib[1] = KERN_OSRELDATE;
  len = sizeof(val);
  ASSERT_EQ(0, sysctl(mib, 2, &val, &len, NULL, 0));

  mib[1] = KERN_ARGMAX;
  len = sizeof(val);
  ASSERT_EQ(0, sysctl(mib, 2, &val, &len, NULL, 0));

  mib[1] = KERN_NGROUPS;
  len = sizeof(val);
  ASSERT_EQ(0, sysctl(mib, 2, &val, &len, NULL, 0));

  mib[1] = KERN_HOSTNAME;
  len = sizeof(strbuf);
  ASSERT_EQ(0, sysctl(mib, 2, strbuf, &len, NULL, 0));

  mib[1] = 9999;
  len = sizeof(val);
  ASSERT_EQ(-1, sysctl(mib, 2, &val, &len, NULL, 0));

  mib[0] = 9999;
  mib[1] = 1;
  len = sizeof(val);
  ASSERT_EQ(-1, sysctl(mib, 2, &val, &len, NULL, 0));
#endif
  PASS();
}

TEST test_bsd_sys_sysctl_get_ncpu(void) {
  int ncpu = 0;

  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
            bsd_sys_sysctl_get_ncpu(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, bsd_sys_sysctl_get_ncpu(&ncpu));
  ASSERT(ncpu > 0);

  g_mock_sysconf_fail = 1;
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, bsd_sys_sysctl_get_ncpu(&ncpu));
  ASSERT(ncpu > 0);
  g_mock_sysconf_fail = 0;

  PASS();
}

SUITE(suite_bsd_sys_sysctl_core) {
  RUN_TEST(test_bsd_sys_sysctl_init);
  RUN_TEST(test_sysctlbyname_windows);
  RUN_TEST(test_bsd_sys_sysctl_get_ncpu);
}
