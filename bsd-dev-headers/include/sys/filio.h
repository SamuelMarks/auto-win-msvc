#ifndef SYS_FILIO_H
#define SYS_FILIO_H

/**
 * @file filio.h
 * @brief BSD generic file-descriptor ioctl commands (<sys/filio.h>).
 */

/* clang-format off */
#include <sys/cdefs.h>
#if (defined(__GNUC__) || defined(__clang__)) && !defined(_WIN32)
#if defined(__has_include_next)
#if __has_include_next(<sys/filio.h>)
#pragma GCC system_header
#include_next <sys/filio.h>
#endif
#endif
#endif
#include <sys/ioccom.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FIOCLEX
#define FIOCLEX _IO('f', 1)
#endif
#ifndef FIONCLEX
#define FIONCLEX _IO('f', 2)
#endif
#ifndef FIONREAD
#define FIONREAD _IOR('f', 127, int)
#endif
#ifndef FIONBIO
#define FIONBIO _IOW('f', 126, int)
#endif
#ifndef FIOASYNC
#define FIOASYNC _IOW('f', 125, int)
#endif
#ifndef FIOSETOWN
#define FIOSETOWN _IOW('f', 124, int)
#endif
#ifndef FIOGETOWN
#define FIOGETOWN _IOR('f', 123, int)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_FILIO_H */
