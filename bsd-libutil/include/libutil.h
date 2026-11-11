#ifndef BSD_LIBUTIL_H
#define BSD_LIBUTIL_H

/**
 * @file libutil.h
 * @brief FreeBSD <libutil.h> humanize_number, expand_number, hexdump, pidfile,
 * and kinfo polyfills.
 */

/* clang-format off */
#include <sys/types.h>
#include <stddef.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/* humanize_number(3) flags */
#define HN_DECIMAL 0x01
#define HN_NOSPACE 0x02
#define HN_B 0x04
#define HN_DIVISOR_1000 0x08
#define HN_IEC_PREFIXES 0x10
#define HN_GETSCALE 0x10
#define HN_AUTOSCALE 0x20

/* hexdump(3) flags */
#define HD_COLUMN_MASK 0xff
#define HD_DELIM_MASK 0xff00
#define HD_OMIT_COUNT (1 << 16)
#define HD_OMIT_HEX (1 << 17)
#define HD_OMIT_CHARS (1 << 18)

/* UUCP lock return codes */
#define UU_LOCK_INUSE 1
#define UU_LOCK_OK 0
#define UU_LOCK_OPEN_ERR -1
#define UU_LOCK_READ_ERR -2
#define UU_LOCK_CREAT_ERR -3
#define UU_LOCK_WRITE_ERR -4
#define UU_LOCK_LINK_ERR -5
#define UU_LOCK_TRY_ERR -6
#define UU_LOCK_OWNER_ERR -7

/* realhostname_sa return codes */
#define HOSTNAME_FOUND 0
#define HOSTNAME_INCORRECTNAME 1
#define HOSTNAME_INVALIDADDR 2
#define HOSTNAME_INVALIDNAME 3

/** @brief Opaque PID file handle. */
struct pidfh {
  int pf_fd;
  char pf_path[1024];
};

struct kinfo_proc;
struct kinfo_file;
struct kinfo_vmentry;
struct sockaddr;

/**
 * @brief Format a 64-bit number into a human-readable string with SI/IEC
 * prefixes.
 * @param[out] buf Destination buffer.
 * @param[in] len Size of buf.
 * @param[in] bytes Value to format.
 * @param[in] suffix Optional unit suffix string (e.g. "B" or "").
 * @param[in] scale Scale factor or HN_AUTOSCALE / HN_GETSCALE.
 * @param[in] flags Bitmask of HN_* flags.
 * @return Number of characters written (or scale index if HN_GETSCALE), or -1
 * on error.
 */
int humanize_number(char *buf, size_t len, int64_t bytes, const char *suffix,
                    int scale, int flags);

/**
 * @brief Parse a human-readable number string with K/M/G/T/P/E suffixes.
 * @param[in] buf Input string.
 * @param[out] num Pointer receiving parsed 64-bit unsigned value.
 * @return 0 on success, -1 on error with errno set.
 */
int expand_number(const char *buf, uint64_t *num);

/**
 * @brief Dump a block of memory in canonical hex+ASCII format to stdout.
 * @param[in] ptr Buffer to dump.
 * @param[in] length Number of bytes to dump.
 * @param[in] hdr Optional line prefix string.
 * @param[in] flags HD_* formatting flags.
 */
void hexdump(const void *ptr, int length, const char *hdr, int flags);

/**
 * @brief Open and lock a PID file.
 * @param[in] path Path to PID file, or NULL for default.
 * @param[in] mode File creation mode.
 * @param[out] pidptr Optional pointer receiving owning PID if already locked.
 * @return Allocated pidfh handle, or NULL on error.
 */
struct pidfh *pidfile_open(const char *path, mode_t mode, pid_t *pidptr);

/**
 * @brief Write current process PID into open PID file.
 * @param[in] pfh PID file handle.
 * @return 0 on success, -1 on error.
 */
int pidfile_write(struct pidfh *pfh);

/**
 * @brief Close a PID file without removing it.
 * @param[in] pfh PID file handle.
 * @return 0 on success, -1 on error.
 */
int pidfile_close(struct pidfh *pfh);

/**
 * @brief Remove and close a PID file.
 * @param[in] pfh PID file handle.
 * @return 0 on success, -1 on error.
 */
int pidfile_remove(struct pidfh *pfh);

/**
 * @brief Return underlying file descriptor of a PID file handle.
 * @param[in] pfh PID file handle.
 * @return File descriptor, or -1 on error.
 */
int pidfile_fileno(const struct pidfh *pfh);

/**
 * @brief Reliably open and lock a file.
 * @param[in] path File path.
 * @param[in] flags Open flags.
 * @return File descriptor, or -1 on error.
 */
int flopen(const char *path, int flags, ...);

/**
 * @brief Reliably open and lock a file relative to a directory descriptor.
 * @param[in] dirfd Directory descriptor.
 * @param[in] path File path.
 * @param[in] flags Open flags.
 * @return File descriptor, or -1 on error.
 */
int flopenat(int dirfd, const char *path, int flags, ...);

/**
 * @brief Retrieve all processes in the system.
 * @param[out] cntp Pointer receiving number of processes.
 * @return Allocated array of kinfo_proc structures, or NULL.
 */
struct kinfo_proc *kinfo_getallproc(int *cntp);

/**
 * @brief Retrieve process info for a specific PID.
 * @param[in] pid Process ID.
 * @return Allocated kinfo_proc structure, or NULL.
 */
struct kinfo_proc *kinfo_getproc(pid_t pid);

/**
 * @brief Retrieve open files for a specific PID.
 * @param[in] pid Process ID.
 * @param[out] cntp Pointer receiving file count.
 * @return Allocated array of kinfo_file structures, or NULL.
 */
struct kinfo_file *kinfo_getfile(pid_t pid, int *cntp);

/**
 * @brief Retrieve virtual memory map for a specific PID.
 * @param[in] pid Process ID.
 * @param[out] cntp Pointer receiving entry count.
 * @return Allocated array of kinfo_vmentry structures, or NULL.
 */
struct kinfo_vmentry *kinfo_getvmmap(pid_t pid, int *cntp);

/**
 * @brief Acquire UUCP device lock.
 * @param[in] ttyname Device name.
 * @return UU_LOCK_OK (0) on success.
 */
int uu_lock(const char *ttyname);

/**
 * @brief Release UUCP device lock.
 * @param[in] ttyname Device name.
 * @return 0 on success.
 */
int uu_unlock(const char *ttyname);

/**
 * @brief Transfer UUCP device lock to another PID.
 * @param[in] ttyname Device name.
 * @param[in] pid New owner PID.
 * @return UU_LOCK_OK (0) on success.
 */
int uu_lock_txfr(const char *ttyname, pid_t pid);

/**
 * @brief Format UUCP lock error code as string.
 * @param[in] uu_lockresult UU_LOCK_* code.
 * @return Error string.
 */
const char *uu_lockerr(int uu_lockresult);

/**
 * @brief Remove local domain suffix from a hostname.
 * @param[in,out] fullhost Hostname buffer to trim in place.
 * @param[in] hostsize Maximum length of resulting hostname.
 */
void trimdomain(char *fullhost, int hostsize);

/**
 * @brief Resolve IPv4 address to verified hostname.
 * @param[out] host Output hostname buffer.
 * @param[in] hsize Size of host buffer.
 * @param[in] ip Pointer to in_addr.
 * @return HOSTNAME_FOUND (0).
 */
int realhostname(char *host, size_t hsize, const void *ip);

/**
 * @brief Resolve sockaddr to verified hostname.
 * @param[out] host Output hostname buffer.
 * @param[in] hsize Size of host buffer.
 * @param[in] addr Socket address pointer.
 * @param[in] addrlen Socket address length.
 * @return HOSTNAME_FOUND (0).
 */
int realhostname_sa(char *host, size_t hsize, struct sockaddr *addr,
                    int addrlen);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_LIBUTIL_H */
