#ifndef BSD_LIBUTIL_H
#define BSD_LIBUTIL_H

/**
 * @file libutil.h
 * @brief FreeBSD <libutil.h> humanize_number, expand_number, hexdump, pidfile,
 * and kinfo polyfills.
 */

/* clang-format off */
#include <sys/types.h>
#if defined(__APPLE__) && defined(__has_include)
#if __has_include(<sys/user.h>)
#include <sys/user.h>
#endif
#elif defined(__linux__)
#include <pty.h>
#endif
#include <stddef.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#else
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__linux__)
/**
 * @brief Prepare a tty for a login session.
 * @param[in] fd File descriptor of the tty.
 * @return 0 on success, -1 on failure.
 */
int login_tty(int fd);
#endif

/** @brief Use one decimal place if value is less than 10. */
#define HN_DECIMAL 0x01
/** @brief Do not put a space between number and prefix. */
#define HN_NOSPACE 0x02
/** @brief Use 'B' prefix when scale index is 0. */
#define HN_B 0x04
/** @brief Divide by 1000 instead of 1024. */
#define HN_DIVISOR_1000 0x08
/** @brief Format with IEC binary prefixes (Ki, Mi, ...). */
#define HN_IEC_PREFIXES 0x10
/** @brief Return scale index instead of formatting string. */
#define HN_GETSCALE 0x10
/** @brief Automatically select best scale factor. */
#define HN_AUTOSCALE 0x20

/** @brief Bitmask extracting column width in hexdump flags. */
#define HD_COLUMN_MASK 0xff
/** @brief Bitmask extracting ASCII delimiter byte in hexdump flags. */
#define HD_DELIM_MASK 0xff00
/** @brief Omit offset column in hexdump output. */
#define HD_OMIT_COUNT (1 << 16)
/** @brief Omit hexadecimal byte columns in hexdump output. */
#define HD_OMIT_HEX (1 << 17)
/** @brief Omit ASCII character representation in hexdump output. */
#define HD_OMIT_CHARS (1 << 18)

/** @brief UUCP lock already held by another live process. */
#define UU_LOCK_INUSE 1
/** @brief UUCP lock operation succeeded. */
#define UU_LOCK_OK 0
/** @brief UUCP lock failed opening lockfile. */
#define UU_LOCK_OPEN_ERR -1
/** @brief UUCP lock failed reading lockfile. */
#define UU_LOCK_READ_ERR -2
/** @brief UUCP lock failed creating temporary lockfile. */
#define UU_LOCK_CREAT_ERR -3
/** @brief UUCP lock failed writing PID to lockfile. */
#define UU_LOCK_WRITE_ERR -4
/** @brief UUCP lock failed linking lockfile. */
#define UU_LOCK_LINK_ERR -5
/** @brief UUCP lock failed trying to acquire lockfile. */
#define UU_LOCK_TRY_ERR -6
/** @brief UUCP lock not owned by calling process. */
#define UU_LOCK_OWNER_ERR -7

/** @brief Hostname resolved and verified. */
#define HOSTNAME_FOUND 0
/** @brief Hostname resolved to a non-matching forward address. */
#define HOSTNAME_INCORRECTNAME 1
/** @brief Invalid network address passed to realhostname. */
#define HOSTNAME_INVALIDADDR 2
/** @brief Reverse DNS returned an invalid hostname. */
#define HOSTNAME_INVALIDNAME 3

/** @brief Opaque PID file handle. */
struct pidfh {
  int pf_fd;          /**< Open file descriptor for the PID file. */
  char pf_path[1024]; /**< NUL-terminated path to the PID file. */
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

/**
 * @brief Return the local packages base path (LOCALBASE or /usr/local).
 * @return Non-NULL NUL-terminated path string.
 */
const char *getlocalbase(void);

struct fstab;
struct dqblk;
struct quotafile;

/**
 * @brief Open filesystem quota file.
 * @param[in] fs Filesystem table entry.
 * @param[in] quotatype Quota type (user or group).
 * @param[in] openflags File open flags.
 * @return Quota handle, or NULL if unsupported.
 */
struct quotafile *quota_open(struct fstab *fs, int quotatype, int openflags);

/**
 * @brief Read quota record for ID.
 * @param[in] qf Quota file handle.
 * @param[out] dqb Destination quota block record.
 * @param[in] id User or group ID.
 * @return 0 on success, -1 on error.
 */
int quota_read(struct quotafile *qf, struct dqblk *dqb, int id);

/**
 * @brief Close quota file handle.
 * @param[in] qf Quota file handle.
 */
void quota_close(struct quotafile *qf);

/** @brief Parse master.passwd format flags. */
#define PWSCAN_MASTER 0x01
/** @brief Emit warnings during password line parsing. */
#define PWSCAN_WARN 0x02

struct passwd;

/**
 * @brief Copy password file updating or removing an entry.
 * @param[in] ffd Source file descriptor.
 * @param[in] tfd Target file descriptor.
 * @param[in] pw Updated passwd entry.
 * @param[in] old_pw Previous passwd entry.
 * @return 0 on success, -1 on error.
 */
int pw_copy(int ffd, int tfd, const struct passwd *pw, struct passwd *old_pw);

/**
 * @brief Duplicate a passwd structure into a single allocation.
 * @param[in] pw Source passwd structure.
 * @return Duplicated structure, or NULL on error.
 */
struct passwd *pw_dup(const struct passwd *pw);

/**
 * @brief Invoke an editor on the temporary master password file.
 * @param[in] notsetuid Non-zero to drop privileges in editor.
 * @return 0 on success, -1 on error.
 */
int pw_edit(int notsetuid);

/**
 * @brief Compare two passwd structures for equality.
 * @param[in] pw1 First passwd structure.
 * @param[in] pw2 Second passwd structure.
 * @return Non-zero if equal, 0 otherwise.
 */
int pw_equal(const struct passwd *pw1, const struct passwd *pw2);

/**
 * @brief Clean up state left over from password database operations.
 */
void pw_fini(void);

/**
 * @brief Initialize password database directory and master filename state.
 * @param[in] dir Directory path.
 * @param[in] master Master password filename.
 * @return 0 on success, -1 on error.
 */
int pw_init(const char *dir, const char *master);

/**
 * @brief Initialize a passwd structure with default empty/unset fields.
 * @param[out] pw Structure to initialize.
 */
void pw_initpwd(struct passwd *pw);

/**
 * @brief Format a passwd structure as a master.passwd line.
 * @param[in] pw Source passwd structure.
 * @return Allocated string, or NULL if unsupported.
 */
char *pw_make(const struct passwd *pw);

/**
 * @brief Format a passwd structure as a legacy V7 passwd line.
 * @param[in] pw Source passwd structure.
 * @return Allocated string, or NULL if unsupported.
 */
char *pw_make_v7(const struct passwd *pw);

/**
 * @brief Rebuild the password database via pwd_mkdb.
 * @param[in] user Optional username to update.
 * @return 0 on success, -1 on error.
 */
int pw_mkdb(const char *user);

/**
 * @brief Lock the master password file for editing.
 * @return Lock descriptor, or -1 on error.
 */
int pw_lock(void);

/**
 * @brief Internal scanner parsing a mutable passwd line in place.
 * @param[in,out] bp Mutable password line buffer.
 * @param[out] pw Destination passwd structure.
 * @param[in] flags PWSCAN_* flags.
 * @return 1 on success, 0 on failure.
 */
int __pw_scan(char *bp, struct passwd *pw, int flags);

/**
 * @brief Parse a passwd or master.passwd line into a passwd structure.
 * @param[in] line Input password line.
 * @param[in] flags PWSCAN_* flags.
 * @return Parsed passwd pointer, or NULL on failure.
 */
struct passwd *pw_scan(const char *line, int flags);

/**
 * @brief Return the temporary password filename currently in use.
 * @return Path to temporary password file.
 */
const char *pw_tempname(void);

/**
 * @brief Create and open a temporary password file.
 * @param[in] mfd Master file descriptor.
 * @return File descriptor, or -1 on error.
 */
int pw_tmp(int mfd);

/**
 * @brief Overrides allocator hooks used by bsd-libutil for OOM testing.
 * @param[in] calloc_fn Custom calloc function, or NULL to restore default.
 * @param[in] strdup_fn Custom strdup function, or NULL to restore default.
 */
void bsd_libutil_set_alloc_hooks(void *(*calloc_fn)(size_t, size_t),
                                 char *(*strdup_fn)(const char *));

/**
 * @brief Overrides file write hook used by pidfile_write for error testing.
 * @param[in] write_fn Custom write function, or NULL to restore default.
 */
void bsd_libutil_set_write_hook(int (*write_fn)(int, const void *,
                                                unsigned int));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_LIBUTIL_H */
