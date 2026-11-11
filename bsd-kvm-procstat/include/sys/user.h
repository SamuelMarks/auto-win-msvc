#ifndef SYS_USER_H
#define SYS_USER_H

/**
 * @file user.h
 * @brief FreeBSD kernel/user process, file, and vmmap inspection structures
 * (<sys/user.h>).
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/proc.h>
#include <signal.h>
#if defined(__APPLE__)
#include <mach/vm_types.h>
#endif
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
#if defined(__has_include)
#if __has_include("linux-sys-user.h")
#include "linux-sys-user.h"
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define KI_NSPARE_INT 4
#define KI_NSPARE_LONG 12
#define KI_NSPARE_PTR 6

#ifndef COMMLEN
#define COMMLEN 19
#endif
#ifndef KI_EMULNAMELEN
#define KI_EMULNAMELEN 16
#endif
#ifndef LOGINCLASSLEN
#define LOGINCLASSLEN 17
#endif
#ifndef LOCKNAMELEN
#define LOCKNAMELEN 8
#endif
#ifndef WMESGLEN
#define WMESGLEN 8
#endif
#ifndef LOGNAMELEN
#define LOGNAMELEN 17
#endif
#ifndef TDNAMLEN
#define TDNAMLEN 16
#endif

/** @brief Priority info structure inside kinfo_proc. */
struct priority {
  unsigned char pri_class;
  unsigned char pri_level;
  unsigned char pri_native;
  unsigned char pri_user;
};

/** @brief FreeBSD process and thread information structure. */
struct kinfo_proc {
  int ki_structsize;
  int ki_layout;
  void *ki_args;
  void *ki_paddr;
  void *ki_addr;
  void *ki_tracep;
  void *ki_textvp;
  void *ki_fd;
  void *ki_vmspace;
  const void *ki_wchan;
  pid_t ki_pid;
  pid_t ki_ppid;
  pid_t ki_pgid;
  pid_t ki_tpgid;
  pid_t ki_sid;
  pid_t ki_tsid;
  short ki_jobc;
  uint32_t ki_tdev;
  sigset_t ki_siglist;
  sigset_t ki_sigmask;
  sigset_t ki_sigignore;
  sigset_t ki_sigcatch;
  uid_t ki_uid;
  uid_t ki_ruid;
  uid_t ki_svuid;
  gid_t ki_rgid;
  gid_t ki_svgid;
  short ki_ngroups;
  gid_t ki_groups[16];
  vm_size_t ki_size;
  segsz_t ki_rssize;
  segsz_t ki_swrss;
  segsz_t ki_tsize;
  segsz_t ki_dsize;
  segsz_t ki_ssize;
  u_short ki_xstat;
  u_short ki_acflag;
  fixpt_t ki_pctcpu;
  u_int ki_estcpu;
  u_int ki_slptime;
  u_int ki_swtime;
  u_int ki_cow;
  uint64_t ki_runtime;
  struct timeval ki_start;
  struct timeval ki_childtime;
  long ki_flag;
  long ki_kiflag;
  int ki_traceflag;
  char ki_stat;
  signed char ki_nice;
  char ki_lock;
  char ki_rqindex;
  u_char ki_oncpu;
  u_char ki_lastcpu;
  char ki_tdname[TDNAMLEN + 1];
  char ki_wmesg[WMESGLEN + 1];
  char ki_login[LOGNAMELEN + 1];
  char ki_lockname[LOCKNAMELEN + 1];
  char ki_comm[COMMLEN + 1];
  char ki_emul[KI_EMULNAMELEN + 1];
  char ki_loginclass[LOGINCLASSLEN + 1];
  int ki_jid;
  int ki_numthreads;
  lwpid_t ki_tid;
  struct priority ki_pri;
  struct rusage ki_rusage;
  struct rusage ki_rusage_ch;
  void *ki_pcb;
  void *ki_kstack;
  void *ki_udata;
  void *ki_tdaddr;
  long ki_sflag;
  long ki_tdflags;
};

/* File descriptor types in kinfo_file */
#define KF_TYPE_NONE 0
#define KF_TYPE_VNODE 1
#define KF_TYPE_SOCKET 2
#define KF_TYPE_PIPE 3
#define KF_TYPE_FIFO 4
#define KF_TYPE_KQUEUE 5
#define KF_TYPE_MQUEUE 6
#define KF_TYPE_SHM 7
#define KF_TYPE_SEM 8
#define KF_TYPE_PTS 9
#define KF_TYPE_PROCDESC 10
#define KF_TYPE_DEV 11
#define KF_TYPE_EVENTFD 12

#define KF_FD_TYPE_CWD -1
#define KF_FD_TYPE_ROOT -2
#define KF_FD_TYPE_JAIL -3
#define KF_FD_TYPE_TRACE -4
#define KF_FD_TYPE_TEXT -5
#define KF_FD_TYPE_CTTY -6

#define KF_FLAG_READ 0x00000001
#define KF_FLAG_WRITE 0x00000002
#define KF_FLAG_APPEND 0x00000004
#define KF_FLAG_ASYNC 0x00000008
#define KF_FLAG_FSYNC 0x00000010
#define KF_FLAG_NONBLOCK 0x00000020
#define KF_FLAG_DIRECT 0x00000040
#define KF_FLAG_HASLOCK 0x00000080
#define KF_FLAG_EXEC 0x00004000

/** @brief FreeBSD open file descriptor information structure. */
struct kinfo_file {
  int kf_structsize;
  int kf_type;
  int kf_fd;
  int kf_ref_count;
  int kf_flags;
  off_t kf_offset;
  int kf_vnode_type;
  int kf_sock_domain;
  int kf_sock_type;
  int kf_sock_protocol;
  uint64_t kf_file_fsid;
  uint64_t kf_file_rdev;
  uint64_t kf_file_fileid;
  uint64_t kf_file_size;
  uint16_t kf_file_mode;
  char kf_path[1024];
};

#define KVME_TYPE_NONE 0
#define KVME_TYPE_DEFAULT 1
#define KVME_TYPE_VNODE 2
#define KVME_TYPE_SWAP 3
#define KVME_TYPE_DEVICE 4
#define KVME_TYPE_PHYS 5
#define KVME_TYPE_DEAD 6
#define KVME_TYPE_SG 7
#define KVME_TYPE_MGTDEVICE 8
#define KVME_TYPE_GUARD 9

#define KVME_PROT_READ 0x00000001
#define KVME_PROT_WRITE 0x00000002
#define KVME_PROT_EXEC 0x00000004

#define KVME_FLAG_COW 0x00000001
#define KVME_FLAG_NEEDS_COPY 0x00000002
#define KVME_FLAG_NOCOREDUMP 0x00000004
#define KVME_FLAG_SUPER 0x00000008
#define KVME_FLAG_GROWS_UP 0x00000010
#define KVME_FLAG_GROWS_DOWN 0x00000020

/** @brief FreeBSD virtual memory map entry structure. */
struct kinfo_vmentry {
  int kve_structsize;
  int kve_type;
  uint64_t kve_start;
  uint64_t kve_end;
  uint64_t kve_offset;
  uint64_t kve_vn_fileid;
  uint64_t kve_vn_fsid;
  int kve_flags;
  int kve_resident;
  int kve_private_resident;
  int kve_protection;
  int kve_ref_count;
  int kve_shadow_count;
  int kve_vn_type;
  uint64_t kve_vn_size;
  uint64_t kve_vn_rdev;
  uint16_t kve_vn_mode;
  uint16_t kve_status;
  char kve_path[1024];
};

/** @brief FreeBSD kernel stack trace entry structure. */
struct kinfo_kstack {
  lwpid_t kkst_tid;
  int kkst_state;
  char kkst_trace[1024];
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_USER_H */
