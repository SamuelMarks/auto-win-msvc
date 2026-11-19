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
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Maximum line length constant for KVM utilities. */
#define _POSIX2_LINE_MAX 2048

/** @brief Opaque KVM descriptor handle. */
typedef struct __kvm kvm_t;

/** @brief Swap device usage information structure. */
struct kvm_swap {
  char ksw_devname[32]; /**< Swap device name. */
  int ksw_used;         /**< Used swap blocks. */
  int ksw_total;        /**< Total swap blocks. */
  int ksw_flags;        /**< Swap device flags. */
  int ksw_reserved1;    /**< Reserved field 1. */
  int ksw_reserved2;    /**< Reserved field 2. */
};

/** @brief Include /dev/ prefix in swap device names. */
#define SWIF_DEV_PREFIX 0x0002

/**
 * @brief Open kernel virtual memory descriptor with error reporting prefix.
 * @param[in] uf Kernel executable path, or NULL.
 * @param[in] mf Memory device/core path, or NULL.
 * @param[in] sf Swap device path, or NULL.
 * @param[in] flag Access mode flags.
 * @param[in] errstr Optional error prefix string.
 * @return KVM handle, or NULL on error.
 */
kvm_t *kvm_open(const char *uf, const char *mf, const char *sf, int flag,
                const char *errstr);

/**
 * @brief Open kernel virtual memory descriptor writing error into errbuf.
 * @param[in] uf Kernel executable path, or NULL.
 * @param[in] mf Memory device/core path, or NULL.
 * @param[in] sf Swap device path, or NULL.
 * @param[in] flag Access mode flags.
 * @param[out] errbuf Optional 256-byte error message buffer.
 * @return KVM handle, or NULL on error.
 */
kvm_t *kvm_openfiles(const char *uf, const char *mf, const char *sf, int flag,
                     char *errbuf);

/**
 * @brief Open cross-architecture kernel virtual memory descriptor.
 * @param[in] uf Kernel executable path, or NULL.
 * @param[in] mf Memory device/core path, or NULL.
 * @param[in] flag Access mode flags.
 * @param[out] errbuf Optional 256-byte error message buffer.
 * @param[in] resolver Optional symbol resolver callback.
 * @return KVM handle, or NULL on error.
 */
kvm_t *kvm_open2(const char *uf, const char *mf, int flag, char *errbuf,
                 int (*resolver)(const char *, uint64_t *));

/**
 * @brief Close a KVM descriptor.
 * @param[in] kd KVM descriptor.
 * @return 0 on success, -1 if kd is NULL.
 */
int kvm_close(kvm_t *kd);

/**
 * @brief Retrieve process list from KVM descriptor.
 * @param[in] kd KVM descriptor.
 * @param[in] op Selection operator.
 * @param[in] arg Selection argument.
 * @param[out] cnt Pointer receiving process count.
 * @return Array of kinfo_proc records owned by kd, or NULL on error.
 */
struct kinfo_proc *kvm_getprocs(kvm_t *kd, int op, int arg, int *cnt);

/**
 * @brief Retrieve argument vector for a process.
 * @param[in] kd KVM descriptor.
 * @param[in] p Process record.
 * @param[in] nchr Maximum characters to copy.
 * @return NULL-terminated argument vector, or NULL on error.
 */
char **kvm_getargv(kvm_t *kd, const struct kinfo_proc *p, int nchr);

/**
 * @brief Retrieve environment vector for a process.
 * @param[in] kd KVM descriptor.
 * @param[in] p Process record.
 * @param[in] nchr Maximum characters to copy.
 * @return NULL-terminated environment vector, or NULL on error.
 */
char **kvm_getenvv(kvm_t *kd, const struct kinfo_proc *p, int nchr);

/**
 * @brief Retrieve swap usage information.
 * @param[in] kd KVM descriptor.
 * @param[out] swap_ary Output swap record array.
 * @param[in] swap_max Maximum entries in swap_ary.
 * @param[in] flags SWIF_* flags.
 * @return Number of populated entries.
 */
int kvm_getswapinfo(kvm_t *kd, struct kvm_swap *swap_ary, int swap_max,
                    int flags);

/**
 * @brief Resolve kernel symbols from namelist.
 * @param[in] kd KVM descriptor.
 * @param[in,out] nl Array of nlist entries.
 * @return Number of invalid entries, or -1 on error.
 */
int kvm_nlist(kvm_t *kd, struct nlist *nl);

/**
 * @brief Resolve 64-bit kernel symbols from namelist.
 * @param[in] kd KVM descriptor.
 * @param[in,out] nl Array of nlist entries.
 * @return Number of invalid entries, or -1 on error.
 */
int kvm_nlist2(kvm_t *kd, struct nlist *nl);

/**
 * @brief Read bytes from kernel virtual address.
 * @param[in] kd KVM descriptor.
 * @param[in] addr Kernel address.
 * @param[out] buf Destination buffer.
 * @param[in] nbytes Bytes to read.
 * @return Bytes read, or -1 on error.
 */
ssize_t kvm_read(kvm_t *kd, unsigned long addr, void *buf, size_t nbytes);

/**
 * @brief Read bytes from 64-bit kernel virtual address.
 * @param[in] kd KVM descriptor.
 * @param[in] addr 64-bit kernel address.
 * @param[out] buf Destination buffer.
 * @param[in] nbytes Bytes to read.
 * @return Bytes read, or -1 on error.
 */
ssize_t kvm_read2(kvm_t *kd, uint64_t addr, void *buf, size_t nbytes);

/**
 * @brief Write bytes to kernel virtual address.
 * @param[in] kd KVM descriptor.
 * @param[in] addr Kernel address.
 * @param[in] buf Source buffer.
 * @param[in] nbytes Bytes to write.
 * @return Bytes written, or -1 on error.
 */
ssize_t kvm_write(kvm_t *kd, unsigned long addr, const void *buf,
                  size_t nbytes);

/**
 * @brief Return last error string on a KVM descriptor.
 * @param[in] kd KVM descriptor.
 * @return Error string.
 */
char *kvm_geterr(kvm_t *kd);

/**
 * @brief Retrieve CPU state time counters.
 * @param[in] kd KVM descriptor.
 * @param[out] cp_time Five-element CPU time array.
 * @return 0 on success, -1 on error.
 */
int kvm_getcptime(kvm_t *kd, long *cp_time);

/**
 * @brief Overrides allocator hooks used by bsd-kvm-procstat for OOM testing.
 * @param[in] malloc_fn Custom malloc function, or NULL to restore default.
 * @param[in] calloc_fn Custom calloc function, or NULL to restore default.
 * @param[in] realloc_fn Custom realloc function, or NULL to restore default.
 * @param[in] strdup_fn Custom strdup function, or NULL to restore default.
 */
void bsd_kvm_procstat_set_alloc_hooks(void *(*malloc_fn)(size_t),
                                      void *(*calloc_fn)(size_t, size_t),
                                      void *(*realloc_fn)(void *, size_t),
                                      char *(*strdup_fn)(const char *));

#if defined(_WIN32)
/**
 * @brief Overrides Win32 OS query hooks for deterministic testing on Windows.
 * @param[in] snap_fn Custom process snapshot creator, or NULL for default.
 * @param[in] proc_step_fn Custom process iterator step, or NULL for default.
 * @param[in] close_handle_fn Custom handle closer, or NULL for default.
 * @param[in] tcp_tbl_fn Custom TCP table query, or NULL for default.
 * @param[in] udp_tbl_fn Custom UDP table query, or NULL for default.
 * @param[in] if_tbl_fn Custom interface table query, or NULL for default.
 * @param[in] mem_status_fn Custom GlobalMemoryStatusEx wrapper, or NULL for
 * default.
 */
void bsd_kvm_procstat_set_win32_hooks(
    void *(*snap_fn)(void),
    int (*proc_step_fn)(void *snap, int first, unsigned long *pid,
                        unsigned long *ppid, unsigned long *threads, char *comm,
                        size_t comm_sz),
    int (*close_handle_fn)(void *h),
    unsigned long (*tcp_tbl_fn)(void *buf, unsigned long *sz),
    unsigned long (*udp_tbl_fn)(void *buf, unsigned long *sz),
    unsigned long (*if_tbl_fn)(void *buf, unsigned long *sz),
    int (*mem_status_fn)(void *ms));
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_KVM_H */
