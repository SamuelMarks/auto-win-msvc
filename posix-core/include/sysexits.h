#if defined(__GNUC__)
#pragma GCC system_header
#endif
#if !defined(_WIN32) && !defined(_MSC_VER)
/* clang-format off */
#if defined(__GNUC__) || defined(__clang__)
#include_next <sysexits.h>
#else
#include <sysexits.h>
#endif
/* clang-format on */
#else

#ifndef POSIX_SYSEXITS_H
#define POSIX_SYSEXITS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sysexits.h
 * @brief Standard BSD exit codes.
 */

/** \brief Successful termination. */
#ifndef EX_OK
#define EX_OK 0
#endif

/** \brief Base value for error exit codes. */
#ifndef EX__BASE
#define EX__BASE 64
#endif

/** \brief The command was used incorrectly, e.g. with the wrong number of
 * arguments, a bad flag, bad syntax in a parameter, or whatever. */
#ifndef EX_USAGE
#define EX_USAGE 64
#endif

/** \brief The input data was incorrect in some way. */
#ifndef EX_DATAERR
#define EX_DATAERR 65
#endif

/** \brief An input file (not a system file) did not exist or was not readable.
 */
#ifndef EX_NOINPUT
#define EX_NOINPUT 66
#endif

/** \brief The user specified did not exist. */
#ifndef EX_NOUSER
#define EX_NOUSER 67
#endif

/** \brief The host specified did not exist. */
#ifndef EX_NOHOST
#define EX_NOHOST 68
#endif

/** \brief A service is unavailable. */
#ifndef EX_UNAVAILABLE
#define EX_UNAVAILABLE 69
#endif

/** \brief An internal software error has been detected. */
#ifndef EX_SOFTWARE
#define EX_SOFTWARE 70
#endif

/** \brief An operating system error has been detected. */
#ifndef EX_OSERR
#define EX_OSERR 71
#endif

/** \brief Some system file (e.g., /etc/passwd, /var/run/utmp, etc.) does not
 * exist, cannot be opened, or has some sort of error. */
#ifndef EX_OSFILE
#define EX_OSFILE 72
#endif

/** \brief A (user specified) output file cannot be created. */
#ifndef EX_CANTCREAT
#define EX_CANTCREAT 73
#endif

/** \brief An error occurred while doing I/O on some file. */
#ifndef EX_IOERR
#define EX_IOERR 74
#endif

/** \brief Temporary failure, indicating something that is not really an error.
 */
#ifndef EX_TEMPFAIL
#define EX_TEMPFAIL 75
#endif

/** \brief The remote system returned something that was "not conceivable"
 * during a protocol exchange. */
#ifndef EX_PROTOCOL
#define EX_PROTOCOL 76
#endif

/** \brief You did not have sufficient permission to perform the operation. */
#ifndef EX_NOPERM
#define EX_NOPERM 77
#endif

/** \brief Something was found in an unconfigured or misconfigured state. */
#ifndef EX_CONFIG
#define EX_CONFIG 78
#endif

/** \brief Maximum exit code value. */
#ifndef EX__MAX
#define EX__MAX 78
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYSEXITS_H */

#endif /* !defined(_WIN32) && !defined(_MSC_VER) */
