#ifndef NAMESPACE_WIN_SHIM_H
#define NAMESPACE_WIN_SHIM_H

/**
 * @file namespace_win_shim.h
 * @brief Cross-platform Windows/MSVC POSIX namespace and compiler intrinsic
 * shim header.
 */

/* Architecture detection for minimal Win32 SDK headers */
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) ||           \
    defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
#ifndef _AMD64_
#define _AMD64_
#endif
#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(_M_IX86)
#ifndef _X86_
#define _X86_
#endif
#elif defined(__arm__) || defined(_M_ARM) || defined(_M_ARMT)
#ifndef _ARM_
#define _ARM_
#endif
#elif defined(__aarch64__) || defined(_M_ARM64)
#ifndef _ARM64_
#define _ARM64_
#endif
#endif

#ifndef NAMESPACE_MINIMAL_WINDOWS_INCLUDES
#define NAMESPACE_MINIMAL_WINDOWS_INCLUDES 1
#endif

#ifndef AUTO_WIN_MSVC_SKIP_IOVEC
#define AUTO_WIN_MSVC_SKIP_IOVEC 1
#endif

/* clang-format off */
#if defined(_MSC_VER) || defined(_WIN32)
#if !defined(_MSC_VER) || _MSC_VER >= 1800
#include <inttypes.h>
#endif
#if defined(NAMESPACE_MINIMAL_WINDOWS_INCLUDES)
#include <BaseTsd.h>
#include <direct.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <process.h>
#else
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <direct.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <process.h>
#include <winsock2.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#if defined(_MSC_VER)
#include <intrin.h>
#include <stdlib.h>
#endif
#endif
#include "linux-getopt.h"
#include "posix-arpa-inet.h"
#include "posix-core.h"
#include "posix-dirent.h"
#include "posix-netdb.h"
#include "posix-netinet-in.h"
#include "posix-netinet-tcp.h"
#include "posix-poll.h"
#include "posix-signal.h"
#include "posix-sockets.h"
#include "posix-stat.h"
#include "posix-time.h"
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef PRId64
#define PRId64 "I64d"
#endif
#ifndef PRIu64
#define PRIu64 "I64u"
#endif
#ifndef PRIx64
#define PRIx64 "I64x"
#endif
#ifndef PRIz
#define PRIz "I"
#endif

#ifndef AUTO_WIN_MSVC_NO_FUNCTION_MACROS
#ifndef open
#define open _open
#endif
#ifndef read
#define read posix_read
#endif
#ifndef write
#define write posix_write
#endif
#ifndef lseek
#define lseek _lseek
#endif
#ifndef unlink
#define unlink _unlink
#endif
#ifndef access
#define access _access
#endif
#ifndef isatty
#define isatty _isatty
#endif
#ifndef fileno
#define fileno _fileno
#endif
#ifndef chdir
#define chdir _chdir
#endif
#ifndef getcwd
#define getcwd _getcwd
#endif
#ifndef rmdir
#define rmdir _rmdir
#endif
#ifndef mkdir
#define mkdir(path, mode) _mkdir(path)
#endif
#ifndef strdup
#define strdup _strdup
#endif
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif
#ifndef strncasecmp
#define strncasecmp _strnicmp
#endif
#ifndef getpid
#define getpid _getpid
#endif
#ifndef popen
#define popen _popen
#endif
#ifndef pclose
#define pclose _pclose
#endif
#ifndef putenv
#define putenv _putenv
#endif
#endif /* AUTO_WIN_MSVC_NO_FUNCTION_MACROS */

#ifndef PATH_MAX
#ifdef MAX_PATH
#define PATH_MAX MAX_PATH
#else
#define PATH_MAX 260
#endif
#endif

#ifndef F_OK
#define F_OK 0
#define W_OK 2
#define R_OK 4
#endif

#ifndef S_IRUSR
#define S_IRUSR _S_IREAD
#define S_IWUSR _S_IWRITE
#endif

#ifndef sleep
#define sleep(x) Sleep((x) * 1000)
#endif
#ifndef usleep
#define usleep(x) Sleep(((x) + 999) / 1000)
#endif

#endif /* defined(_MSC_VER) || defined(_WIN32) */

#undef LoadString

#if !defined(__GNUC__) && !defined(__clang__)
#if !defined(__GNUC__) && !defined(__clang__)
#ifndef __attribute__
#define __attribute__(x)
#endif
#endif
#endif
#ifndef __typeof
#define __typeof typeof
#endif
#ifndef __thread
#if defined(_MSC_VER)
#define __thread __declspec(thread)
#else
#define __thread
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
/** @brief 64-bit unsigned integer type for intrinsic polyfills on MSVC. */
typedef unsigned __int64 namespace_win_uint64_t;
#elif defined(__GNUC__)
/** @brief 64-bit unsigned integer type for intrinsic polyfills on GCC/Clang. */
__extension__ typedef unsigned long long namespace_win_uint64_t;
#else
/** @brief Fallback unsigned integer type for intrinsic polyfills. */
typedef unsigned long namespace_win_uint64_t;
#endif

/**
 * @brief Counts the number of set 1-bits in a 64-bit unsigned integer.
 * @param[in] val 64-bit unsigned value to inspect.
 * @return Number of set bits (0 to 64).
 */
static
#if defined(_MSC_VER) || defined(__WATCOMC__)
    __inline
#elif defined(__GNUC__)
    __inline__
#endif
    int posix_builtin_popcountll(namespace_win_uint64_t val) {
  int count = 0;
  while (val != 0) {
    val &= (val - 1U);
    count++;
  }
  return count;
}

/**
 * @brief Counts trailing zero bits in a 64-bit unsigned integer.
 * @param[in] val 64-bit unsigned value to inspect.
 * @return Index of least significant set bit (0 to 63), or 64 when val is 0.
 */
static
#if defined(_MSC_VER) || defined(__WATCOMC__)
    __inline
#elif defined(__GNUC__)
    __inline__
#endif
    int posix_builtin_ctzll(namespace_win_uint64_t val) {
  int count = 0;
  if (val == 0) {
    return 64;
  }
  while ((val & 1U) == 0) {
    val >>= 1;
    count++;
  }
  return count;
}

#if defined(_MSC_VER)
#ifndef __builtin_expect
#define __builtin_expect(exp, c) (exp)
#endif
#ifndef __builtin_bswap32
#define __builtin_bswap32 _byteswap_ulong
#endif
#ifndef __builtin_bswap64
#define __builtin_bswap64 _byteswap_uint64
#endif
#ifndef __builtin_popcount
#define __builtin_popcount __popcnt
#endif
#ifndef __builtin_popcountll
#define __builtin_popcountll posix_builtin_popcountll
#endif
#ifndef __builtin_ctzll
#define __builtin_ctzll posix_builtin_ctzll
#endif
#endif /* _MSC_VER */

#ifndef SIGPIPE
#define SIGPIPE 13
#endif
#ifndef _MODE_T_
#define _MODE_T_
#endif
#ifndef SSIZE_MAX
#define SSIZE_MAX LONG_MAX
#endif
#ifndef SIGCHLD
#define SIGCHLD 20
#endif

#ifndef setenv
#if defined(_MSC_VER)
#define setenv(k, v, o) _putenv_s(k, v)
#endif
#endif
#ifndef unsetenv
#if defined(_MSC_VER)
#define unsetenv(k) _putenv_s(k, "")
#endif
#endif

#ifndef sysconf
#define sysconf(x) (x)
#endif
#ifndef SIGUSR1
#define SIGUSR1 10
#endif
#ifndef _SC_OPEN_MAX
#define _SC_OPEN_MAX 4
#endif
#ifndef SIGUSR2
#define SIGUSR2 12
#endif
#ifndef SIGHUP
#define SIGHUP 1
#endif
#ifndef SIGALRM
#define SIGALRM 14
#endif
#ifndef WNOHANG
#define WNOHANG 1
#endif
#ifndef O_NOFOLLOW
#define O_NOFOLLOW 0
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY 0
#endif
#ifndef AT_REMOVEDIR
#define AT_REMOVEDIR 0x200
#endif
#ifndef RSYNC_RSH
#define RSYNC_RSH "ssh"
#endif
#ifndef NOBODY_USER
#define NOBODY_USER "nobody"
#endif
#ifndef NOBODY_GROUP
#define NOBODY_GROUP "nobody"
#endif
#ifndef RSYNCD_SYSCONF
#define RSYNCD_SYSCONF "/etc/rsyncd.conf"
#endif
#ifndef RSYNC_PATH
#define RSYNC_PATH "rsync"
#endif
#ifndef DEFAULT_CVSIGNORE
#define DEFAULT_CVSIGNORE ".svn .git .hg .bzr .cvs"
#endif

#if defined(_WIN32) || defined(_MSC_VER)
#ifndef _SSIZE_T_DEFINED
typedef intptr_t ssize_t;
#define _SSIZE_T_DEFINED
#endif
#if !defined(_MODE_T_DEFINED) && !defined(_MODE_T_DEFINED_) &&                 \
    !defined(_MODE_T_)
#define _MODE_T_DEFINED
#define _MODE_T_DEFINED_
#define _MODE_T_
typedef unsigned short mode_t;
#endif
#ifndef _UID_T_DEFINED
typedef int uid_t;
#define _UID_T_DEFINED
#endif
#ifndef _GID_T_DEFINED
typedef int gid_t;
#define _GID_T_DEFINED
#endif
#ifndef _PID_T_DEFINED
typedef int pid_t;
#define _PID_T_DEFINED
#endif
#endif

#ifndef _UCHAR_T_DEFINED
typedef unsigned char uchar;
#define _UCHAR_T_DEFINED
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NAMESPACE_WIN_SHIM_H */
