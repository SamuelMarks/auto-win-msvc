/* clang-format off */
#include "greatest.h"
#include <kvm.h>
#include <libprocstat.h>
#include <sys/procctl.h>
#include <sys/mount.h>
#include <jail.h>
/* clang-format on */

TEST test_bsd_kvm_procstat(void) {
  kvm_t *kd;
  struct kinfo_proc *kp;
  struct kvm_swap sw;
  struct procstat *ps;
  unsigned int cnt = 0;
  int n = 0;
  int data = 0;

  kd = kvm_openfiles(NULL, NULL, NULL, 0, NULL);
  ASSERT(kd != NULL);
  kp = kvm_getprocs(kd, 0, 0, &n);
  ASSERT(kp != NULL);
  ASSERT(n >= 1);
  ASSERT_EQ(1, kvm_getswapinfo(kd, &sw, 1, 0));
  ASSERT_EQ(0, kvm_close(kd));

  ps = procstat_open_sysctl();
  ASSERT(ps != NULL);
  kp = procstat_getprocs(ps, 0, 0, &cnt);
  ASSERT(kp != NULL);
  ASSERT(cnt >= 1U);
  procstat_freeprocs(ps, kp);
  procstat_close(ps);

  ASSERT_EQ(0, procctl(P_PID, 0, PROC_SPROTECT, &data));
  ASSERT_EQ(-1, jail_getid("test"));
  PASS();
}

SUITE(suite_bsd_kvm_procstat_core) { RUN_TEST(test_bsd_kvm_procstat); }
