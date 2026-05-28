#ifndef NAMESPACE_WIN_SHIM_H
#define NAMESPACE_WIN_SHIM_H

/* Architecture detection for Windows headers */
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) ||           \
    defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
#define _AMD64_
#elif defined(i386) || defined(__i386) || defined(__i386__) ||                 \
    defined(__i386__) || defined(_M_IX86)
#define _X86_
#elif defined(__arm__) || defined(_M_ARM) || defined(_M_ARMT)
#define _ARM_
#elif defined(__aarch64__) || defined(_M_ARM64)
#define _ARM64_
#endif

#if defined(_MSC_VER) || defined(_WIN32)

/* Number formatting shims for long long and size_t on older MSVC runtimes */
/* clang-format off */
#include <inttypes.h>
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
/* PRIz is not standard, but useful for size_t cross-platform if %zu isn't
 * supported */
#define PRIz "I"
#endif

/* -------------------------------------------------------------------------- */
/* Mode: Only-include-what-we-need (Comment out below #define to use full
 * windows.h mode) */
#define NAMESPACE_MINIMAL_WINDOWS_INCLUDES 1
/* -------------------------------------------------------------------------- */

#if defined(NAMESPACE_MINIMAL_WINDOWS_INCLUDES)

/* Standard MSVC CRT Headers */
#include <BaseTsd.h>
#include <direct.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <process.h>

/* Minimal Synchapi for Sleep (if needed without Windows.h) */
/* #include <synchapi.h> */

#else

/* Mode: #ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h> */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
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

#endif /* NAMESPACE_MINIMAL_WINDOWS_INCLUDES */

/* POSIX Type Shims */
#include <sys/stat.h>
#include <sys/types.h>




/* POSIX Function Shims via Macros */
#define open _open
/* #define close _close */
#define read posix_read
#define write posix_write
#define lseek _lseek
#define unlink _unlink
#define access _access
#define isatty _isatty
#define fileno _fileno

#define chdir _chdir
#define getcwd _getcwd
#define rmdir _rmdir
/* POSIX mkdir takes two arguments, MSVC _mkdir takes one */
#define mkdir(path, mode) _mkdir(path)

#define strdup _strdup
#define strcasecmp _stricmp
#define strncasecmp _strnicmp

#define getpid _getpid
#define popen _popen
#define pclose _pclose
#define putenv _putenv

/* POSIX Macro Shims */
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
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

/* Time/Wait Shims */
#ifndef sleep
/* Windows Sleep takes milliseconds, POSIX sleep takes seconds */
#define sleep(x) Sleep((x) * 1000)
#endif
#ifndef usleep
/* Approximate mapping for usleep */
#define usleep(x) Sleep(((x) + 999) / 1000)
#endif

/* GCC/Clang Compiler Intrinsics Polyfills for MSVC */
#if defined(_MSC_VER)
#include <intrin.h>
#include <stdlib.h>

#ifndef __builtin_expect
/** \brief __builtin_expect macro. */
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
#if defined(_M_X64) || defined(_M_AMD64)
#define __builtin_popcountll __popcnt64
#else
/** \brief posix_builtin_popcountll function. */
static __inline int posix_builtin_popcountll(unsigned __int64 val) {
  return __popcnt((unsigned int)(val & 0xFFFFFFFF)) +
         __popcnt((unsigned int)(val >> 32));
}
#define __builtin_popcountll posix_builtin_popcountll
#endif
#endif

#ifndef __builtin_ctzll
/** \brief posix_builtin_ctzll function. */
static __inline int posix_builtin_ctzll(unsigned __int64 val) {
  unsigned long index;
#if defined(_M_X64) || defined(_M_AMD64)
  if (_BitScanForward64(&index, val)) {
    return (int)index;
  }
#else
  if (_BitScanForward(&index, (unsigned long)val)) {
    return (int)index;
  }
  if (_BitScanForward(&index, (unsigned long)(val >> 32))) {
    return (int)(index + 32);
  }
#endif
  return 64; /* Undefined for 0, returning 64 is a common fallback */
}
#define __builtin_ctzll posix_builtin_ctzll
#endif

#endif /* _MSC_VER */

#endif /* defined(_MSC_VER) || defined(_WIN32) */

#endif /* NAMESPACE_WIN_SHIM_H */
#undef LoadString

#ifndef __attribute__
#define __attribute__(x)
#endif
#ifndef __typeof
#define __typeof typeof
#endif
#ifndef __thread
#define __thread __declspec(thread)
#endif
#define AUTO_WIN_MSVC_SKIP_IOVEC 1
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

#ifdef __cplusplus
extern "C" {
#endif

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
#define setenv(k, v, o) _putenv_s(k, v)
#endif
#ifndef unsetenv
#define unsetenv(k) _putenv_s(k, "")
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

#ifndef SIGUSR1
#define SIGUSR1 10
#endif
#ifndef _SC_OPEN_MAX
#define _SC_OPEN_MAX 4
#endif
#ifndef sysconf
#define sysconf(x) (x)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
#define SIGUSR2 12
#define SIGHUP 1
#define SIGALRM 14
#define WNOHANG 1
#define O_NOFOLLOW 0
#define O_DIRECTORY 0
#define AT_REMOVEDIR 0x200
#define RSYNC_RSH "ssh"
#define NOBODY_USER "nobody"
#define NOBODY_GROUP "nobody"
#define RSYNCD_SYSCONF "/etc/rsyncd.conf"
#define RSYNC_PATH "rsync"
#define DEFAULT_CVSIGNORE ".svn .git .hg .bzr .cvs"

#ifndef _SSIZE_T_DEFINED
typedef intptr_t ssize_t;
#define _SSIZE_T_DEFINED
#endif
#ifndef _UCHAR_T_DEFINED
typedef unsigned char uchar;
#define _UCHAR_T_DEFINED
#endif
#ifndef _MODE_T_DEFINED
typedef unsigned short mode_t;
#define _MODE_T_DEFINED
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
