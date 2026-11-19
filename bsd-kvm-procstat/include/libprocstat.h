#ifndef BSD_LIBPROCSTAT_H
#define BSD_LIBPROCSTAT_H

/**
 * @file libprocstat.h
 * @brief FreeBSD process and file inspection library (<libprocstat.h>)
 * polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/user.h>
#include <elf.h>
#include <sys/resource.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

struct procstat;
#define PS_FST_VTYPE_VNON 1
#define PS_FST_VTYPE_VREG 2
#define PS_FST_VTYPE_VDIR 3
#define PS_FST_VTYPE_VBLK 4
#define PS_FST_VTYPE_VCHR 5
#define PS_FST_VTYPE_VLNK 6
#define PS_FST_VTYPE_VSOCK 7
#define PS_FST_VTYPE_VFIFO 8
#define PS_FST_VTYPE_VBAD 9
#define PS_FST_VTYPE_UNKNOWN 255

#define PS_FST_TYPE_VNODE 1
#define PS_FST_TYPE_FIFO 2
#define PS_FST_TYPE_SOCKET 3
#define PS_FST_TYPE_PIPE 4
#define PS_FST_TYPE_PTS 5
#define PS_FST_TYPE_KQUEUE 6
#define PS_FST_TYPE_MQUEUE 8
#define PS_FST_TYPE_SHM 9
#define PS_FST_TYPE_SEM 10
#define PS_FST_TYPE_UNKNOWN 11
#define PS_FST_TYPE_NONE 12
#define PS_FST_TYPE_PROCDESC 13
#define PS_FST_TYPE_DEV 14
#define PS_FST_TYPE_EVENTFD 15
#define PS_FST_TYPE_INOTIFY 16

#define PS_FST_UFLAG_RDIR 0x0001
#define PS_FST_UFLAG_CDIR 0x0002
#define PS_FST_UFLAG_JAIL 0x0004
#define PS_FST_UFLAG_TRACE 0x0008
#define PS_FST_UFLAG_TEXT 0x0010
#define PS_FST_UFLAG_MMAP 0x0020
#define PS_FST_UFLAG_CTTY 0x0040

#define PS_FST_FFLAG_READ 0x0001
#define PS_FST_FFLAG_WRITE 0x0002
#define PS_FST_FFLAG_NONBLOCK 0x0004
#define PS_FST_FFLAG_APPEND 0x0008
#define PS_FST_FFLAG_SHLOCK 0x0010
#define PS_FST_FFLAG_EXLOCK 0x0020
#define PS_FST_FFLAG_ASYNC 0x0040
#define PS_FST_FFLAG_SYNC 0x0080
#define PS_FST_FFLAG_NOFOLLOW 0x0100
#define PS_FST_FFLAG_CREAT 0x0200
#define PS_FST_FFLAG_TRUNC 0x0400
#define PS_FST_FFLAG_EXCL 0x0800
#define PS_FST_FFLAG_DIRECT 0x1000
#define PS_FST_FFLAG_EXEC 0x2000
#define PS_FST_FFLAG_HASLOCK 0x4000

struct rlimit;

/** @brief Linked list head of file info structures. */
STAILQ_HEAD(filestat_list, filestat);

/** @brief Individual open file descriptor entry for libprocstat. */
struct filestat {
  int fs_type;
  int fs_flags;
  int fs_fflags;
  int fs_uflags;
  int fs_fd;
  int fs_ref_count;
  off_t fs_offset;
  void *fs_typedep;
  char *fs_path;
  STAILQ_ENTRY(filestat) next;
  cap_rights_t fs_cap_rights;
};

/** @brief Vnode status details for procstat_get_vnode_info. */
struct vnstat {
  uint64_t vn_fileid;
  uint64_t vn_size;
  char *vn_mntdir;
  int vn_type;
  uint64_t vn_fsid;
  uint64_t vn_dev;
  char vn_devname[32];
  uint16_t vn_mode;
};

/** @brief Socket status details for procstat_get_socket_info. */
struct sockstat {
  uint64_t inp_ppcb;
  uint64_t so_addr;
  uint64_t so_pcb;
  uint64_t unp_conn;
  int dom_family;
  int proto;
  int so_rcv_sb_state;
  int so_snd_sb_state;
  struct sockaddr_storage sa_local;
  struct sockaddr_storage sa_peer;
  int type;
  char dname[32];
  unsigned int send_q;
  unsigned int recv_q;
  unsigned int sendq;
  unsigned int recvq;
};

/** @brief Pipe status details for procstat_get_pipe_info. */
struct pipestat {
  size_t buffer_cnt;
  uint64_t addr;
  uint64_t peer;
};

/** @brief Pseudo-terminal status details for procstat_get_pts_info. */
struct ptsstat {
  uint64_t dev;
  char devname[32];
};

/** @brief Semaphore status details for procstat_get_sem_info. */
struct semstat {
  uint32_t value;
  uint16_t mode;
};

/** @brief Shared memory status details for procstat_get_shm_info. */
struct shmstat {
  uint64_t size;
  uint16_t mode;
};

/** @brief Open procstat handle via sysctl backend. */
struct procstat *procstat_open_sysctl(void);
/** @brief Open procstat handle via kvm backend. */
struct procstat *procstat_open_kvm(const char *nlistf, const char *memf);
/** @brief Open procstat handle via core file backend. */
struct procstat *procstat_open_core(const char *filename);
/** @brief Close procstat handle. */
void procstat_close(struct procstat *prstat);

/** @brief Retrieve process list from procstat handle. */
struct kinfo_proc *procstat_getprocs(struct procstat *prstat, int what, int arg,
                                     unsigned int *count);
/** @brief Free process list returned by procstat_getprocs. */
void procstat_freeprocs(struct procstat *prstat, struct kinfo_proc *p);

/** @brief Retrieve open file list for a process. */
struct filestat_list *procstat_getfiles(struct procstat *prstat,
                                        struct kinfo_proc *kp, int mmapped);
/** @brief Free file list returned by procstat_getfiles. */
void procstat_freefiles(struct procstat *prstat, struct filestat_list *head);

/** @brief Retrieve virtual memory map for a process. */
struct kinfo_vmentry *procstat_getvmmap(struct procstat *prstat,
                                        struct kinfo_proc *kp,
                                        unsigned int *count);
/** @brief Free virtual memory map returned by procstat_getvmmap. */
void procstat_freevmmap(struct procstat *prstat, struct kinfo_vmentry *vmmap);

/** @brief Retrieve argument vector for a process. */
char **procstat_getargv(struct procstat *prstat, struct kinfo_proc *p,
                        size_t nchr);
/** @brief Free argument vector cached in procstat handle. */
void procstat_freeargv(struct procstat *prstat);
/** @brief Retrieve environment vector for a process. */
char **procstat_getenvv(struct procstat *prstat, struct kinfo_proc *p,
                        size_t nchr);
/** @brief Free environment vector cached in procstat handle. */
void procstat_freeenvv(struct procstat *prstat);

/** @brief Retrieve executable pathname for a process. */
int procstat_getpathname(struct procstat *prstat, struct kinfo_proc *kp,
                         char *pathname, size_t maxlen);
/** @brief Retrieve resource limit for a process. */
int procstat_getrlimit(struct procstat *prstat, struct kinfo_proc *kp,
                       int which, struct rlimit *rlimit);
/** @brief Retrieve file creation mode mask for a process. */
int procstat_getumask(struct procstat *prstat, struct kinfo_proc *kp,
                      unsigned short *maskp);
/** @brief Retrieve supplementary groups for a process. */
gid_t *procstat_getgroups(struct procstat *prstat, struct kinfo_proc *kp,
                          unsigned int *count);
/** @brief Free group list returned by procstat_getgroups. */
void procstat_freegroups(struct procstat *prstat, gid_t *groups);
/** @brief Retrieve OS release date for a process. */
int procstat_getosrel(struct procstat *prstat, struct kinfo_proc *kp,
                      int *osrelp);
/** @brief Retrieve kernel thread stacks for a process. */
struct kinfo_kstack *procstat_getkstack(struct procstat *prstat,
                                        struct kinfo_proc *kp,
                                        unsigned int *count);
/** @brief Free kernel stack entries returned by procstat_getkstack. */
void procstat_freekstack(struct procstat *prstat, struct kinfo_kstack *kkstp);

/** @brief Retrieve vnode details for an open file entry. */
int procstat_get_vnode_info(struct procstat *prstat, struct filestat *fst,
                            struct vnstat *vn, char *errbuf);
/** @brief Retrieve socket details for an open file entry. */
int procstat_get_socket_info(struct procstat *prstat, struct filestat *fst,
                             struct sockstat *sock, char *errbuf);
/** @brief Retrieve pipe details for an open file entry. */
int procstat_get_pipe_info(struct procstat *prstat, struct filestat *fst,
                           struct pipestat *pipe, char *errbuf);
/** @brief Retrieve pseudo-terminal details for an open file entry. */
int procstat_get_pts_info(struct procstat *prstat, struct filestat *fst,
                          struct ptsstat *pts, char *errbuf);
/** @brief Retrieve semaphore details for an open file entry. */
int procstat_get_sem_info(struct procstat *prstat, struct filestat *fst,
                          struct semstat *sem, char *errbuf);
/** @brief Retrieve shared memory details for an open file entry. */
int procstat_get_shm_info(struct procstat *prstat, struct filestat *fst,
                          struct shmstat *shm, char *errbuf);

#define PS_ADVLOCK_RO 0x01
#define PS_ADVLOCK_RW 0x02
#define PS_ADVLOCK_TYPE_FLOCK 0x01
#define PS_ADVLOCK_TYPE_PID 0x02
#define PS_ADVLOCK_TYPE_REMOTE 0x03

struct advlock {
  int rw;
  int type;
  int pid;
  int sysid;
  uint64_t file_fsid;
  uint64_t file_rdev;
  uint64_t file_fileid;
  off_t start;
  off_t len;
  const char *path;
  STAILQ_ENTRY(advlock) next;
};
STAILQ_HEAD(advlock_list, advlock);

/** @brief Retrieve ELF auxiliary vector for a process. */
Elf_Auxinfo *procstat_getauxv(struct procstat *prstat, struct kinfo_proc *kp,
                              unsigned int *cntp);
/** @brief Free ELF auxiliary vector returned by procstat_getauxv. */
void procstat_freeauxv(struct procstat *prstat, Elf_Auxinfo *auxv);

struct ptrace_lwpinfo;
struct kinfo_knote;
/** @brief Retrieve ptrace LWP thread information from procstat handle. */
struct ptrace_lwpinfo *procstat_getptlwpinfo(struct procstat *prstat,
                                             unsigned int *cntp);
/** @brief Free ptrace LWP array returned by procstat_getptlwpinfo. */
void procstat_freeptlwpinfo(struct procstat *prstat, struct ptrace_lwpinfo *pl);
/** @brief Retrieve current resource limit usage array for a process. */
rlim_t *procstat_getrlimitusage(struct procstat *prstat, struct kinfo_proc *kp,
                                unsigned int *cntp);
/** @brief Free resource limit usage array returned by procstat_getrlimitusage.
 */
void procstat_freerlimitusage(struct procstat *prstat, rlim_t *resuse);
/** @brief Retrieve registered knotes for an open kqueue descriptor. */
struct kinfo_knote *procstat_get_kqueue_info(struct procstat *prstat,
                                             struct kinfo_proc *kp, int kqfd,
                                             unsigned int *cntp, char *errbuf);
/** @brief Free knote array returned by procstat_get_kqueue_info. */
void procstat_freekqinfo(struct procstat *prstat, struct kinfo_knote *kni);

/** @brief Retrieve advisory file locks across the system. */
struct advlock_list *procstat_getadvlock(struct procstat *prstat);
/** @brief Free advisory lock list returned by procstat_getadvlock. */
void procstat_freeadvlock(struct procstat *prstat, struct advlock_list *lst);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_LIBPROCSTAT_H */
