#ifndef BSD_SYSDECODE_H
#define BSD_SYSDECODE_H

/**
 * @file sysdecode.h
 * @brief FreeBSD system-call argument decoding library (<sysdecode.h>)
 * polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#include <stdio.h>
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

enum sysdecode_abi {
  SYSDECODE_ABI_UNKNOWN = 0,
  SYSDECODE_ABI_FREEBSD = 1,
  SYSDECODE_ABI_FREEBSD32 = 2,
  SYSDECODE_ABI_LINUX = 3,
  SYSDECODE_ABI_LINUX32 = 4
};

/** @brief Translate an ABI-specific errno value to FreeBSD native errno. */
int sysdecode_abi_to_freebsd_errno(enum sysdecode_abi abi, int error);
/** @brief Translate a FreeBSD native errno value to ABI-specific errno. */
int sysdecode_freebsd_to_abi_errno(enum sysdecode_abi abi, int error);
/** @brief Return the symbolic name of a system call number for an ABI. */
const char *sysdecode_syscallnames(enum sysdecode_abi abi, unsigned int code);
/** @brief Return the symbolic name of an ioctl command code. */
const char *sysdecode_ioctlname(unsigned long val);
/** @brief Return the symbolic name of a signal number. */
const char *sysdecode_signame(int sig);
/** @brief Return the symbolic name of an fcntl command. */
const char *sysdecode_fcntl_cmd(int cmd);
/** @brief Return the symbolic name of a getsockopt/setsockopt level. */
const char *sysdecode_sockopt_level(int level);
/** @brief Return the symbolic name of a socket option within a level. */
const char *sysdecode_sockopt_name(int level, int optname);
/** @brief Decode open(2) flags symbolically to fp. */
int sysdecode_open_flags(FILE *fp, int flags, int *rem);
/** @brief Decode mmap(2) protection bits symbolically to fp. */
int sysdecode_mmap_prot(FILE *fp, int prot, int *rem);
/** @brief Decode mmap(2) flags symbolically to fp. */
int sysdecode_mmap_flags(FILE *fp, int flags, int *rem);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYSDECODE_H */
