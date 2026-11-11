#ifndef CAPSICUM_HELPERS_H
#define CAPSICUM_HELPERS_H

/**
 * @file capsicum_helpers.h
 * @brief FreeBSD Capsicum helper functions polyfill for Windows and DOS.
 */

/* clang-format off */
#include <sys/capsicum.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Ignore EBADF when limiting streams. */
#define CAPH_IGNORE_EBADF 0x0001
/** @brief Allow reading on stream. */
#define CAPH_READ 0x0002
/** @brief Allow writing on stream. */
#define CAPH_WRITE 0x0004
/** @brief Allow path lookups on descriptor. */
#define CAPH_LOOKUP 0x0008

/**
 * @brief Enter capability mode helper (no-op).
 * @return Always 0.
 */
static __inline int caph_enter(void) { return 0; }

/**
 * @brief Enter capability mode when Casper is active (no-op).
 * @return Always 0.
 */
static __inline int caph_enter_casper(void) { return 0; }

/**
 * @brief Limit descriptor rights helper (no-op).
 * @param[in] fd File descriptor.
 * @param[in] rights Rights pointer.
 * @return Always 0.
 */
static __inline int caph_rights_limit(int fd, const cap_rights_t *rights) {
  (void)fd;
  (void)rights;
  return 0;
}

/**
 * @brief Limit descriptor ioctls helper (no-op).
 * @param[in] fd File descriptor.
 * @param[in] cmds Allowed commands.
 * @param[in] ncmds Count of commands.
 * @return Always 0.
 */
static __inline int caph_ioctls_limit(int fd, const unsigned long *cmds,
                                      size_t ncmds) {
  (void)fd;
  (void)cmds;
  (void)ncmds;
  return 0;
}

/**
 * @brief Limit descriptor fcntls helper (no-op).
 * @param[in] fd File descriptor.
 * @param[in] fcntlrights Allowed fcntl bitmask.
 * @return Always 0.
 */
static __inline int caph_fcntls_limit(int fd, uint32_t fcntlrights) {
  (void)fd;
  (void)fcntlrights;
  return 0;
}

/**
 * @brief Limit stdio stream descriptor (no-op).
 * @param[in] fd File descriptor.
 * @param[in] flags CAPH_* flags.
 * @return Always 0.
 */
static __inline int caph_limit_stream(int fd, int flags) {
  (void)fd;
  (void)flags;
  return 0;
}

/**
 * @brief Limit stderr descriptor (no-op).
 * @return Always 0.
 */
static __inline int caph_limit_stderr(void) { return 0; }

/**
 * @brief Limit stdin descriptor (no-op).
 * @return Always 0.
 */
static __inline int caph_limit_stdin(void) { return 0; }

/**
 * @brief Limit stdout descriptor (no-op).
 * @return Always 0.
 */
static __inline int caph_limit_stdout(void) { return 0; }

/**
 * @brief Limit stdin, stdout, and stderr descriptors (no-op).
 * @return Always 0.
 */
static __inline int caph_limit_stdio(void) { return 0; }

/**
 * @brief Cache timezone data prior to entering capability mode (no-op).
 */
static __inline void caph_cache_tzdata(void) {}

/**
 * @brief Cache NLS catalog pages prior to entering capability mode (no-op).
 */
static __inline void caph_cache_catpages(void) {}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CAPSICUM_HELPERS_H */
