/* posix-types.h - Strict C89 Header */
#ifndef POSIX_TYPES_H
#define POSIX_TYPES_H

/**
 * @file posix-types.h
 * @brief Strict C89 POSIX sys/types.h polyfills and types for MSVC.
 */

/* clang-format off */
#include "auto-win-msvc-error.h"
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <basetsd.h>
#include <sys/types.h>
#include <time.h>

#if defined(__MINGW32__)
/* MinGW natively provides these types, so we prevent redefining them. */
#define _PID_T_DEFINED
#define _SSIZE_T_DEFINED
#define _MODE_T_DEFINED
#define _OFF_T_DEFINED
#define _OFF64_T_DEFINED
#define _USECONDS_T_DEFINED
#define _DEV_T_DEFINED
#define _INO_T_DEFINED
#define _CLOCK_T_DEFINED
#define _TIME_T_DEFINED
#define _CLOCKID_T_DEFINED
#endif

#else
#include <sys/types.h>
#include <time.h>
#if !defined(_MSC_VER) && !defined(_WIN32) && !defined(__MSDOS__) && !defined(__DOS__) && !defined(__WATCOMC__)
#include <unistd.h>
#endif

#if defined(__MINGW32__)
/* MinGW types handled above */
#elif !defined(__MSDOS__) && !defined(__DOS__) && !defined(__WATCOMC__)
#include <sys/socket.h>
#endif

#endif
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(_WIN32) && !defined(__CYGWIN__)) || defined(__MSDOS__) ||         \
    defined(__DOS__) || defined(__WATCOMC__)

#if defined(__GNUC__)
#define POSIX_TYPES_EXTENSION __extension__
#else
#define POSIX_TYPES_EXTENSION
#endif

/**
 * @brief Process ID type.
 * MSVC defines this as int.
 */
#if !defined(_PID_T_DEFINED) && !defined(_PID_T_) && !defined(__WATCOMC__)
#define _PID_T_DEFINED
#define _PID_T_
typedef int pid_t;
#endif

/**
 * @brief Signed size type.
 * MSVC defines this as SSIZE_T from basetsd.h.
 */
#ifndef _MQD_T_DEFINED
#define _MQD_T_DEFINED
typedef int mqd_t;
#endif

#if !defined(_SSIZE_T_DEFINED) && !defined(_SSIZE_T_) && !defined(__WATCOMC__)
#define _SSIZE_T_DEFINED
#define _SSIZE_T_
#if defined(_WIN32)
typedef SSIZE_T ssize_t;
#else
typedef long ssize_t;
#endif
#endif

/**
 * @brief File mode type.
 * MSVC defines this as unsigned short.
 */
#if !defined(_MODE_T_DEFINED) && !defined(_MODE_T_DEFINED_) &&                 \
    !defined(_MODE_T_) && !defined(__WATCOMC__)
#define _MODE_T_DEFINED
#define _MODE_T_DEFINED_
#define _MODE_T_
typedef unsigned short mode_t;
#endif

#ifndef HAVE_MODE_T
#define HAVE_MODE_T 1
#endif
#ifndef HAVE_PID_T
#define HAVE_PID_T 1
#endif
#ifndef HAVE_ID_T
#define HAVE_ID_T 1
#endif
#ifndef HAVE_OFF_T
#define HAVE_OFF_T 1
#endif

#if !defined(_UID_T_DEFINED) && !defined(__WATCOMC__)
#define _UID_T_DEFINED
typedef int uid_t;
#endif
#ifndef HAVE_UID_T
#define HAVE_UID_T 1
#endif

#if !defined(_GID_T_DEFINED) && !defined(__WATCOMC__)
#define _GID_T_DEFINED
typedef int gid_t;
#endif
#ifndef HAVE_GID_T
#define HAVE_GID_T 1
#endif

#ifndef _UCHAR_T_DEFINED
#define _UCHAR_T_DEFINED
typedef unsigned char uchar;
#endif

/**
 * @brief File offset type.
 * MSVC defines this as long in sys/types.h under certain conditions.
 */
#if !defined(_OFF_T_DEFINED) && !defined(__WATCOMC__)
#define _OFF_T_DEFINED
#if defined(_MSC_VER) || defined(__WATCOMC__)
typedef __int64 off_t;
#else
POSIX_TYPES_EXTENSION typedef long long off_t;
#endif
#endif

/**
 * @brief 64-bit file offset type.
 * MSVC defines this as __int64.
 */
#ifndef _OFF64_T_DEFINED
#define _OFF64_T_DEFINED
#if defined(_MSC_VER) || defined(__WATCOMC__)
typedef __int64 off64_t;
#else
POSIX_TYPES_EXTENSION typedef long long off64_t;
#endif
#endif

/**
 * @brief 64-bit signed quad integer type.
 */
#ifndef _QUAD_T_DEFINED
#define _QUAD_T_DEFINED
#if defined(_MSC_VER) || defined(__WATCOMC__)
typedef __int64 quad_t;
/**
 * @brief 64-bit unsigned quad integer type.
 */
typedef unsigned __int64 u_quad_t;
#else
POSIX_TYPES_EXTENSION typedef long long quad_t;
/**
 * @brief 64-bit unsigned quad integer type.
 */
POSIX_TYPES_EXTENSION typedef unsigned long long u_quad_t;
#endif
#endif

/**
 * @brief Microseconds type.
 * MSVC defines this as unsigned int.
 */
#ifndef _USECONDS_T_DEFINED
#define _USECONDS_T_DEFINED
typedef unsigned int useconds_t;
#endif

/**
 * @brief Signed microseconds type.
 * MSVC defines this as long.
 */
#ifndef _SUSECONDS_T_DEFINED
#define _SUSECONDS_T_DEFINED
typedef long suseconds_t;
#endif

/**
 * @brief Device ID type.
 * MSVC defines this as unsigned int in sys/types.h.
 */
#if !defined(_DEV_T_DEFINED) && !defined(__WATCOMC__)
#define _DEV_T_DEFINED
typedef unsigned int dev_t;
#endif

/**
 * @brief Inode number type.
 * MSVC defines this as unsigned short in sys/types.h.
 */
#if !defined(_INO_T_DEFINED) && !defined(__WATCOMC__)
#define _INO_T_DEFINED
typedef unsigned short ino_t;
#endif

/**
 * @brief Number of hard links type.
 * MSVC defines this as short.
 */
#if !defined(_NLINK_T_DEFINED) && !defined(__WATCOMC__)
#define _NLINK_T_DEFINED
typedef short nlink_t;
#endif

/**
 * @brief Socket length type.
 * MSVC defines this as int.
 */
#ifndef _SOCKLEN_T_DEFINED
#define _SOCKLEN_T_DEFINED
typedef int socklen_t;
#endif

/**
 * @brief Socket address family type.
 * MSVC defines ADDRESS_FAMILY as unsigned short.
 */
#ifndef _SA_FAMILY_T_DEFINED
#define _SA_FAMILY_T_DEFINED
typedef unsigned short sa_family_t;
#endif

/**
 * @brief Signal set type.
 * MSVC defines this as unsigned long.
 */
#ifndef _SIGSET_T_DEFINED
#define _SIGSET_T_DEFINED
typedef unsigned long sigset_t;
#endif

/**
 * @brief Identifier type.
 * MSVC defines this as int.
 */
#if !defined(_ID_T_DEFINED) && !defined(__WATCOMC__)
#define _ID_T_DEFINED
typedef int id_t;
#endif

/**
 * @brief Inter-process communication key type.
 * MSVC defines this as int.
 */
#if !defined(_KEY_T_DEFINED) && !defined(__WATCOMC__)
#define _KEY_T_DEFINED
typedef int key_t;
#endif

/**
 * @brief Clock ticks type.
 * MSVC defines this as long.
 */
#if !defined(_CLOCK_T_DEFINED) && !defined(__WATCOMC__)
#define _CLOCK_T_DEFINED
typedef long clock_t;
#endif

/**
 * @brief Time in seconds type.
 * MSVC defines this as __time64_t.
 */
#if !defined(_TIME_T_DEFINED) && !defined(__WATCOMC__)
#define _TIME_T_DEFINED
#if defined(_MSC_VER)
typedef __time64_t time_t;
#else
typedef long time_t;
#endif
#endif

/**
 * @brief Timer type.
 * MSVC defines this as void*.
 */
#if !defined(_TIMER_T_DEFINED) && !defined(__WATCOMC__)
#define _TIMER_T_DEFINED
typedef void *timer_t;
#endif

/**
 * @brief Clock ID type.
 * MSVC defines this as int.
 */
#if !defined(_CLOCKID_T_DEFINED) && !defined(__WATCOMC__)
#define _CLOCKID_T_DEFINED
typedef int clockid_t;
#endif

/**
 * @brief File system block count type.
 * MSVC defines this as unsigned long long.
 */
#ifndef _FSBLKCNT_T_DEFINED
#define _FSBLKCNT_T_DEFINED
#if defined(_MSC_VER) || defined(__WATCOMC__)
typedef unsigned __int64 fsblkcnt_t;
#else
POSIX_TYPES_EXTENSION typedef unsigned long long fsblkcnt_t;
#endif
#endif

/**
 * @brief File system file count type.
 * MSVC defines this as unsigned long long.
 */
#ifndef _FSFILCNT_T_DEFINED
#define _FSFILCNT_T_DEFINED
#if defined(_MSC_VER) || defined(__WATCOMC__)
typedef unsigned __int64 fsfilcnt_t;
#else
POSIX_TYPES_EXTENSION typedef unsigned long long fsfilcnt_t;
#endif
#endif

/**
 * @brief Block size type.
 * MSVC defines this as long.
 */
#ifndef _BLKSIZE_T_DEFINED
#define _BLKSIZE_T_DEFINED
typedef long blksize_t;
#endif

/**
 * @brief Block count type.
 * MSVC defines this as long.
 */
#ifndef _BLKCNT_T_DEFINED
#define _BLKCNT_T_DEFINED
typedef long blkcnt_t;
#endif

#else /* !_WIN32 */

#if !defined(__useconds_t_defined) && !defined(_USECONDS_T_DEFINED) &&         \
    !defined(_USECONDS_T) && !defined(_USECONDS_T_DECLARED) &&                 \
    !defined(_DEFINED_useconds_t) && !defined(__linux__) &&                    \
    !defined(__CYGWIN__)
#define _USECONDS_T_DEFINED
#define _USECONDS_T
#define __useconds_t_defined
#define _DEFINED_useconds_t
typedef unsigned int useconds_t;
#endif

#if !defined(__suseconds_t_defined) && !defined(_SUSECONDS_T_DEFINED) &&       \
    !defined(_SUSECONDS_T) && !defined(_SUSECONDS_T_DECLARED) &&               \
    !defined(_DEFINED_suseconds_t) && !defined(__linux__) &&                   \
    !defined(__CYGWIN__)
#define _SUSECONDS_T_DEFINED
#define _SUSECONDS_T
#define __suseconds_t_defined
#define _DEFINED_suseconds_t
typedef long suseconds_t;
#endif

#if !defined(__blksize_t_defined) && !defined(_BLKSIZE_T_DEFINED) &&           \
    !defined(_BLKSIZE_T) && !defined(_BLKSIZE_T_DECLARED) &&                   \
    !defined(_DEFINED_blksize_t) && !defined(__linux__) &&                     \
    !defined(__CYGWIN__)
#define _BLKSIZE_T_DEFINED
#define _BLKSIZE_T
#define __blksize_t_defined
#define _DEFINED_blksize_t
typedef long blksize_t;
#endif

#ifndef _MQD_T_DEFINED
#define _MQD_T_DEFINED
typedef int mqd_t;
#endif
#endif /* defined(_WIN32) && !defined(__CYGWIN__) */

#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#else
#ifndef _A_W_M_STDINT_TYPES_DEFINED
#define _A_W_M_STDINT_TYPES_DEFINED
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#ifdef _WIN64
typedef int64_t intptr_t;
typedef uint64_t uintptr_t;
#else
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;
#endif
#endif
#endif

#if (defined(_WIN32) && !defined(__CYGWIN__)) || defined(__MSDOS__) ||         \
    defined(__DOS__) || defined(__WATCOMC__) ||                                \
    (defined(__APPLE__) && defined(_POSIX_C_SOURCE) &&                         \
     !defined(_DARWIN_C_SOURCE)) ||                                            \
    (defined(__linux__) && !defined(__USE_MISC)) ||                            \
    (defined(__CYGWIN__) && !__BSD_VISIBLE)

#if !defined(_BSD_SOURCE_TYPES_DEFINED) && !defined(_U_CHAR_DEFINED) &&        \
    !defined(__u_char_defined)
#define _BSD_SOURCE_TYPES_DEFINED
#define _U_CHAR_DEFINED
#define __u_char_defined
#if !defined(_WINSOCKAPI_) && !defined(_WINSOCK2API_)
#ifndef _U_CHAR
#define _U_CHAR
typedef unsigned char u_char;
#endif
#ifndef _U_SHORT
#define _U_SHORT
typedef unsigned short u_short;
#endif
#ifndef _U_INT
#define _U_INT
typedef unsigned int u_int;
#endif
#ifndef _U_LONG
#define _U_LONG
typedef unsigned long u_long;
#endif
#endif
#endif

#ifndef _UNCHAR_DEFINED
#define _UNCHAR_DEFINED
typedef unsigned char unchar;
#endif
#ifndef _USHORT_DEFINED
#define _USHORT_DEFINED
typedef unsigned short ushort;
#endif
#if !defined(_UINT_DEFINED) && !defined(__DARWIN_UINT)
#define _UINT_DEFINED
#define __DARWIN_UINT
typedef unsigned int uint;
#endif
#ifndef _ULONG_DEFINED
#define _ULONG_DEFINED
typedef unsigned long ulong;
#endif

#endif /* BSD u_char/u_short/u_int/u_long shorthand types */

#if (defined(_WIN32) && !defined(__CYGWIN__)) || defined(__MSDOS__) ||         \
    defined(__DOS__) || defined(__WATCOMC__) ||                                \
    (defined(__linux__) && !defined(__USE_MISC)) ||                            \
    (defined(__CYGWIN__) && !__BSD_VISIBLE)

#if !defined(_U_INT8_T_DEFINED) && !defined(__BIT_TYPES_DEFINED__)
#define _U_INT8_T_DEFINED
typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;
#endif

#if !defined(_CADDR_T_DEFINED) && !defined(_CADDR_T) &&                        \
    !defined(__daddr_t_defined)
#define _CADDR_T_DEFINED
#define _CADDR_T
typedef char *caddr_t;
#endif

#if !defined(_DADDR_T_DEFINED) && !defined(_DADDR_T) &&                        \
    !defined(__daddr_t_defined)
#define _DADDR_T_DEFINED
#define _DADDR_T
#define __daddr_t_defined
typedef int64_t daddr_t;
#endif

#if !defined(_REGISTER_T_DEFINED) && !defined(_REGISTER_T) &&                  \
    !defined(__BIT_TYPES_DEFINED__)
#define _REGISTER_T_DEFINED
#define _REGISTER_T
typedef intptr_t register_t;
#endif

#endif /* _WIN32 || DOS || WATCOMC || strict Linux/Cygwin */

#if (defined(_WIN32) && !defined(__CYGWIN__)) || defined(__MSDOS__) ||         \
    defined(__DOS__) || defined(__WATCOMC__)

#if !defined(_IDTYPE_T_DEFINED) && !defined(_IDTYPE_T_DECLARED)
#define _IDTYPE_T_DEFINED
#define _IDTYPE_T_DECLARED
typedef enum { P_ALL, P_PID, P_PGID } idtype_t;
#define P_ALL P_ALL
#define P_PID P_PID
#define P_PGID P_PGID
#endif

#ifndef _FD_MASK_DEFINED
#define _FD_MASK_DEFINED
typedef unsigned long fd_mask;
#endif
#ifndef NFDBITS
#define NFDBITS (sizeof(fd_mask) * 8)
#endif
#ifndef howmany
#define howmany(x, y) (((x) + ((y) - 1)) / (y))
#endif

#endif /* _WIN32 || DOS || WATCOMC */

#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__)

#if !defined(__APPLE__) && !(defined(__linux__) && defined(__USE_MISC))
#ifndef _QUAD_T_DEFINED
#define _QUAD_T_DEFINED
typedef int64_t quad_t;
typedef uint64_t u_quad_t;
#endif
#endif

#if !defined(__APPLE__)
#ifndef _QADDR_T_DEFINED
#define _QADDR_T_DEFINED
typedef quad_t *qaddr_t;
#endif
#endif

#ifndef _C_CADDR_T_DEFINED
#define _C_CADDR_T_DEFINED
typedef const char *c_caddr_t;
#endif

#ifndef _U_DADDR_T_DEFINED
#define _U_DADDR_T_DEFINED
typedef uint64_t u_daddr_t;
#endif

#if !defined(__APPLE__)
#ifndef _FIXPT_T_DEFINED
#define _FIXPT_T_DEFINED
typedef uint32_t fixpt_t;
#endif
#ifndef _SEGSZ_T_DEFINED
#define _SEGSZ_T_DEFINED
typedef int64_t segsz_t;
typedef int64_t swblk_t;
#endif
#ifndef _VM_TYPES_DEFINED
#define _VM_TYPES_DEFINED
typedef uintptr_t vm_offset_t;
typedef int64_t vm_ooffset_t;
typedef uintptr_t vm_paddr_t;
typedef uint64_t vm_pindex_t;
typedef uintptr_t vm_size_t;
#endif
#ifndef _FFLAGS_T_DEFINED
#define _FFLAGS_T_DEFINED
typedef uint32_t fflags_t;
#endif
#else
typedef int64_t vm_ooffset_t;
typedef uint64_t vm_pindex_t;
#endif /* !__APPLE__ */

#ifndef _LWPID_T_DEFINED
#define _LWPID_T_DEFINED
typedef int32_t lwpid_t;
#endif

#ifndef _SBINTIME_T_DEFINED
#define _SBINTIME_T_DEFINED
typedef int64_t sbintime_t;
#endif

#ifndef _KVADDR_T_DEFINED
#define _KVADDR_T_DEFINED
typedef uint64_t kvaddr_t;
typedef uint64_t ksize_t;
#endif

#ifndef _U_REGISTER_T_DEFINED
#define _U_REGISTER_T_DEFINED
typedef uintptr_t u_register_t;
typedef intptr_t syscallarg_t;
#endif

#ifndef _ACCMODE_T_DEFINED
#define _ACCMODE_T_DEFINED
typedef int accmode_t;
#endif

#if !defined(_CPULEVEL_T_DEFINED) && !defined(BSD_SYS_CPUSET_H)
#define _CPULEVEL_T_DEFINED
typedef int cpulevel_t;
typedef int cpuwhich_t;
#endif

#ifndef _CAP_RIGHTS_T_DEFINED
#define _CAP_RIGHTS_T_DEFINED
struct cap_rights {
  uint64_t cr_rights[2];
};
typedef struct cap_rights cap_rights_t;
#endif

#endif /* !BSD */

/**
 * @brief Retrieves information on posix-types availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_types_get_info(int *out_available);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_TYPES_H */
