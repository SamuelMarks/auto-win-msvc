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

#define CAPRIGHT(idx, bit) ((((uint64_t)1UL) << (57 + (idx))) | (bit))

#define CAP_READ CAPRIGHT(0, ((uint64_t)1UL))
#define CAP_WRITE                                                              \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000002UL))
#define CAP_SEEK_TELL                                                          \
  CAPRIGHT(0, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000004UL))
#define CAP_SEEK                                                               \
  (CAP_SEEK_TELL | (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000008UL))
#define CAP_PREAD (CAP_SEEK | CAP_READ)
#define CAP_PWRITE (CAP_SEEK | CAP_WRITE)
#define CAP_MMAP CAPRIGHT(0, 0x000000000000001((uint64_t)0UL))
#define CAP_MMAP_R (CAP_MMAP | CAP_SEEK | CAP_READ)
#define CAP_MMAP_W (CAP_MMAP | CAP_SEEK | CAP_WRITE)
#define CAP_MMAP_X (CAP_MMAP | CAP_SEEK | 0x000000000000002((uint64_t)0UL))
#define CAP_MMAP_RW (CAP_MMAP_R | CAP_MMAP_W)
#define CAP_MMAP_RX (CAP_MMAP_R | CAP_MMAP_X)
#define CAP_MMAP_WX (CAP_MMAP_W | CAP_MMAP_X)
#define CAP_MMAP_RWX (CAP_MMAP_R | CAP_MMAP_W | CAP_MMAP_X)
#define CAP_CREATE CAPRIGHT(0, 0x000000000000004((uint64_t)0UL))
#define CAP_FEXECVE CAPRIGHT(0, 0x000000000000008((uint64_t)0UL))
#define CAP_FSYNC CAPRIGHT(0, 0x000000000000010((uint64_t)0UL))
#define CAP_FTRUNCATE CAPRIGHT(0, 0x000000000000020((uint64_t)0UL))
#define CAP_LOOKUP CAPRIGHT(0, 0x000000000000040((uint64_t)0UL))
#define CAP_FCHDIR CAPRIGHT(0, 0x000000000000080((uint64_t)0UL))
#define CAP_FCHFLAGS CAPRIGHT(0, 0x000000000000100((uint64_t)0UL))
#define CAP_CHFLAGSAT (CAP_FCHFLAGS | CAP_LOOKUP)
#define CAP_FCHMOD CAPRIGHT(0, 0x000000000000200((uint64_t)0UL))
#define CAP_FCHMODAT (CAP_FCHMOD | CAP_LOOKUP)
#define CAP_FCHOWN CAPRIGHT(0, 0x000000000000400((uint64_t)0UL))
#define CAP_FCHOWNAT (CAP_FCHOWN | CAP_LOOKUP)
#define CAP_FCNTL CAPRIGHT(0, 0x000000000000800((uint64_t)0UL))
#define CAP_FLOCK CAPRIGHT(0, 0x000000000001000((uint64_t)0UL))
#define CAP_FPATHCONF CAPRIGHT(0, 0x000000000002000((uint64_t)0UL))
#define CAP_FSCK CAPRIGHT(0, 0x000000000004000((uint64_t)0UL))
#define CAP_FSTAT CAPRIGHT(0, 0x000000000008000((uint64_t)0UL))
#define CAP_FSTATAT (CAP_FSTAT | CAP_LOOKUP)
#define CAP_FSTATFS CAPRIGHT(0, 0x000000000010000((uint64_t)0UL))
#define CAP_FUTIMES CAPRIGHT(0, 0x000000000020000((uint64_t)0UL))
#define CAP_FUTIMESAT (CAP_FUTIMES | CAP_LOOKUP)
#define CAP_LINKAT_TARGET (CAP_LOOKUP | 0x000000000040000((uint64_t)0UL))
#define CAP_MKDIRAT (CAP_LOOKUP | 0x000000000080000((uint64_t)0UL))
#define CAP_MKFIFOAT (CAP_LOOKUP | 0x000000000100000((uint64_t)0UL))
#define CAP_MKNODAT (CAP_LOOKUP | 0x000000000200000((uint64_t)0UL))
#define CAP_RENAMEAT_SOURCE (CAP_LOOKUP | 0x000000000400000((uint64_t)0UL))
#define CAP_SYMLINKAT (CAP_LOOKUP | 0x000000000800000((uint64_t)0UL))
#define CAP_UNLINKAT (CAP_LOOKUP | 0x000000001000000((uint64_t)0UL))
#define CAP_ACCEPT CAPRIGHT(0, 0x000000002000000((uint64_t)0UL))
#define CAP_BIND CAPRIGHT(0, 0x000000004000000((uint64_t)0UL))
#define CAP_CONNECT CAPRIGHT(0, 0x000000008000000((uint64_t)0UL))
#define CAP_GETPEERNAME CAPRIGHT(0, 0x000000010000000((uint64_t)0UL))
#define CAP_GETSOCKNAME CAPRIGHT(0, 0x000000020000000((uint64_t)0UL))
#define CAP_GETSOCKOPT CAPRIGHT(0, 0x000000040000000((uint64_t)0UL))
#define CAP_LISTEN CAPRIGHT(0, 0x000000080000000((uint64_t)0UL))
#define CAP_PEELOFF CAPRIGHT(0, 0x000000100000000((uint64_t)0UL))
#define CAP_RECV CAP_READ
#define CAP_SEND CAP_WRITE
#define CAP_SETSOCKOPT CAPRIGHT(0, 0x000000200000000((uint64_t)0UL))
#define CAP_SHUTDOWN CAPRIGHT(0, 0x000000400000000((uint64_t)0UL))
#define CAP_BINDAT (CAP_LOOKUP | 0x000000800000000((uint64_t)0UL))
#define CAP_CONNECTAT (CAP_LOOKUP | 0x000001000000000((uint64_t)0UL))
#define CAP_LINKAT_SOURCE (CAP_LOOKUP | 0x000002000000000((uint64_t)0UL))
#define CAP_RENAMEAT_TARGET (CAP_LOOKUP | 0x000004000000000((uint64_t)0UL))
#define CAP_SOCK_CLIENT                                                        \
  (CAP_CONNECT | CAP_GETPEERNAME | CAP_GETSOCKNAME | CAP_GETSOCKOPT |          \
   CAP_PEELOFF | CAP_RECV | CAP_SEND | CAP_SETSOCKOPT | CAP_SHUTDOWN)
#define CAP_SOCK_SERVER                                                        \
  (CAP_ACCEPT | CAP_BIND | CAP_GETPEERNAME | CAP_GETSOCKNAME |                 \
   CAP_GETSOCKOPT | CAP_LISTEN | CAP_PEELOFF | CAP_RECV | CAP_SEND |           \
   CAP_SETSOCKOPT | CAP_SHUTDOWN)
#define CAP_ALL0                                                               \
  CAPRIGHT(0, (((uint64_t)0x000007FFUL << 32) | (uint64_t)0xFFFFFFFFUL))
#define CAP_UNUSED0_44 CAPRIGHT(0, 0x000008000000000((uint64_t)0UL))
#define CAP_UNUSED0_57 CAPRIGHT(0, 0x010000000000000((uint64_t)0UL))

#define CAP_MAC_GET CAPRIGHT(1, ((uint64_t)1UL))
#define CAP_MAC_SET                                                            \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000002UL))
#define CAP_SEM_GETVALUE                                                       \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000004UL))
#define CAP_SEM_POST                                                           \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x00000008UL))
#define CAP_SEM_WAIT CAPRIGHT(1, 0x000000000000001((uint64_t)0UL))
#define CAP_EVENT CAPRIGHT(1, 0x000000000000002((uint64_t)0UL))
#define CAP_KQUEUE_EVENT CAPRIGHT(1, 0x000000000000004((uint64_t)0UL))
#define CAP_IOCTL CAPRIGHT(1, 0x000000000000008((uint64_t)0UL))
#define CAP_TTYHOOK CAPRIGHT(1, 0x000000000000010((uint64_t)0UL))
#define CAP_PDGETPID CAPRIGHT(1, 0x000000000000020((uint64_t)0UL))
#define CAP_PDWAIT CAPRIGHT(1, 0x000000000000040((uint64_t)0UL))
#define CAP_PDKILL CAPRIGHT(1, 0x000000000000080((uint64_t)0UL))
#define CAP_EXTATTR_DELETE CAPRIGHT(1, 0x000000000000100((uint64_t)0UL))
#define CAP_EXTATTR_GET CAPRIGHT(1, 0x000000000000200((uint64_t)0UL))
#define CAP_EXTATTR_LIST CAPRIGHT(1, 0x000000000000400((uint64_t)0UL))
#define CAP_EXTATTR_SET CAPRIGHT(1, 0x000000000000800((uint64_t)0UL))
#define CAP_ACL_CHECK CAPRIGHT(1, 0x000000000001000((uint64_t)0UL))
#define CAP_ACL_DELETE CAPRIGHT(1, 0x000000000002000((uint64_t)0UL))
#define CAP_ACL_GET CAPRIGHT(1, 0x000000000004000((uint64_t)0UL))
#define CAP_ACL_SET CAPRIGHT(1, 0x000000000008000((uint64_t)0UL))
#define CAP_KQUEUE_CHANGE CAPRIGHT(1, 0x000000000010000((uint64_t)0UL))
#define CAP_KQUEUE (CAP_KQUEUE_EVENT | CAP_KQUEUE_CHANGE)
#define CAP_ALL1                                                               \
  CAPRIGHT(1, (((uint64_t)0x00000000UL << 32) | (uint64_t)0x001FFFFFUL))
#define CAP_OPENAT (CAP_LOOKUP | CAP_READ | CAP_WRITE)

#define CAP_FCNTL_GETFL (1 << 3)
#define CAP_FCNTL_SETFL (1 << 4)
#define CAP_FCNTL_GETOWN (1 << 5)
#define CAP_FCNTL_SETOWN (1 << 6)
#define CAP_FCNTL_ALL                                                          \
  (CAP_FCNTL_GETFL | CAP_FCNTL_SETFL | CAP_FCNTL_GETOWN | CAP_FCNTL_SETOWN)

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

cap_rights_t *cap_rights_init(cap_rights_t *rights, ...);
cap_rights_t *cap_rights_set(cap_rights_t *rights, ...);
cap_rights_t *cap_rights_clear(cap_rights_t *rights, ...);
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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_CAPSICUM_H */
