#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)
/**
 * @file bsd_kvm_procstat.c
 * @brief Implementation of FreeBSD KVM, libprocstat, devstat, memstat, mount,
 * jail, rctl, acl, and extattr shims.
 */

/* clang-format off */
#include <kvm.h>
#include <libprocstat.h>
#include <libutil.h>
#include <sys/procctl.h>
#include <sys/ptrace.h>
#include <sys/vmmeter.h>
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <process.h>
#include <winsock2.h>
#include <tlhelp32.h>
#ifdef PROCESSENTRY32
#undef PROCESSENTRY32
#endif
#ifdef Process32First
#undef Process32First
#endif
#ifdef Process32Next
#undef Process32Next
#endif
#else
#include <unistd.h>
#endif
/* clang-format on */

struct __kvm {
  char errbuf[256];
  struct kinfo_proc *procs;
  int nprocs;
  char *argv_buf[2];
};

struct procstat {
  int type;
  kvm_t *kd;
};

struct memory_type {
  char name[MEMSTAT_MAXNAME];
  uint64_t bytes;
  uint64_t count;
};

struct memory_type_list {
  struct memory_type item;
  int error;
};

static void fill_single_kinfo_proc(struct kinfo_proc *kp, pid_t pid,
                                   const char *comm) {
  memset(kp, 0, sizeof(*kp));
  kp->ki_structsize = (int)sizeof(*kp);
  kp->ki_pid = pid;
  kp->ki_ppid = 1;
  kp->ki_pgid = pid;
  kp->ki_sid = pid;
  kp->ki_stat = SRUN;
  kp->ki_numthreads = 1;
  kp->ki_size = 4096 * 1024;
  kp->ki_rssize = 1024;
#if defined(_MSC_VER)
  strncpy_s(kp->ki_comm, sizeof(kp->ki_comm), comm ? comm : "proc", _TRUNCATE);
  strncpy_s(kp->ki_login, sizeof(kp->ki_login), "root", _TRUNCATE);
#else
  strncpy(kp->ki_comm, comm ? comm : "proc", sizeof(kp->ki_comm) - 1);
  strncpy(kp->ki_login, "root", sizeof(kp->ki_login) - 1);
#endif
}

kvm_t *kvm_openfiles(const char *uf, const char *mf, const char *sf, int flag,
                     char *errbuf) {
  kvm_t *kd = (kvm_t *)calloc(1, sizeof(*kd));
  (void)uf;
  (void)mf;
  (void)sf;
  (void)flag;
  if (kd == NULL) {
    if (errbuf != NULL) {
#if defined(_MSC_VER)
      strcpy_s(errbuf, 256, "out of memory");
#else
      strcpy(errbuf, "out of memory");
#endif
    }
    return NULL;
  }
  return kd;
}

kvm_t *kvm_open(const char *uf, const char *mf, const char *sf, int flag,
                const char *errstr) {
  (void)errstr;
  return kvm_openfiles(uf, mf, sf, flag, NULL);
}

kvm_t *kvm_open2(const char *uf, const char *mf, int flag, char *errbuf,
                 int (*resolver)(const char *, uint64_t *)) {
  (void)resolver;
  return kvm_openfiles(uf, mf, NULL, flag, errbuf);
}

int kvm_close(kvm_t *kd) {
  if (kd == NULL) {
    return -1;
  }
  if (kd->procs != NULL) {
    free(kd->procs);
  }
  free(kd);
  return 0;
}

struct kinfo_proc *kvm_getprocs(kvm_t *kd, int op, int arg, int *cnt) {
  (void)op;
  (void)arg;
  if (kd == NULL || cnt == NULL) {
    return NULL;
  }
  if (kd->procs != NULL) {
    free(kd->procs);
    kd->procs = NULL;
  }
#if defined(_WIN32)
  {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap != INVALID_HANDLE_VALUE) {
      PROCESSENTRY32 pe;
      int cap = 64, n = 0;
      kd->procs = (struct kinfo_proc *)calloc((size_t)cap, sizeof(*kd->procs));
      pe.dwSize = sizeof(pe);
      if (kd->procs != NULL && Process32First(snap, &pe)) {
        do {
          if (n >= cap) {
            int ncap = cap * 2;
            struct kinfo_proc *np = (struct kinfo_proc *)realloc(
                kd->procs, (size_t)ncap * sizeof(*np));
            if (np == NULL) {
              break;
            }
            kd->procs = np;
            cap = ncap;
          }
          fill_single_kinfo_proc(&kd->procs[n], (pid_t)pe.th32ProcessID,
                                 pe.szExeFile);
          kd->procs[n].ki_ppid = (pid_t)pe.th32ParentProcessID;
          kd->procs[n].ki_numthreads = (int)pe.cntThreads;
          n++;
        } while (Process32Next(snap, &pe));
      }
      CloseHandle(snap);
      if (n > 0) {
        kd->nprocs = n;
        *cnt = n;
        return kd->procs;
      }
    }
  }
#endif
  kd->procs = (struct kinfo_proc *)calloc(1, sizeof(*kd->procs));
  if (kd->procs == NULL) {
    *cnt = 0;
    return NULL;
  }
#if defined(_WIN32)
  fill_single_kinfo_proc(&kd->procs[0], (pid_t)_getpid(), "app.exe");
#else
  fill_single_kinfo_proc(&kd->procs[0], (pid_t)getpid(), "app");
#endif
  kd->nprocs = 1;
  *cnt = 1;
  return kd->procs;
}

char **kvm_getargv(kvm_t *kd, const struct kinfo_proc *p, int nchr) {
  (void)nchr;
  if (kd == NULL || p == NULL) {
    return NULL;
  }
  kd->argv_buf[0] = (char *)p->ki_comm;
  kd->argv_buf[1] = NULL;
  return kd->argv_buf;
}

char **kvm_getenvv(kvm_t *kd, const struct kinfo_proc *p, int nchr) {
  (void)nchr;
  if (kd == NULL || p == NULL) {
    return NULL;
  }
  kd->argv_buf[0] = NULL;
  return kd->argv_buf;
}

int kvm_getswapinfo(kvm_t *kd, struct kvm_swap *swap_ary, int swap_max,
                    int flags) {
  (void)kd;
  (void)flags;
  if (swap_ary == NULL || swap_max <= 0) {
    return 0;
  }
  memset(&swap_ary[0], 0, sizeof(swap_ary[0]));
#if defined(_MSC_VER)
  strcpy_s(swap_ary[0].ksw_devname, sizeof(swap_ary[0].ksw_devname), "swap0");
#else
  strcpy(swap_ary[0].ksw_devname, "swap0");
#endif
  swap_ary[0].ksw_total = 1024;
  swap_ary[0].ksw_used = 64;
  return 1;
}

int nlist(const char *filename, struct nlist *nl) {
  (void)filename;
  if (nl == NULL) {
    return -1;
  }
  while (nl->n_name != NULL && nl->n_name[0] != '\0') {
    nl->n_value = 0;
    nl->n_type = 0;
    nl++;
  }
  return 0;
}

int kvm_nlist(kvm_t *kd, struct nlist *nl) {
  (void)kd;
  return nlist(NULL, nl);
}

int kvm_nlist2(kvm_t *kd, struct nlist *nl) { return kvm_nlist(kd, nl); }

ssize_t kvm_read(kvm_t *kd, unsigned long addr, void *buf, size_t nbytes) {
  (void)kd;
  (void)addr;
  if (buf == NULL) {
    return -1;
  }
  memset(buf, 0, nbytes);
  return (ssize_t)nbytes;
}

ssize_t kvm_read2(kvm_t *kd, uint64_t addr, void *buf, size_t nbytes) {
  return kvm_read(kd, (unsigned long)addr, buf, nbytes);
}

ssize_t kvm_write(kvm_t *kd, unsigned long addr, const void *buf,
                  size_t nbytes) {
  (void)kd;
  (void)addr;
  (void)buf;
  (void)nbytes;
  errno = EPERM;
  return -1;
}

char *kvm_geterr(kvm_t *kd) { return (kd != NULL) ? kd->errbuf : "kvm error"; }

int kvm_getcptime(kvm_t *kd, long *cp_time) {
  (void)kd;
  if (cp_time == NULL) {
    return -1;
  }
  cp_time[0] = 100;
  cp_time[1] = 0;
  cp_time[2] = 50;
  cp_time[3] = 0;
  cp_time[4] = 850;
  return 0;
}

struct kinfo_proc *kinfo_getallproc(int *cntp) {
  kvm_t *kd;
  struct kinfo_proc *procs, *copy = NULL;
  int n = 0;
  if (cntp == NULL) {
    return NULL;
  }
  *cntp = 0;
  kd = kvm_openfiles(NULL, NULL, NULL, 0, NULL);
  if (kd == NULL) {
    return NULL;
  }
  procs = kvm_getprocs(kd, 0, 0, &n);
  if (procs != NULL && n > 0) {
    copy = (struct kinfo_proc *)malloc((size_t)n * sizeof(*copy));
    if (copy != NULL) {
      memcpy(copy, procs, (size_t)n * sizeof(*copy));
      *cntp = n;
    }
  }
  kvm_close(kd);
  return copy;
}

struct kinfo_proc *kinfo_getproc(pid_t pid) {
  struct kinfo_proc *kp = (struct kinfo_proc *)calloc(1, sizeof(*kp));
  if (kp != NULL) {
    fill_single_kinfo_proc(kp, pid, "proc");
  }
  return kp;
}

struct kinfo_file *kinfo_getfile(pid_t pid, int *cntp) {
  (void)pid;
  if (cntp != NULL) {
    *cntp = 0;
  }
  return NULL;
}

struct kinfo_vmentry *kinfo_getvmmap(pid_t pid, int *cntp) {
  (void)pid;
  if (cntp != NULL) {
    *cntp = 0;
  }
  return NULL;
}

struct procstat *procstat_open_sysctl(void) {
  struct procstat *ps = (struct procstat *)calloc(1, sizeof(*ps));
  if (ps != NULL) {
    ps->kd = kvm_openfiles(NULL, NULL, NULL, 0, NULL);
  }
  return ps;
}

struct procstat *procstat_open_kvm(const char *nlistf, const char *memf) {
  (void)nlistf;
  (void)memf;
  return procstat_open_sysctl();
}

struct procstat *procstat_open_core(const char *filename) {
  (void)filename;
  return procstat_open_sysctl();
}

void procstat_close(struct procstat *prstat) {
  if (prstat != NULL) {
    if (prstat->kd != NULL) {
      kvm_close(prstat->kd);
    }
    free(prstat);
  }
}

struct kinfo_proc *procstat_getprocs(struct procstat *prstat, int what, int arg,
                                     unsigned int *count) {
  int cnt = 0;
  struct kinfo_proc *p;
  if (prstat == NULL || prstat->kd == NULL || count == NULL) {
    return NULL;
  }
  p = kvm_getprocs(prstat->kd, what, arg, &cnt);
  *count = (unsigned int)cnt;
  return p;
}

void procstat_freeprocs(struct procstat *prstat, struct kinfo_proc *p) {
  (void)prstat;
  (void)p;
}

struct filestat_list *procstat_getfiles(struct procstat *prstat,
                                        struct kinfo_proc *kp, int mmapped) {
  struct filestat_list *head;
  (void)prstat;
  (void)kp;
  (void)mmapped;
  head = (struct filestat_list *)calloc(1, sizeof(*head));
  if (head != NULL) {
    STAILQ_INIT(head);
  }
  return head;
}

void procstat_freefiles(struct procstat *prstat, struct filestat_list *head) {
  (void)prstat;
  if (head != NULL) {
    free(head);
  }
}

struct kinfo_vmentry *procstat_getvmmap(struct procstat *prstat,
                                        struct kinfo_proc *kp,
                                        unsigned int *count) {
  (void)prstat;
  (void)kp;
  if (count != NULL) {
    *count = 0;
  }
  return NULL;
}

void procstat_freevmmap(struct procstat *prstat, struct kinfo_vmentry *vmmap) {
  (void)prstat;
  if (vmmap != NULL) {
    free(vmmap);
  }
}

char **procstat_getargv(struct procstat *prstat, struct kinfo_proc *p,
                        size_t nchr) {
  return (prstat != NULL) ? kvm_getargv(prstat->kd, p, (int)nchr) : NULL;
}

void procstat_freeargv(struct procstat *prstat) { (void)prstat; }

char **procstat_getenvv(struct procstat *prstat, struct kinfo_proc *p,
                        size_t nchr) {
  return (prstat != NULL) ? kvm_getenvv(prstat->kd, p, (int)nchr) : NULL;
}

void procstat_freeenvv(struct procstat *prstat) { (void)prstat; }

int procstat_getpathname(struct procstat *prstat, struct kinfo_proc *kp,
                         char *pathname, size_t maxlen) {
  (void)prstat;
  if (kp == NULL || pathname == NULL || maxlen == 0) {
    return -1;
  }
#if defined(_MSC_VER)
  strncpy_s(pathname, maxlen, kp->ki_comm, _TRUNCATE);
#else
  strncpy(pathname, kp->ki_comm, maxlen - 1);
  pathname[maxlen - 1] = '\0';
#endif
  return 0;
}

int procstat_getrlimit(struct procstat *prstat, struct kinfo_proc *kp,
                       int which, struct rlimit *rlimit) {
  (void)prstat;
  (void)kp;
  (void)which;
  (void)rlimit;
  return -1;
}

int procstat_getumask(struct procstat *prstat, struct kinfo_proc *kp,
                      unsigned short *maskp) {
  (void)prstat;
  (void)kp;
  if (maskp != NULL) {
    *maskp = 022;
  }
  return 0;
}

gid_t *procstat_getgroups(struct procstat *prstat, struct kinfo_proc *kp,
                          unsigned int *count) {
  (void)prstat;
  if (kp == NULL || count == NULL) {
    return NULL;
  }
  *count = 1;
  return kp->ki_groups;
}

void procstat_freegroups(struct procstat *prstat, gid_t *groups) {
  (void)prstat;
  (void)groups;
}

int procstat_getosrel(struct procstat *prstat, struct kinfo_proc *kp,
                      int *osrelp) {
  (void)prstat;
  (void)kp;
  if (osrelp != NULL) {
    *osrelp = 1500000;
  }
  return 0;
}

struct kinfo_kstack *procstat_getkstack(struct procstat *prstat,
                                        struct kinfo_proc *kp,
                                        unsigned int *count) {
  (void)prstat;
  (void)kp;
  if (count != NULL) {
    *count = 0;
  }
  return NULL;
}

void procstat_freekstack(struct procstat *prstat, struct kinfo_kstack *kkstp) {
  (void)prstat;
  if (kkstp != NULL) {
    free(kkstp);
  }
}

int procstat_get_vnode_info(struct procstat *prstat, struct filestat *fst,
                            struct vnstat *vn, char *errbuf) {
  (void)prstat;
  (void)fst;
  (void)errbuf;
  if (vn != NULL) {
    memset(vn, 0, sizeof(*vn));
  }
  return 0;
}

int procstat_get_socket_info(struct procstat *prstat, struct filestat *fst,
                             struct sockstat *sock, char *errbuf) {
  (void)prstat;
  (void)fst;
  (void)errbuf;
  if (sock != NULL) {
    memset(sock, 0, sizeof(*sock));
  }
  return 0;
}

int procstat_get_pipe_info(struct procstat *prstat, struct filestat *fst,
                           struct pipestat *pipe, char *errbuf) {
  (void)prstat;
  (void)fst;
  (void)errbuf;
  if (pipe != NULL) {
    memset(pipe, 0, sizeof(*pipe));
  }
  return 0;
}

int procstat_get_pts_info(struct procstat *prstat, struct filestat *fst,
                          struct ptsstat *pts, char *errbuf) {
  (void)prstat;
  (void)fst;
  (void)errbuf;
  if (pts != NULL) {
    memset(pts, 0, sizeof(*pts));
  }
  return 0;
}

int procstat_get_sem_info(struct procstat *prstat, struct filestat *fst,
                          struct semstat *sem, char *errbuf) {
  (void)prstat;
  (void)fst;
  (void)errbuf;
  if (sem != NULL) {
    memset(sem, 0, sizeof(*sem));
  }
  return 0;
}

int procstat_get_shm_info(struct procstat *prstat, struct filestat *fst,
                          struct shmstat *shm, char *errbuf) {
  (void)prstat;
  (void)fst;
  (void)errbuf;
  if (shm != NULL) {
    memset(shm, 0, sizeof(*shm));
  }
  return 0;
}

int procctl(idtype_t idtype, id_t id, int com, void *data) {
  (void)idtype;
  (void)id;
  (void)com;
  if (data != NULL) {
    *(int *)data = 0;
  }
  return 0;
}

#if defined(_WIN32) || defined(__WATCOMC__) || defined(__DOS__) ||             \
    defined(__MSDOS__)
int ptrace(int request, pid_t pid, caddr_t addr, int data) {
  (void)request;
  (void)pid;
  (void)addr;
  (void)data;
  errno = ENOSYS;
  return -1;
}
#endif

int devstat_checkversion(void *kd) {
  (void)kd;
  return 0;
}

int devstat_getnumdevs(void *kd) {
  (void)kd;
  return 0;
}

int devstat_getdevs(void *kd, struct statinfo *stats) {
  (void)kd;
  if (stats != NULL) {
    memset(stats->cp_time, 0, sizeof(stats->cp_time));
  }
  return 0;
}

int devstat_selectdevs(struct device_selection **dev_select, int *num_selected,
                       int *num_selections, long *select_generation,
                       long current_generation, struct devstat *devices,
                       int numdevs, void *matches, int num_matches,
                       char **dev_selections, int num_dev_selections,
                       int select_mode, int maxshowdevs, int perf_select) {
  (void)dev_select;
  (void)select_generation;
  (void)current_generation;
  (void)devices;
  (void)numdevs;
  (void)matches;
  (void)num_matches;
  (void)dev_selections;
  (void)num_dev_selections;
  (void)select_mode;
  (void)maxshowdevs;
  (void)perf_select;
  if (num_selected) {
    *num_selected = 0;
  }
  if (num_selections) {
    *num_selections = 0;
  }
  return 0;
}

int devstat_compute_statistics(struct devstat *current,
                               struct devstat *previous, long double etime,
                               ...) {
  (void)current;
  (void)previous;
  (void)etime;
  return 0;
}

long double devstat_compute_etime(void *cur_time, void *prev_time) {
  (void)cur_time;
  (void)prev_time;
  return 1.0;
}

struct memory_type_list *memstat_mtl_alloc(void) {
  return (struct memory_type_list *)calloc(1, sizeof(struct memory_type_list));
}

struct memory_type *memstat_mtl_first(struct memory_type_list *list) {
  (void)list;
  return NULL;
}

struct memory_type *memstat_mtl_next(struct memory_type *mtp) {
  (void)mtp;
  return NULL;
}

struct memory_type *memstat_mtl_find(struct memory_type_list *list,
                                     int allocator, const char *name) {
  (void)list;
  (void)allocator;
  (void)name;
  return NULL;
}

void memstat_mtl_free(struct memory_type_list *list) {
  if (list != NULL) {
    free(list);
  }
}

int memstat_mtl_geterror(struct memory_type_list *list) {
  return (list != NULL) ? list->error : 0;
}

int memstat_sysctl_all(struct memory_type_list *list, int flags) {
  (void)list;
  (void)flags;
  return 0;
}

int memstat_sysctl_uma(struct memory_type_list *list, int flags) {
  (void)list;
  (void)flags;
  return 0;
}

int memstat_sysctl_malloc(struct memory_type_list *list, int flags) {
  (void)list;
  (void)flags;
  return 0;
}

int memstat_kvm_all(struct memory_type_list *list, void *kvm_handle) {
  (void)list;
  (void)kvm_handle;
  return 0;
}

int memstat_kvm_uma(struct memory_type_list *list, void *kvm_handle) {
  (void)list;
  (void)kvm_handle;
  return 0;
}

int memstat_kvm_malloc(struct memory_type_list *list, void *kvm_handle) {
  (void)list;
  (void)kvm_handle;
  return 0;
}

const char *memstat_get_name(const struct memory_type *mtp) {
  return (mtp != NULL) ? mtp->name : "";
}

int memstat_get_allocator(const struct memory_type *mtp) {
  (void)mtp;
  return 0;
}

uint64_t memstat_get_countlimit(const struct memory_type *mtp) {
  (void)mtp;
  return 0;
}
uint64_t memstat_get_byteslimit(const struct memory_type *mtp) {
  (void)mtp;
  return 0;
}
uint64_t memstat_get_sizemask(const struct memory_type *mtp) {
  (void)mtp;
  return 0;
}
uint64_t memstat_get_size(const struct memory_type *mtp) {
  (void)mtp;
  return 0;
}
uint64_t memstat_get_memalloced(const struct memory_type *mtp) {
  (void)mtp;
  return 0;
}
uint64_t memstat_get_memfreed(const struct memory_type *mtp) {
  (void)mtp;
  return 0;
}
uint64_t memstat_get_numallocs(const struct memory_type *mtp) {
  (void)mtp;
  return 0;
}
uint64_t memstat_get_numfrees(const struct memory_type *mtp) {
  (void)mtp;
  return 0;
}
uint64_t memstat_get_bytes(const struct memory_type *mtp) {
  return (mtp != NULL) ? mtp->bytes : 0;
}
uint64_t memstat_get_count(const struct memory_type *mtp) {
  return (mtp != NULL) ? mtp->count : 0;
}
uint64_t memstat_get_free(const struct memory_type *mtp) {
  (void)mtp;
  return 0;
}
uint64_t memstat_get_failures(const struct memory_type *mtp) {
  (void)mtp;
  return 0;
}

static struct statfs g_mnt_statfs;

int getmntinfo(struct statfs **mntbufp, int flags) {
  (void)flags;
  if (mntbufp == NULL) {
    return 0;
  }
  (void)statfs(".", &g_mnt_statfs);
  *mntbufp = &g_mnt_statfs;
  return 1;
}

int getfsstat(struct statfs *buf, long bufsize, int mode) {
  (void)mode;
  if (buf == NULL) {
    return 1;
  }
  if ((size_t)bufsize >= sizeof(struct statfs)) {
    (void)statfs(".", &buf[0]);
    return 1;
  }
  return 0;
}

int fhstatfs(const fhandle_t *fhp, struct statfs *buf) {
  (void)fhp;
  return statfs(".", buf);
}

#if defined(_WIN32) || defined(__WATCOMC__) || defined(__DOS__) ||             \
    defined(__MSDOS__)
struct fstab *getfsent(void) { return NULL; }
struct fstab *getfsspec(const char *spec) {
  (void)spec;
  return NULL;
}
struct fstab *getfsfile(const char *file) {
  (void)file;
  return NULL;
}
int setfsent(void) { return 1; }
void endfsent(void) {}

int quotactl(const char *path, int cmd, int id, void *addr) {
  (void)path;
  (void)cmd;
  (void)id;
  (void)addr;
  errno = EOPNOTSUPP;
  return -1;
}
#endif

char jail_errmsg[1024] = "jails not supported";

int jail_getid(const char *name) {
  (void)name;
  return -1;
}

char *jail_getname(int jid) {
  (void)jid;
  return NULL;
}

int jail_getv(int flags, ...) {
  (void)flags;
  return -1;
}

int jail_setv(int flags, ...) {
  (void)flags;
  return -1;
}

int jail_get(struct iovec *iov, unsigned int niov, int flags) {
  (void)iov;
  (void)niov;
  (void)flags;
  return -1;
}

int jail_set(struct iovec *iov, unsigned int niov, int flags) {
  (void)iov;
  (void)niov;
  (void)flags;
  return -1;
}

int jail_attach(int jid) {
  (void)jid;
  return -1;
}

int jail_remove(int jid) {
  (void)jid;
  return -1;
}

int rctl_get_racct(const char *inbufp, size_t inbuflen, char *outbufp,
                   size_t outbuflen) {
  (void)inbufp;
  (void)inbuflen;
  (void)outbufp;
  (void)outbuflen;
  errno = ENOSYS;
  return -1;
}

int rctl_get_rules(const char *inbufp, size_t inbuflen, char *outbufp,
                   size_t outbuflen) {
  return rctl_get_racct(inbufp, inbuflen, outbufp, outbuflen);
}

int rctl_get_limits(const char *inbufp, size_t inbuflen, char *outbufp,
                    size_t outbuflen) {
  return rctl_get_racct(inbufp, inbuflen, outbufp, outbuflen);
}

int rctl_add_rule(const char *inbufp, size_t inbuflen, char *outbufp,
                  size_t outbuflen) {
  return rctl_get_racct(inbufp, inbuflen, outbufp, outbuflen);
}

int rctl_remove_rule(const char *inbufp, size_t inbuflen, char *outbufp,
                     size_t outbuflen) {
  return rctl_get_racct(inbufp, inbuflen, outbufp, outbuflen);
}

acl_t acl_get_file(const char *path_p, acl_type_t type) {
  (void)path_p;
  (void)type;
  return NULL;
}

acl_t acl_get_link_np(const char *path_p, acl_type_t type) {
  (void)path_p;
  (void)type;
  return NULL;
}

acl_t acl_get_fd(int fd) {
  (void)fd;
  return NULL;
}

acl_t acl_get_fd_np(int fd, acl_type_t type) {
  (void)fd;
  (void)type;
  return NULL;
}

int acl_get_entry(acl_t acl, int entry_id, acl_entry_t *entry_p) {
  (void)acl;
  (void)entry_id;
  if (entry_p) {
    *entry_p = NULL;
  }
  return 0;
}

int acl_get_tag_type(acl_entry_t entry_d, acl_tag_t *tag_type_p) {
  (void)entry_d;
  if (tag_type_p) {
    *tag_type_p = 0;
  }
  return -1;
}

int acl_free(void *obj_p) {
  if (obj_p) {
    free(obj_p);
  }
  return 0;
}

int acl_is_trivial_np(const acl_t acl, int *trivialp) {
  (void)acl;
  if (trivialp) {
    *trivialp = 1;
  }
  return 0;
}

ssize_t extattr_get_fd(int fd, int attrnamespace, const char *attrname,
                       void *data, size_t nbytes) {
  (void)fd;
  (void)attrnamespace;
  (void)attrname;
  (void)data;
  (void)nbytes;
  errno = EOPNOTSUPP;
  return -1;
}

ssize_t extattr_get_file(const char *path, int attrnamespace,
                         const char *attrname, void *data, size_t nbytes) {
  (void)path;
  return extattr_get_fd(-1, attrnamespace, attrname, data, nbytes);
}

ssize_t extattr_get_link(const char *path, int attrnamespace,
                         const char *attrname, void *data, size_t nbytes) {
  (void)path;
  return extattr_get_fd(-1, attrnamespace, attrname, data, nbytes);
}

ssize_t extattr_list_fd(int fd, int attrnamespace, void *data, size_t nbytes) {
  return extattr_get_fd(fd, attrnamespace, NULL, data, nbytes);
}

ssize_t extattr_list_file(const char *path, int attrnamespace, void *data,
                          size_t nbytes) {
  (void)path;
  return extattr_get_fd(-1, attrnamespace, NULL, data, nbytes);
}

ssize_t extattr_list_link(const char *path, int attrnamespace, void *data,
                          size_t nbytes) {
  (void)path;
  return extattr_get_fd(-1, attrnamespace, NULL, data, nbytes);
}

int extattr_set_fd(int fd, int attrnamespace, const char *attrname,
                   const void *data, size_t nbytes) {
  (void)fd;
  (void)attrnamespace;
  (void)attrname;
  (void)data;
  (void)nbytes;
  errno = EOPNOTSUPP;
  return -1;
}

int extattr_set_file(const char *path, int attrnamespace, const char *attrname,
                     const void *data, size_t nbytes) {
  (void)path;
  return extattr_set_fd(-1, attrnamespace, attrname, data, nbytes);
}

int extattr_set_link(const char *path, int attrnamespace, const char *attrname,
                     const void *data, size_t nbytes) {
  (void)path;
  return extattr_set_fd(-1, attrnamespace, attrname, data, nbytes);
}

int extattr_delete_fd(int fd, int attrnamespace, const char *attrname) {
  return extattr_set_fd(fd, attrnamespace, attrname, NULL, 0);
}

int extattr_delete_file(const char *path, int attrnamespace,
                        const char *attrname) {
  (void)path;
  return extattr_set_fd(-1, attrnamespace, attrname, NULL, 0);
}

int extattr_delete_link(const char *path, int attrnamespace,
                        const char *attrname) {
  (void)path;
  return extattr_set_fd(-1, attrnamespace, attrname, NULL, 0);
}

int extattr_namespace_to_string(int attrnamespace, char **string) {
  if (string == NULL) {
    return -1;
  }
#if defined(_MSC_VER)
  *string =
      _strdup((attrnamespace == EXTATTR_NAMESPACE_SYSTEM) ? "system" : "user");
#else
  *string =
      strdup((attrnamespace == EXTATTR_NAMESPACE_SYSTEM) ? "system" : "user");
#endif
  return 0;
}

int extattr_string_to_namespace(const char *string, int *attrnamespace) {
  if (string == NULL || attrnamespace == NULL) {
    return -1;
  }
  *attrnamespace = (strcmp(string, "system") == 0) ? EXTATTR_NAMESPACE_SYSTEM
                                                   : EXTATTR_NAMESPACE_USER;
  return 0;
}

typedef int make_iso_compilers_happy_tu_bsd_kvm_procstat;

#endif
typedef int make_iso_compilers_happy_mega_bsd_kvm_procstat;
