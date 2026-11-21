/* clang-format off */
#include "greatest.h"
#include <kvm.h>
#include <libprocstat.h>
#include <libutil.h>
#include <sys/procctl.h>
#include <sys/ptrace.h>
#include <devstat.h>
#include <memstat.h>
#include <sys/mount.h>
#include <fstab.h>
#include <ufs/ufs/quota.h>
#include <jail.h>
#include <sys/rctl.h>
#include <sys/acl.h>
#include <sys/extattr.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#if defined(__linux__)
#include <fcntl.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <winsock2.h>
#endif
/* clang-format on */

static int g_kvm_oom_countdown = 0;

static void *mock_kvm_malloc(size_t sz) {
  if (g_kvm_oom_countdown > 0) {
    g_kvm_oom_countdown--;
    if (g_kvm_oom_countdown == 0) {
      return NULL;
    }
  }
  return malloc(sz);
}

static void *mock_kvm_calloc(size_t n, size_t sz) {
  if (g_kvm_oom_countdown > 0) {
    g_kvm_oom_countdown--;
    if (g_kvm_oom_countdown == 0) {
      return NULL;
    }
  }
  return calloc(n, sz);
}

static void *mock_kvm_realloc(void *p, size_t sz) {
  if (g_kvm_oom_countdown > 0) {
    g_kvm_oom_countdown--;
    if (g_kvm_oom_countdown == 0) {
      return NULL;
    }
  }
  return realloc(p, sz);
}

static char *mock_kvm_strdup(const char *s) {
  size_t len;
  char *p;
  if (g_kvm_oom_countdown > 0) {
    g_kvm_oom_countdown--;
    if (g_kvm_oom_countdown == 0) {
      return NULL;
    }
  }
  len = strlen(s) + 1;
  p = (char *)malloc(len);
  if (p != NULL) {
    memcpy(p, s, len);
  }
  return p;
}

#if defined(_WIN32)
static int g_win_snap_fail = 0;
static int g_win_proc_total = 0;
static int g_win_proc_idx = 0;

static void *mock_win_snap(void) {
  if (g_win_snap_fail) {
    return (void *)INVALID_HANDLE_VALUE;
  }
  return (void *)(size_t)0x1234U;
}

static int mock_win_proc_step(void *snap, int first, unsigned long *pid,
                              unsigned long *ppid, unsigned long *threads,
                              char *comm, size_t comm_sz) {
  (void)snap;
  if (first) {
    g_win_proc_idx = 0;
  }
  if (g_win_proc_idx >= g_win_proc_total) {
    return 0;
  }
  g_win_proc_idx++;
  *pid = (unsigned long)(100 + g_win_proc_idx);
  *ppid = 1UL;
  *threads = 2UL;
  if (comm_sz > 5) {
    memcpy(comm, "mock", 5);
  }
  return 1;
}

static int mock_win_close_handle(void *h) {
  (void)h;
  return 1;
}

static unsigned long mock_win_tbl_ok(void *buf, unsigned long *sz) {
  (void)buf;
  *sz = 0;
  return 0UL;
}

static unsigned long mock_win_tbl_insuf(void *buf, unsigned long *sz) {
  if (buf == NULL) {
    *sz = 64UL;
    return (unsigned long)ERROR_INSUFFICIENT_BUFFER;
  }
  return 0UL;
}

static int mock_win_mem_fail(void *ms) {
  (void)ms;
  return 0;
}
#endif

TEST test_bsd_kvm_procstat(void) {
  char errbuf[256];
  char readbuf[16];
  long cp_time[5];
  struct nlist nl_empty[2];
  struct nlist nl_null[2];
  struct kvm_swap sw;
  kvm_t *kd;
  struct kinfo_proc *kp;
  int n = 0;

  /* OOM on kvm_openfiles with and without errbuf */
  bsd_kvm_procstat_set_alloc_hooks(mock_kvm_malloc, mock_kvm_calloc,
                                   mock_kvm_realloc, mock_kvm_strdup);
  g_kvm_oom_countdown = 1;
  errbuf[0] = '\0';
  ASSERT(kvm_openfiles(NULL, NULL, NULL, 0, errbuf) == NULL);
  ASSERT_STR_EQ("out of memory", errbuf);

  g_kvm_oom_countdown = 1;
  ASSERT(kvm_open(NULL, NULL, NULL, 0, "err") == NULL);
  bsd_kvm_procstat_set_alloc_hooks(NULL, NULL, NULL, NULL);

  ASSERT_EQ(-1, kvm_close(NULL));
  ASSERT_STR_EQ("kvm error", kvm_geterr(NULL));

  /* Open and close withoutprocs, plus open2 */
  kd = kvm_open2(NULL, NULL, 0, errbuf, NULL);
  ASSERT(kd != NULL);
  ASSERT(kvm_geterr(kd) != NULL);
  ASSERT_EQ(0, kvm_close(kd));

  kd = kvm_open(NULL, NULL, NULL, 0, NULL);
  ASSERT(kd != NULL);

  /* kvm_getprocs invalid args, OOM, and repeated call freeing prior procs */
  ASSERT(kvm_getprocs(NULL, 0, 0, &n) == NULL);
  ASSERT(kvm_getprocs(kd, 0, 0, NULL) == NULL);

  bsd_kvm_procstat_set_alloc_hooks(mock_kvm_malloc, mock_kvm_calloc,
                                   mock_kvm_realloc, mock_kvm_strdup);
#if defined(_WIN32)
  bsd_kvm_procstat_set_win32_hooks(mock_win_snap, mock_win_proc_step,
                                   mock_win_close_handle, NULL, NULL, NULL,
                                   NULL);
  g_win_snap_fail = 1;
  g_kvm_oom_countdown = 1;
  ASSERT(kvm_getprocs(kd, 0, 0, &n) == NULL);
  ASSERT(kvm_getprocs(kd, 0, 0, &n) != NULL);

  /* Snapshot ok, calloc OOM -> fallback ok */
  g_win_snap_fail = 0;
  g_win_proc_total = 2;
  g_kvm_oom_countdown = 1;
  ASSERT(kvm_getprocs(kd, 0, 0, &n) != NULL);

  /* Snapshot ok, 0 processes -> frees initial buffer and falls back */
  g_win_proc_total = 0;
  ASSERT(kvm_getprocs(kd, 0, 0, &n) != NULL);
  ASSERT_EQ(1, n);

  /* Snapshot ok, 6 processes (> cap=4): realloc OOM then realloc success */
  g_win_proc_total = 6;
  g_kvm_oom_countdown = 2;
  kp = kvm_getprocs(kd, 0, 0, &n);
  ASSERT(kp != NULL);
  ASSERT_EQ(4, n);

  kp = kvm_getprocs(kd, 0, 0, &n);
  ASSERT(kp != NULL);
  ASSERT_EQ(6, n);

  bsd_kvm_procstat_set_alloc_hooks(NULL, NULL, NULL, NULL);
  kp = kvm_getprocs(kd, 0, 0, &n);
  ASSERT(kp != NULL);
  ASSERT_EQ(6, n);

  bsd_kvm_procstat_set_win32_hooks(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#else
  g_kvm_oom_countdown = 1;
  ASSERT(kvm_getprocs(kd, 0, 0, &n) == NULL);
  ASSERT_EQ(0, n);
  bsd_kvm_procstat_set_alloc_hooks(NULL, NULL, NULL, NULL);
#endif

  kp = kvm_getprocs(kd, 0, 0, &n);
  ASSERT(kp != NULL);
  ASSERT(n >= 1);
  kp = kvm_getprocs(kd, 0, 0, &n);
  ASSERT(kp != NULL);

  /* argv, envv, swapinfo */
  ASSERT(kvm_getargv(NULL, kp, 0) == NULL);
  ASSERT(kvm_getargv(kd, NULL, 0) == NULL);
  ASSERT(kvm_getargv(kd, kp, 0) != NULL);

  ASSERT(kvm_getenvv(NULL, kp, 0) == NULL);
  ASSERT(kvm_getenvv(kd, NULL, 0) == NULL);
  ASSERT(kvm_getenvv(kd, kp, 0) != NULL);

  ASSERT_EQ(0, kvm_getswapinfo(kd, NULL, 1, 0));
  ASSERT_EQ(0, kvm_getswapinfo(kd, &sw, 0, 0));
  ASSERT_EQ(1, kvm_getswapinfo(kd, &sw, 1, 0));
  ASSERT_STR_EQ("swap0", sw.ksw_devname);

  /* nlist, kvm_nlist, kvm_nlist2 */
  ASSERT_EQ(-1, nlist(NULL, NULL));
  memset(nl_empty, 0, sizeof(nl_empty));
  nl_empty[0].n_name = "_hz";
  nl_empty[1].n_name = "";
  ASSERT_EQ(0, kvm_nlist(kd, nl_empty));

  memset(nl_null, 0, sizeof(nl_null));
  nl_null[0].n_name = "_tick";
  nl_null[1].n_name = NULL;
  ASSERT_EQ(0, kvm_nlist2(kd, nl_null));

  /* kvm_read, kvm_read2, kvm_write, kvm_getcptime */
  ASSERT_EQ((ssize_t)-1, kvm_read(kd, 0UL, NULL, sizeof(readbuf)));
  ASSERT_EQ((ssize_t)sizeof(readbuf),
            kvm_read(kd, 0UL, readbuf, sizeof(readbuf)));
  ASSERT_EQ((ssize_t)sizeof(readbuf),
            kvm_read2(kd, (uint64_t)0, readbuf, sizeof(readbuf)));
  ASSERT_EQ((ssize_t)-1, kvm_write(kd, 0UL, readbuf, sizeof(readbuf)));

  ASSERT_EQ(-1, kvm_getcptime(kd, NULL));
  ASSERT_EQ(0, kvm_getcptime(kd, cp_time));
  ASSERT_EQ(100L, cp_time[0]);

  ASSERT_EQ(0, kvm_close(kd));

  /* kinfo_* functions & OOMs */
  ASSERT(kinfo_getallproc(NULL) == NULL);
  bsd_kvm_procstat_set_alloc_hooks(mock_kvm_malloc, mock_kvm_calloc,
                                   mock_kvm_realloc, mock_kvm_strdup);
  g_kvm_oom_countdown = 1;
  ASSERT(kinfo_getallproc(&n) == NULL);
#if defined(_WIN32)
  bsd_kvm_procstat_set_win32_hooks(mock_win_snap, mock_win_proc_step,
                                   mock_win_close_handle, NULL, NULL, NULL,
                                   NULL);
  g_win_snap_fail = 1;
#endif
  g_kvm_oom_countdown = 2;
  ASSERT(kinfo_getallproc(&n) == NULL);
  g_kvm_oom_countdown = 3;
  ASSERT(kinfo_getallproc(&n) == NULL);
#if defined(_WIN32)
  g_win_snap_fail = 0;
  bsd_kvm_procstat_set_win32_hooks(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#endif
  g_kvm_oom_countdown = 1;
  ASSERT(kinfo_getproc((pid_t)1) == NULL);
  bsd_kvm_procstat_set_alloc_hooks(NULL, NULL, NULL, NULL);

  kp = kinfo_getallproc(&n);
  ASSERT(kp != NULL);
  ASSERT(n >= 1);
  free(kp);

  kp = kinfo_getproc((pid_t)42);
  ASSERT(kp != NULL);
  ASSERT_EQ((pid_t)42, kp->ki_pid);
  free(kp);

  ASSERT(kinfo_getfile((pid_t)1, NULL) == NULL);
  ASSERT(kinfo_getfile((pid_t)1, &n) == NULL);
  ASSERT_EQ(0, n);
  ASSERT(kinfo_getvmmap((pid_t)1, NULL) == NULL);
  ASSERT(kinfo_getvmmap((pid_t)1, &n) == NULL);
  ASSERT_EQ(0, n);

  PASS();
}

TEST test_bsd_procstat_and_memstat(void) {
  struct procstat *ps;
  struct procstat *ps_nokd;
  struct kinfo_proc *kp;
  struct filestat_list *flist;
  char pathbuf[64];
  char errbuf[64];
  unsigned int cnt = 0;
  unsigned short mask = 0;
  int osrel = 0;
  struct vnstat vn;
  struct sockstat sock;
  struct pipestat pipe_st;
  struct ptsstat pts;
  struct semstat sem;
  struct shmstat shm;
  struct statinfo dstats;
  int nsel = 1;
  int nsels = 1;
  struct memory_type_list *mtl;
  struct memory_type *mtp;
  void *dummy_alloc;

  procstat_close(NULL);

  bsd_kvm_procstat_set_alloc_hooks(mock_kvm_malloc, mock_kvm_calloc,
                                   mock_kvm_realloc, mock_kvm_strdup);
  g_kvm_oom_countdown = 1;
  ASSERT(procstat_open_sysctl() == NULL);

  /* OOM on internal kvm_openfiles so ps->kd == NULL */
  g_kvm_oom_countdown = 2;
  ps_nokd = procstat_open_kvm(NULL, NULL);
  ASSERT(ps_nokd != NULL);
  ASSERT(procstat_getprocs(ps_nokd, 0, 0, &cnt) == NULL);
  procstat_close(ps_nokd);

  ps = procstat_open_core(NULL);
  ASSERT(ps != NULL);

  g_kvm_oom_countdown = 1;
  ASSERT(procstat_getfiles(ps, NULL, 0) == NULL);
  g_kvm_oom_countdown = 1;
  ASSERT(memstat_mtl_alloc() == NULL);
  bsd_kvm_procstat_set_alloc_hooks(NULL, NULL, NULL, NULL);

  ASSERT(procstat_getprocs(NULL, 0, 0, &cnt) == NULL);
  ASSERT(procstat_getprocs(ps, 0, 0, NULL) == NULL);
  kp = procstat_getprocs(ps, 0, 0, &cnt);
  ASSERT(kp != NULL);
  ASSERT(cnt >= 1U);

  flist = procstat_getfiles(ps, kp, 0);
  ASSERT(flist != NULL);
  procstat_freefiles(ps, NULL);
  procstat_freefiles(ps, flist);

  ASSERT(procstat_getvmmap(ps, kp, NULL) == NULL);
  ASSERT(procstat_getvmmap(ps, kp, &cnt) == NULL);
  procstat_freevmmap(ps, NULL);
  dummy_alloc = malloc(8);
  procstat_freevmmap(ps, (struct kinfo_vmentry *)dummy_alloc);

  ASSERT(procstat_getargv(NULL, kp, 0) == NULL);
  ASSERT(procstat_getargv(ps, kp, 0) != NULL);
  procstat_freeargv(ps);

  ASSERT(procstat_getenvv(NULL, kp, 0) == NULL);
  ASSERT(procstat_getenvv(ps, kp, 0) != NULL);
  procstat_freeenvv(ps);

  ASSERT_EQ(-1, procstat_getpathname(ps, NULL, pathbuf, sizeof(pathbuf)));
  ASSERT_EQ(-1, procstat_getpathname(ps, kp, NULL, sizeof(pathbuf)));
  ASSERT_EQ(-1, procstat_getpathname(ps, kp, pathbuf, 0));
  ASSERT_EQ(0, procstat_getpathname(ps, kp, pathbuf, sizeof(pathbuf)));

  ASSERT_EQ(-1, procstat_getrlimit(ps, kp, 0, NULL));
  ASSERT_EQ(0, procstat_getumask(ps, kp, NULL));
  ASSERT_EQ(0, procstat_getumask(ps, kp, &mask));
  ASSERT_EQ((unsigned short)022, mask);

  ASSERT(procstat_getgroups(ps, NULL, &cnt) == NULL);
  ASSERT(procstat_getgroups(ps, kp, NULL) == NULL);
  ASSERT(procstat_getgroups(ps, kp, &cnt) != NULL);
  procstat_freegroups(ps, NULL);

  ASSERT_EQ(0, procstat_getosrel(ps, kp, NULL));
  ASSERT_EQ(0, procstat_getosrel(ps, kp, &osrel));
#if defined(__I86__)
  ASSERT_EQ(15000, osrel);
#else
  ASSERT_EQ(1500000, osrel);
#endif

  ASSERT(procstat_getkstack(ps, kp, NULL) == NULL);
  ASSERT(procstat_getkstack(ps, kp, &cnt) == NULL);
  procstat_freekstack(ps, NULL);
  dummy_alloc = malloc(8);
  procstat_freekstack(ps, (struct kinfo_kstack *)dummy_alloc);

  ASSERT_EQ(0, procstat_get_vnode_info(ps, NULL, NULL, errbuf));
  ASSERT_EQ(0, procstat_get_vnode_info(ps, NULL, &vn, errbuf));
  ASSERT_EQ(0, procstat_get_socket_info(ps, NULL, NULL, errbuf));
  ASSERT_EQ(0, procstat_get_socket_info(ps, NULL, &sock, errbuf));
  ASSERT_EQ(0, procstat_get_pipe_info(ps, NULL, NULL, errbuf));
  ASSERT_EQ(0, procstat_get_pipe_info(ps, NULL, &pipe_st, errbuf));
  ASSERT_EQ(0, procstat_get_pts_info(ps, NULL, NULL, errbuf));
  ASSERT_EQ(0, procstat_get_pts_info(ps, NULL, &pts, errbuf));
  ASSERT_EQ(0, procstat_get_sem_info(ps, NULL, NULL, errbuf));
  ASSERT_EQ(0, procstat_get_sem_info(ps, NULL, &sem, errbuf));
  ASSERT_EQ(0, procstat_get_shm_info(ps, NULL, NULL, errbuf));
  ASSERT_EQ(0, procstat_get_shm_info(ps, NULL, &shm, errbuf));

  ASSERT(procstat_getptlwpinfo(ps, NULL) == NULL);
  ASSERT(procstat_getptlwpinfo(ps, &cnt) == NULL);
  procstat_freeptlwpinfo(ps, NULL);

  ASSERT(procstat_getrlimitusage(ps, kp, NULL) == NULL);
  ASSERT(procstat_getrlimitusage(ps, kp, &cnt) == NULL);
  procstat_freerlimitusage(ps, NULL);

  ASSERT(procstat_get_kqueue_info(ps, kp, 0, NULL, NULL) == NULL);
  errbuf[0] = 'x';
  ASSERT(procstat_get_kqueue_info(ps, kp, 0, &cnt, errbuf) == NULL);
  ASSERT_EQ('\0', errbuf[0]);
  procstat_freekqinfo(ps, NULL);

  ASSERT(procstat_getauxv(ps, kp, NULL) == NULL);
  ASSERT(procstat_getauxv(ps, kp, &cnt) == NULL);
  procstat_freeauxv(ps, NULL);

  ASSERT(procstat_getadvlock(ps) == NULL);
  procstat_freeadvlock(ps, NULL);

  procstat_freeprocs(ps, kp);
  procstat_close(ps);

  /* devstat */
  ASSERT_EQ(0, devstat_checkversion(NULL));
  ASSERT_EQ(0, devstat_getnumdevs(NULL));
  ASSERT_EQ(0, devstat_getdevs(NULL, NULL));
  ASSERT_EQ(0, devstat_getdevs(NULL, &dstats));
  ASSERT_EQ(0, devstat_selectdevs(NULL, NULL, NULL, NULL, 0L, NULL, 0, NULL, 0,
                                  NULL, 0, 0, 0, 0));
  ASSERT_EQ(0, devstat_selectdevs(NULL, &nsel, &nsels, NULL, 0L, NULL, 0, NULL,
                                  0, NULL, 0, 0, 0, 0));
  ASSERT_EQ(0, nsel);
  ASSERT_EQ(0, nsels);
  ASSERT_EQ(0, devstat_compute_statistics(NULL, NULL, (long double)1.0));
  ASSERT(devstat_compute_etime(NULL, NULL) > (long double)0.5);

  /* memstat */
  ASSERT(memstat_mtl_first(NULL) == NULL);
  ASSERT(memstat_mtl_next(NULL) == NULL);
  ASSERT(memstat_mtl_find(NULL, 0, "x") == NULL);
  memstat_mtl_free(NULL);
  ASSERT_EQ(0, memstat_mtl_geterror(NULL));
  ASSERT_EQ(0, memstat_sysctl_all(NULL, 0));
  ASSERT_EQ(0, memstat_sysctl_uma(NULL, 0));
  ASSERT_EQ(0, memstat_sysctl_malloc(NULL, 0));
  ASSERT_EQ(0, memstat_kvm_all(NULL, NULL));
  ASSERT_EQ(0, memstat_kvm_uma(NULL, NULL));
  ASSERT_EQ(0, memstat_kvm_malloc(NULL, NULL));

  mtl = memstat_mtl_alloc();
  ASSERT(mtl != NULL);
  ASSERT_EQ(0, memstat_mtl_geterror(mtl));
  ASSERT_EQ(0, memstat_sysctl_all(mtl, 0));
#if defined(_WIN32)
  /* Exercise Win32 awm_query_phys_avail_bytes branches (OOM and fallbacks) */
  bsd_kvm_procstat_set_alloc_hooks(mock_kvm_malloc, mock_kvm_calloc,
                                   mock_kvm_realloc, mock_kvm_strdup);
  bsd_kvm_procstat_set_win32_hooks(NULL, NULL, NULL, mock_win_tbl_insuf,
                                   mock_win_tbl_insuf, mock_win_tbl_insuf,
                                   mock_win_mem_fail);
  g_kvm_oom_countdown = 1;
  ASSERT_EQ(0, memstat_sysctl_all(mtl, 0));
  g_kvm_oom_countdown = 2;
  ASSERT_EQ(0, memstat_sysctl_all(mtl, 0));
  g_kvm_oom_countdown = 3;
  ASSERT_EQ(0, memstat_sysctl_all(mtl, 0));

  bsd_kvm_procstat_set_win32_hooks(NULL, NULL, NULL, mock_win_tbl_ok,
                                   mock_win_tbl_ok, mock_win_tbl_ok,
                                   mock_win_mem_fail);
  ASSERT_EQ(0, memstat_sysctl_all(mtl, 0));
  bsd_kvm_procstat_set_win32_hooks(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  bsd_kvm_procstat_set_alloc_hooks(NULL, NULL, NULL, NULL);
#endif
  mtp = memstat_mtl_first(mtl);
  ASSERT(mtp != NULL);
  ASSERT_STR_EQ("", memstat_get_name(NULL));
  ASSERT(memstat_get_name(mtp) != NULL);
  ASSERT_EQ(0, memstat_get_allocator(mtp));
  ASSERT_EQ((uint64_t)0U, memstat_get_countlimit(mtp));
  ASSERT_EQ((uint64_t)0U, memstat_get_byteslimit(mtp));
  ASSERT_EQ((uint64_t)0U, memstat_get_sizemask(mtp));
  ASSERT_EQ((uint64_t)0U, memstat_get_size(mtp));
  ASSERT_EQ((uint64_t)0U, memstat_get_memalloced(mtp));
  ASSERT_EQ((uint64_t)0U, memstat_get_memfreed(mtp));
  ASSERT_EQ((uint64_t)0U, memstat_get_numallocs(mtp));
  ASSERT_EQ((uint64_t)0U, memstat_get_numfrees(mtp));
  ASSERT_EQ((uint64_t)0U, memstat_get_bytes(NULL));
  ASSERT(memstat_get_bytes(mtp) > (uint64_t)0U);
  ASSERT_EQ((uint64_t)0U, memstat_get_count(NULL));
  ASSERT_EQ((uint64_t)1U, memstat_get_count(mtp));
  ASSERT_EQ((uint64_t)0U, memstat_get_free(mtp));
  ASSERT_EQ((uint64_t)0U, memstat_get_failures(mtp));
  memstat_mtl_free(mtl);

  PASS();
}

TEST test_bsd_mount_jail_rctl_acl_extattr(void) {
  int data = 1;
  struct statfs sfs;
  fhandle_t fh;
  acl_entry_t entry = NULL;
  acl_tag_t tag = 1;
  int trivial = 0;
  void *acl_dummy;
  char *ns_str = NULL;
  int ns = -1;
#if !defined(__APPLE__)
  struct statfs *mntp = NULL;
  struct xvfsconf vfc;
#endif
#if defined(__linux__)
  struct awm_bsd_statfs mnt_sfs;
  int mnt_fd;
#endif

  ASSERT_EQ(0, procctl(P_PID, 0, PROC_SPROTECT, NULL));
  ASSERT_EQ(0, procctl(P_PID, 0, PROC_SPROTECT, &data));
  ASSERT_EQ(0, data);

#if defined(_WIN32)
  ASSERT_EQ(-1, ptrace(PT_TRACE_ME, (pid_t)0, (caddr_t)0, 0));
  ASSERT(getfsent() == NULL);
  ASSERT(getfsspec("/dev/sda1") == NULL);
  ASSERT(getfsfile("/") == NULL);
  ASSERT_EQ(1, setfsent());
  endfsent();
#endif

  memset(&fh, 0, sizeof(fh));
  ASSERT_EQ(0, fhstatfs(&fh, &sfs));

#if !defined(__APPLE__)
  ASSERT_EQ(0, getmntinfo(NULL, MNT_NOWAIT));
  ASSERT_EQ(1, getmntinfo(&mntp, MNT_NOWAIT));
  ASSERT(mntp != NULL);
  ASSERT_EQ(1, getfsstat(NULL, 0L, MNT_NOWAIT));
  ASSERT_EQ(0, getfsstat(&sfs, 1L, MNT_NOWAIT));
  ASSERT_EQ(1, getfsstat(&sfs, (long)sizeof(sfs), MNT_NOWAIT));
  ASSERT_EQ(-1, quotactl(".", 0, 0, NULL));
  ASSERT_EQ(-1, getvfsbyname("ufs", &vfc));
#endif

#if defined(__linux__)
  ASSERT_EQ(-1, awm_bsd_statfs(".", NULL));
  ASSERT_EQ(0, awm_bsd_statfs(NULL, &mnt_sfs));
  ASSERT_EQ(0, awm_bsd_statfs(".", &mnt_sfs));
  ASSERT_STR_EQ("ext4", mnt_sfs.f_fstypename);
  ASSERT_EQ(-1, awm_bsd_statfs("/nonexistent_awm_bsd_mount_99999", &mnt_sfs));

  ASSERT_EQ(-1, awm_bsd_fstatfs(0, NULL));
  ASSERT_EQ(-1, awm_bsd_fstatfs(-1, &mnt_sfs));
  mnt_fd = open(".", O_RDONLY);
  ASSERT(mnt_fd >= 0);
  ASSERT_EQ(0, awm_bsd_fstatfs(mnt_fd, &mnt_sfs));
  close(mnt_fd);
#endif

  /* jail & rctl */
  ASSERT_EQ(-1, jail_getid("j0"));
  ASSERT(jail_getname(0) == NULL);
  ASSERT_EQ(-1, jail_getv(0));
  ASSERT_EQ(-1, jail_setv(0));
  ASSERT_EQ(-1, jail_get(NULL, 0U, 0));
  ASSERT_EQ(-1, jail_set(NULL, 0U, 0));
  ASSERT_EQ(-1, jail_attach(0));
  ASSERT_EQ(-1, jail_remove(0));

  ASSERT_EQ(-1, rctl_get_racct(NULL, 0, NULL, 0));
  ASSERT_EQ(-1, rctl_get_rules(NULL, 0, NULL, 0));
  ASSERT_EQ(-1, rctl_get_limits(NULL, 0, NULL, 0));
  ASSERT_EQ(-1, rctl_add_rule(NULL, 0, NULL, 0));
  ASSERT_EQ(-1, rctl_remove_rule(NULL, 0, NULL, 0));

  /* acl */
  ASSERT(acl_get_file(".", ACL_TYPE_ACCESS) == NULL);
  ASSERT(acl_get_link_np(".", ACL_TYPE_ACCESS) == NULL);
  ASSERT(acl_get_fd(-1) == NULL);
  ASSERT(acl_get_fd_np(-1, ACL_TYPE_ACCESS) == NULL);
  ASSERT_EQ(0, acl_get_entry(NULL, ACL_FIRST_ENTRY, NULL));
  ASSERT_EQ(0, acl_get_entry(NULL, ACL_FIRST_ENTRY, &entry));
  ASSERT_EQ(-1, acl_get_tag_type(NULL, NULL));
  ASSERT_EQ(-1, acl_get_tag_type(NULL, &tag));
  ASSERT_EQ(0, tag);
  ASSERT_EQ(0, acl_free(NULL));
  acl_dummy = malloc(4);
  ASSERT_EQ(0, acl_free(acl_dummy));
  ASSERT_EQ(0, acl_is_trivial_np(NULL, NULL));
  ASSERT_EQ(0, acl_is_trivial_np(NULL, &trivial));
  ASSERT_EQ(1, trivial);

  /* extattr */
  ASSERT_EQ((ssize_t)-1,
            extattr_get_fd(-1, EXTATTR_NAMESPACE_USER, "a", NULL, 0));
  ASSERT_EQ((ssize_t)-1,
            extattr_get_file(".", EXTATTR_NAMESPACE_USER, "a", NULL, 0));
  ASSERT_EQ((ssize_t)-1,
            extattr_get_link(".", EXTATTR_NAMESPACE_USER, "a", NULL, 0));
  ASSERT_EQ((ssize_t)-1, extattr_list_fd(-1, EXTATTR_NAMESPACE_USER, NULL, 0));
  ASSERT_EQ((ssize_t)-1,
            extattr_list_file(".", EXTATTR_NAMESPACE_USER, NULL, 0));
  ASSERT_EQ((ssize_t)-1,
            extattr_list_link(".", EXTATTR_NAMESPACE_USER, NULL, 0));
  ASSERT_EQ(-1, extattr_set_fd(-1, EXTATTR_NAMESPACE_USER, "a", NULL, 0));
  ASSERT_EQ(-1, extattr_set_file(".", EXTATTR_NAMESPACE_USER, "a", NULL, 0));
  ASSERT_EQ(-1, extattr_set_link(".", EXTATTR_NAMESPACE_USER, "a", NULL, 0));
  ASSERT_EQ(-1, extattr_delete_fd(-1, EXTATTR_NAMESPACE_USER, "a"));
  ASSERT_EQ(-1, extattr_delete_file(".", EXTATTR_NAMESPACE_USER, "a"));
  ASSERT_EQ(-1, extattr_delete_link(".", EXTATTR_NAMESPACE_USER, "a"));

  ASSERT_EQ(-1, extattr_namespace_to_string(EXTATTR_NAMESPACE_USER, NULL));
  bsd_kvm_procstat_set_alloc_hooks(mock_kvm_malloc, mock_kvm_calloc,
                                   mock_kvm_realloc, mock_kvm_strdup);
  g_kvm_oom_countdown = 1;
  ASSERT_EQ(-1, extattr_namespace_to_string(EXTATTR_NAMESPACE_USER, &ns_str));
  bsd_kvm_procstat_set_alloc_hooks(NULL, NULL, NULL, NULL);

  ASSERT_EQ(0, extattr_namespace_to_string(EXTATTR_NAMESPACE_SYSTEM, &ns_str));
  ASSERT_STR_EQ("system", ns_str);
  free(ns_str);
  ASSERT_EQ(0, extattr_namespace_to_string(EXTATTR_NAMESPACE_USER, &ns_str));
  ASSERT_STR_EQ("user", ns_str);
  free(ns_str);

  ASSERT_EQ(-1, extattr_string_to_namespace(NULL, &ns));
  ASSERT_EQ(-1, extattr_string_to_namespace("user", NULL));
  ASSERT_EQ(0, extattr_string_to_namespace("system", &ns));
  ASSERT_EQ(EXTATTR_NAMESPACE_SYSTEM, ns);
  ASSERT_EQ(0, extattr_string_to_namespace("user", &ns));
  ASSERT_EQ(EXTATTR_NAMESPACE_USER, ns);

  PASS();
}

SUITE(suite_bsd_kvm_procstat_core) {
  RUN_TEST(test_bsd_kvm_procstat);
  RUN_TEST(test_bsd_procstat_and_memstat);
  RUN_TEST(test_bsd_mount_jail_rctl_acl_extattr);
}
