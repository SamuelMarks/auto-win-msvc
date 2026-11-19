#ifndef SYS_CAPSICUM_H
#define SYS_CAPSICUM_H

/**
 * @file capsicum.h
 * @brief FreeBSD Capsicum capability mode and rights polyfill for Windows and
 * DOS.
 */

/* clang-format off */
#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/caprights.h>
#include <stddef.h>
#include <stdarg.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Encode capability right word index and bitmask. */
#define CAPRIGHT(idx, bit) ((((uint64_t)1UL) << (57 + (idx))) | (bit))

/** @brief Permit read operations. */
#define CAP_READ CAPRIGHT(0, ((uint64_t)1UL))
/** @brief Permit write operations. */
#define CAP_WRITE                                                              \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000002UL))
/** @brief Permit querying file offset via lseek(fd, 0, SEEK_CUR). */
#define CAP_SEEK_TELL                                                          \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000004UL))
/** @brief Permit seeking file offset. */
#define CAP_SEEK                                                               \
  (CAP_SEEK_TELL | (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000008UL))
/** @brief Permit pread/preadv operations. */
#define CAP_PREAD (CAP_SEEK | CAP_READ)
/** @brief Permit pwrite/pwritev operations. */
#define CAP_PWRITE (CAP_SEEK | CAP_WRITE)
/** @brief Permit base mmap(PROT_NONE) operations. */
#define CAP_MMAP                                                               \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000010UL))
/** @brief Permit readable memory mapping. */
#define CAP_MMAP_R (CAP_MMAP | CAP_SEEK | CAP_READ)
/** @brief Permit writable memory mapping. */
#define CAP_MMAP_W (CAP_MMAP | CAP_SEEK | CAP_WRITE)
/** @brief Permit executable memory mapping. */
#define CAP_MMAP_X                                                             \
  (CAP_MMAP | CAP_SEEK |                                                       \
   (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000020UL))
/** @brief Permit read-write memory mapping. */
#define CAP_MMAP_RW (CAP_MMAP_R | CAP_MMAP_W)
/** @brief Permit read-execute memory mapping. */
#define CAP_MMAP_RX (CAP_MMAP_R | CAP_MMAP_X)
/** @brief Permit write-execute memory mapping. */
#define CAP_MMAP_WX (CAP_MMAP_W | CAP_MMAP_X)
/** @brief Permit read-write-execute memory mapping. */
#define CAP_MMAP_RWX (CAP_MMAP_R | CAP_MMAP_W | CAP_MMAP_X)
/** @brief Permit creating files via openat(O_CREAT). */
#define CAP_CREATE                                                             \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000040UL))
/** @brief Permit fexecve(). */
#define CAP_FEXECVE                                                            \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000080UL))
/** @brief Permit fsync() and fdatasync(). */
#define CAP_FSYNC                                                              \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000100UL))
/** @brief Permit ftruncate(). */
#define CAP_FTRUNCATE                                                          \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000200UL))
/** @brief Permit relative path lookup beneath directory descriptor. */
#define CAP_LOOKUP                                                             \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000400UL))
/** @brief Permit fchdir(). */
#define CAP_FCHDIR                                                             \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000800UL))
/** @brief Permit fchflags(). */
#define CAP_FCHFLAGS                                                           \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00001000UL))
/** @brief Permit chflagsat(). */
#define CAP_CHFLAGSAT (CAP_FCHFLAGS | CAP_LOOKUP)
/** @brief Permit fchmod(). */
#define CAP_FCHMOD                                                             \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00002000UL))
/** @brief Permit fchmodat(). */
#define CAP_FCHMODAT (CAP_FCHMOD | CAP_LOOKUP)
/** @brief Permit fchown(). */
#define CAP_FCHOWN                                                             \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00004000UL))
/** @brief Permit fchownat(). */
#define CAP_FCHOWNAT (CAP_FCHOWN | CAP_LOOKUP)
/** @brief Permit fcntl(). */
#define CAP_FCNTL                                                              \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00008000UL))
/** @brief Permit flock(). */
#define CAP_FLOCK                                                              \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00010000UL))
/** @brief Permit fpathconf(). */
#define CAP_FPATHCONF                                                          \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00020000UL))
/** @brief Permit UFS background fsck operations. */
#define CAP_FSCK                                                               \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00040000UL))
/** @brief Permit fstat(). */
#define CAP_FSTAT                                                              \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00080000UL))
/** @brief Permit fstatat(). */
#define CAP_FSTATAT (CAP_FSTAT | CAP_LOOKUP)
/** @brief Permit fstatfs(). */
#define CAP_FSTATFS                                                            \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00100000UL))
/** @brief Permit futimes() and futimens(). */
#define CAP_FUTIMES                                                            \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00200000UL))
/** @brief Permit futimesat() and utimensat(). */
#define CAP_FUTIMESAT (CAP_FUTIMES | CAP_LOOKUP)
/** @brief Permit linkat() target directory operations. */
#define CAP_LINKAT_TARGET                                                      \
  (CAP_LOOKUP | (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00400000UL))
/** @brief Permit mkdirat(). */
#define CAP_MKDIRAT                                                            \
  (CAP_LOOKUP | (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00800000UL))
/** @brief Permit mkfifoat(). */
#define CAP_MKFIFOAT                                                           \
  (CAP_LOOKUP | (((uint64_t)0x00000000UL << 32) | (uint64_t)0x01000000UL))
/** @brief Permit mknodat(). */
#define CAP_MKNODAT                                                            \
  (CAP_LOOKUP | (((uint64_t)0x00000000UL << 32) | (uint64_t)0x02000000UL))
/** @brief Permit renameat() source directory operations. */
#define CAP_RENAMEAT_SOURCE                                                    \
  (CAP_LOOKUP | (((uint64_t)0x00000000UL << 32) | (uint64_t)0x04000000UL))
/** @brief Permit symlinkat(). */
#define CAP_SYMLINKAT                                                          \
  (CAP_LOOKUP | (((uint64_t)0x00000000UL << 32) | (uint64_t)0x08000000UL))
/** @brief Permit unlinkat(). */
#define CAP_UNLINKAT                                                           \
  (CAP_LOOKUP | (((uint64_t)0x00000000UL << 32) | (uint64_t)0x10000000UL))
/** @brief Permit accept() and accept4(). */
#define CAP_ACCEPT                                                             \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x20000000UL))
/** @brief Permit bind(). */
#define CAP_BIND                                                               \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x40000000UL))
/** @brief Permit connect(). */
#define CAP_CONNECT                                                            \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x80000000UL))
/** @brief Permit getpeername(). */
#define CAP_GETPEERNAME                                                        \
  CAPRIGHT(0, (((uint64_t)0x00000001UL << 32) | (uint64_t)0x00000000UL))
/** @brief Permit getsockname(). */
#define CAP_GETSOCKNAME                                                        \
  CAPRIGHT(0, (((uint64_t)0x00000002UL << 32) | (uint64_t)0x00000000UL))
/** @brief Permit getsockopt(). */
#define CAP_GETSOCKOPT                                                         \
  CAPRIGHT(0, (((uint64_t)0x00000004UL << 32) | (uint64_t)0x00000000UL))
/** @brief Permit listen(). */
#define CAP_LISTEN                                                             \
  CAPRIGHT(0, (((uint64_t)0x00000008UL << 32) | (uint64_t)0x00000000UL))
/** @brief Permit sctp_peeloff(). */
#define CAP_PEELOFF                                                            \
  CAPRIGHT(0, (((uint64_t)0x00000010UL << 32) | (uint64_t)0x00000000UL))
/** @brief Alias of CAP_READ for socket receive operations. */
#define CAP_RECV CAP_READ
/** @brief Alias of CAP_WRITE for socket send operations. */
#define CAP_SEND CAP_WRITE
/** @brief Permit setsockopt(). */
#define CAP_SETSOCKOPT                                                         \
  CAPRIGHT(0, (((uint64_t)0x00000020UL << 32) | (uint64_t)0x00000000UL))
/** @brief Permit shutdown(). */
#define CAP_SHUTDOWN                                                           \
  CAPRIGHT(0, (((uint64_t)0x00000040UL << 32) | (uint64_t)0x00000000UL))
/** @brief Permit bindat(). */
#define CAP_BINDAT                                                             \
  (CAP_LOOKUP | (((uint64_t)0x00000080UL << 32) | (uint64_t)0x00000000UL))
/** @brief Permit connectat(). */
#define CAP_CONNECTAT                                                          \
  (CAP_LOOKUP | (((uint64_t)0x00000100UL << 32) | (uint64_t)0x00000000UL))
/** @brief Permit linkat() source directory operations. */
#define CAP_LINKAT_SOURCE                                                      \
  (CAP_LOOKUP | (((uint64_t)0x00000200UL << 32) | (uint64_t)0x00000000UL))
/** @brief Permit renameat() target directory operations. */
#define CAP_RENAMEAT_TARGET                                                    \
  (CAP_LOOKUP | (((uint64_t)0x00000400UL << 32) | (uint64_t)0x00000000UL))
/** @brief Composite capability mask for client sockets. */
#define CAP_SOCK_CLIENT                                                        \
  (CAP_CONNECT | CAP_GETPEERNAME | CAP_GETSOCKNAME | CAP_GETSOCKOPT |          \
   CAP_PEELOFF | CAP_RECV | CAP_SEND | CAP_SETSOCKOPT | CAP_SHUTDOWN)
/** @brief Composite capability mask for server sockets. */
#define CAP_SOCK_SERVER                                                        \
  (CAP_ACCEPT | CAP_BIND | CAP_GETPEERNAME | CAP_GETSOCKNAME |                 \
   CAP_GETSOCKOPT | CAP_LISTEN | CAP_PEELOFF | CAP_RECV | CAP_SEND |           \
   CAP_SETSOCKOPT | CAP_SHUTDOWN)
/** @brief All defined rights in capability word 0. */
#define CAP_ALL0                                                               \
  CAPRIGHT(0, (((uint64_t)0x000007FFUL << 32) | (uint64_t)0xFFFFFFFFUL))
/** @brief Reserved capability bit 44 in word 0. */
#define CAP_UNUSED0_44                                                         \
  CAPRIGHT(0, (((uint64_t)0x00000800UL << 32) | (uint64_t)0x00000000UL))
/** @brief Reserved capability bit 57 in word 0. */
#define CAP_UNUSED0_57                                                         \
  CAPRIGHT(0, (((uint64_t)0x01000000UL << 32) | (uint64_t)0x00000000UL))

/** @brief Permit mac_get_fd(). */
#define CAP_MAC_GET CAPRIGHT(1, ((uint64_t)1UL))
/** @brief Permit mac_set_fd(). */
#define CAP_MAC_SET                                                            \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000002UL))
/** @brief Permit ksem_getvalue(). */
#define CAP_SEM_GETVALUE                                                       \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000004UL))
/** @brief Permit ksem_post(). */
#define CAP_SEM_POST                                                           \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000008UL))
/** @brief Permit ksem_wait() and ksem_trywait(). */
#define CAP_SEM_WAIT                                                           \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000010UL))
/** @brief Permit select(), poll(), and kevent() monitoring. */
#define CAP_EVENT                                                              \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000020UL))
/** @brief Permit kevent() retrieval on kqueue descriptor. */
#define CAP_KQUEUE_EVENT                                                       \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000040UL))
/** @brief Permit ioctl(). */
#define CAP_IOCTL                                                              \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000080UL))
/** @brief Permit tty hook configuration. */
#define CAP_TTYHOOK                                                            \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000100UL))
/** @brief Permit pdgetpid(). */
#define CAP_PDGETPID                                                           \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000200UL))
/** @brief Permit pdwait4(). */
#define CAP_PDWAIT                                                             \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000400UL))
/** @brief Permit pdkill(). */
#define CAP_PDKILL                                                             \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000800UL))
/** @brief Permit extattr_delete_fd(). */
#define CAP_EXTATTR_DELETE                                                     \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00001000UL))
/** @brief Permit extattr_get_fd(). */
#define CAP_EXTATTR_GET                                                        \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00002000UL))
/** @brief Permit extattr_list_fd(). */
#define CAP_EXTATTR_LIST                                                       \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00004000UL))
/** @brief Permit extattr_set_fd(). */
#define CAP_EXTATTR_SET                                                        \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00008000UL))
/** @brief Permit acl_valid_fd_np(). */
#define CAP_ACL_CHECK                                                          \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00010000UL))
/** @brief Permit acl_delete_fd_np(). */
#define CAP_ACL_DELETE                                                         \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00020000UL))
/** @brief Permit acl_get_fd() and acl_get_fd_np(). */
#define CAP_ACL_GET                                                            \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00040000UL))
/** @brief Permit acl_set_fd() and acl_set_fd_np(). */
#define CAP_ACL_SET                                                            \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00080000UL))
/** @brief Permit kevent() changelist modification on kqueue descriptor. */
#define CAP_KQUEUE_CHANGE                                                      \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00100000UL))
/** @brief Composite capability mask for kqueue descriptors. */
#define CAP_KQUEUE (CAP_KQUEUE_EVENT | CAP_KQUEUE_CHANGE)
/** @brief All defined rights in capability word 1. */
#define CAP_ALL1                                                               \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x001FFFFFUL))
/** @brief Convenience mask for openat() read/write lookup. */
#define CAP_OPENAT (CAP_LOOKUP | CAP_READ | CAP_WRITE)

/** @brief Permit fcntl(F_GETFL). */
#define CAP_FCNTL_GETFL (1 << 3)
/** @brief Permit fcntl(F_SETFL). */
#define CAP_FCNTL_SETFL (1 << 4)
/** @brief Permit fcntl(F_GETOWN). */
#define CAP_FCNTL_GETOWN (1 << 5)
/** @brief Permit fcntl(F_SETOWN). */
#define CAP_FCNTL_SETOWN (1 << 6)
/** @brief Mask of all fcntl capability sub-rights. */
#define CAP_FCNTL_ALL                                                          \
  (CAP_FCNTL_GETFL | CAP_FCNTL_SETFL | CAP_FCNTL_GETOWN | CAP_FCNTL_SETOWN)

/** @brief Sentinel indicating all ioctl commands are permitted. */
#define CAP_IOCTLS_ALL 2147483647L

/**
 * @brief Enter Capsicum capability mode (no-op on Windows/DOS).
 * @return Always 0.
 */
int cap_enter(void);

/**
 * @brief Query whether process is in capability mode.
 * @param[out] modep Pointer receiving 0 (not in capability mode).
 * @return 0 on success, -1 on NULL pointer.
 */
int cap_getmode(unsigned int *modep);

/**
 * @brief Check if process is sandboxed in capability mode.
 * @return Always 0 (false).
 */
int cap_sandboxed(void);

/**
 * @brief Initialize a cap_rights_t structure with variadic capability bits.
 * @param[in] version CAP_RIGHTS_VERSION.
 * @param[out] rights Structure to initialize.
 * @return Pointer to rights.
 */
cap_rights_t *__cap_rights_init(int version, cap_rights_t *rights, ...);

/**
 * @brief Set additional capability bits on a cap_rights_t structure.
 * @param[in,out] rights Structure to modify.
 * @return Pointer to rights.
 */
cap_rights_t *__cap_rights_set(cap_rights_t *rights, ...);

/**
 * @brief Clear capability bits from a cap_rights_t structure.
 * @param[in,out] rights Structure to modify.
 * @return Pointer to rights.
 */
cap_rights_t *__cap_rights_clear(cap_rights_t *rights, ...);

/**
 * @brief Check whether capability bits are set in a cap_rights_t structure.
 * @param[in] rights Structure to inspect.
 * @return 1 if set, 0 otherwise.
 */
int __cap_rights_is_set(const cap_rights_t *rights, ...);

/**
 * @brief Validate a cap_rights_t structure.
 * @param[in] rights Structure to validate.
 * @return 1 if valid.
 */
int cap_rights_is_valid(const cap_rights_t *rights);

/**
 * @brief Merge capability rights from src into dst.
 * @param[in,out] dst Destination rights.
 * @param[in] src Source rights.
 * @return Pointer to dst.
 */
cap_rights_t *cap_rights_merge(cap_rights_t *dst, const cap_rights_t *src);

/**
 * @brief Remove capability rights in src from dst.
 * @param[in,out] dst Destination rights.
 * @param[in] src Rights to remove.
 * @return Pointer to dst.
 */
cap_rights_t *cap_rights_remove(cap_rights_t *dst, const cap_rights_t *src);

/**
 * @brief Check if big contains all capability rights in little.
 * @param[in] big Superset rights.
 * @param[in] little Subset rights.
 * @return 1 if big contains little.
 */
int cap_rights_contains(const cap_rights_t *big, const cap_rights_t *little);

/** @brief Initialize capability rights structure (C89 variadic fallback). */
cap_rights_t *cap_rights_init(cap_rights_t *rights, ...);
/** @brief Set capability rights on structure (C89 variadic fallback). */
cap_rights_t *cap_rights_set(cap_rights_t *rights, ...);
/** @brief Clear capability rights on structure (C89 variadic fallback). */
cap_rights_t *cap_rights_clear(cap_rights_t *rights, ...);
/** @brief Check whether capability rights are set (C89 variadic fallback). */
int cap_rights_is_set(const cap_rights_t *rights, ...);

/**
 * @brief Limit capability rights on a file descriptor (no-op on Windows/DOS).
 * @param[in] fd File descriptor.
 * @param[in] rights Capability rights.
 * @return Always 0.
 */
int cap_rights_limit(int fd, const cap_rights_t *rights);

/**
 * @brief Retrieve capability rights for a file descriptor.
 * @param[in] version Capability rights structure version.
 * @param[in] fd File descriptor.
 * @param[out] rights Output rights structure.
 * @return Always 0.
 */
int __cap_rights_get(int version, int fd, cap_rights_t *rights);
/** @brief Retrieve capability rights for a file descriptor. */
#define cap_rights_get(fd, rights)                                             \
  __cap_rights_get(CAP_RIGHTS_VERSION, (fd), (rights))

/**
 * @brief Limit allowed ioctl commands on a file descriptor.
 * @param[in] fd File descriptor.
 * @param[in] cmds Array of allowed ioctl commands.
 * @param[in] ncmds Number of commands.
 * @return Always 0.
 */
int cap_ioctls_limit(int fd, const unsigned long *cmds, size_t ncmds);

/**
 * @brief Retrieve allowed ioctl commands on a file descriptor.
 * @param[in] fd File descriptor.
 * @param[out] cmds Output array of commands.
 * @param[in] maxcmds Maximum commands to store.
 * @return CAP_IOCTLS_ALL.
 */
ssize_t cap_ioctls_get(int fd, unsigned long *cmds, size_t maxcmds);

/**
 * @brief Limit allowed fcntl commands on a file descriptor.
 * @param[in] fd File descriptor.
 * @param[in] fcntlrights Bitmask of CAP_FCNTL_* flags.
 * @return Always 0.
 */
int cap_fcntls_limit(int fd, uint32_t fcntlrights);

/**
 * @brief Retrieve allowed fcntl commands on a file descriptor.
 * @param[in] fd File descriptor.
 * @param[out] fcntlrightsp Pointer receiving CAP_FCNTL_ALL.
 * @return Always 0.
 */
int cap_fcntls_get(int fd, uint32_t *fcntlrightsp);

#ifndef CAP_FCHROOT
/** @brief Capability right for fchroot(). */
#define CAP_FCHROOT ((uint64_t)0x200UL)
#endif
#ifndef CAP_INOTIFY_ADD
/** @brief Capability right for inotify_add_watch(). */
#define CAP_INOTIFY_ADD ((uint64_t)0x400UL)
/** @brief Capability right for inotify_rm_watch(). */
#define CAP_INOTIFY_RM ((uint64_t)0x800UL)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_CAPSICUM_H */
