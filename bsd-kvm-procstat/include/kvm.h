#ifndef BSD_KVM_H
#define BSD_KVM_H

/**
 * @file kvm.h
 * @brief FreeBSD kernel virtual memory access library (<kvm.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/user.h>
#include <nlist.h>
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define _POSIX2_LINE_MAX 2048

/** @brief Opaque KVM descriptor handle. */
typedef struct __kvm kvm_t;

/** @brief Swap device usage information structure. */
struct kvm_swap {
  char ksw_devname[32];
  int ksw_used;
  int ksw_total;
  int ksw_flags;
  int ksw_reserved1;
  int ksw_reserved2;
};

#define SWIF_DEV_PREFIX 0x0002

/** @brief Open kernel virtual memory descriptor with error reporting callback.
 */
kvm_t *kvm_open(const char *uf, const char *mf, const char *sf, int flag,
                const char *errstr);
/** @brief Open kernel virtual memory descriptor writing error into errbuf. */
kvm_t *kvm_openfiles(const char *uf, const char *mf, const char *sf, int flag,
                     char *errbuf);
/** @brief Open cross-architecture kernel virtual memory descriptor. */
kvm_t *kvm_open2(const char *uf, const char *mf, int flag, char *errbuf,
                 int (*resolver)(const char *, uint64_t *));
/** @brief Close a KVM descriptor. */
int kvm_close(kvm_t *kd);
/** @brief Retrieve process list from KVM descriptor. */
struct kinfo_proc *kvm_getprocs(kvm_t *kd, int op, int arg, int *cnt);
/** @brief Retrieve argument vector for a process. */
char **kvm_getargv(kvm_t *kd, const struct kinfo_proc *p, int nchr);
/** @brief Retrieve environment vector for a process. */
char **kvm_getenvv(kvm_t *kd, const struct kinfo_proc *p, int nchr);
/** @brief Retrieve swap usage information. */
int kvm_getswapinfo(kvm_t *kd, struct kvm_swap *swap_ary, int swap_max,
                    int flags);
/** @brief Resolve kernel symbols from namelist. */
int kvm_nlist(kvm_t *kd, struct nlist *nl);
/** @brief Resolve 64-bit kernel symbols from namelist. */
int kvm_nlist2(kvm_t *kd, struct nlist *nl);
/** @brief Read bytes from kernel virtual address. */
ssize_t kvm_read(kvm_t *kd, unsigned long addr, void *buf, size_t nbytes);
/** @brief Read bytes from 64-bit kernel virtual address. */
ssize_t kvm_read2(kvm_t *kd, uint64_t addr, void *buf, size_t nbytes);
/** @brief Write bytes to kernel virtual address. */
ssize_t kvm_write(kvm_t *kd, unsigned long addr, const void *buf,
                  size_t nbytes);
/** @brief Return last error string on a KVM descriptor. */
char *kvm_geterr(kvm_t *kd);
/** @brief Retrieve CPU state time counters. */
int kvm_getcptime(kvm_t *kd, long *cp_time);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_KVM_H */
