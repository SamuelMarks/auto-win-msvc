#ifndef POSIX_CORE_SYSEXITS_H
#define POSIX_CORE_SYSEXITS_H

#if defined(__GNUC__)
#pragma GCC system_header
#endif

/* clang-format off */
#if !defined(_WIN32) && !defined(_MSC_VER)
#if defined(__GNUC__) || defined(__clang__)
#include_next <sysexits.h>
#else
#include <sysexits.h>
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sysexits.h
 * @brief Standard BSD exit codes.
 */

#if defined(_WIN32) || defined(_MSC_VER)

/** \brief Successful termination. */
#ifndef EX_OK
#define EX_OK 0
#endif

/** \brief Base value for error exit codes. */
#ifndef EX__BASE
#define EX__BASE 64
#endif

/** \brief The command was used incorrectly. */
#ifndef EX_USAGE
#define EX_USAGE 64
#endif

/** \brief The input data was incorrect in some way. */
#ifndef EX_DATAERR
#define EX_DATAERR 65
#endif

/** \brief An input file did not exist or was not readable. */
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

/** \brief Some system file does not exist, cannot be opened, or has error. */
#ifndef EX_OSFILE
#define EX_OSFILE 72
#endif

/** \brief An output file cannot be created. */
#ifndef EX_CANTCREAT
#define EX_CANTCREAT 73
#endif

/** \brief An error occurred while doing I/O on some file. */
#ifndef EX_IOERR
#define EX_IOERR 74
#endif

/** \brief Temporary failure, indicating something not really an error. */
#ifndef EX_TEMPFAIL
#define EX_TEMPFAIL 75
#endif

/** \brief Remote system returned something not conceivable. */
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

#endif /* defined(_WIN32) || defined(_MSC_VER) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_CORE_SYSEXITS_H */
