#ifndef SYS_TTYCOM_H
#define SYS_TTYCOM_H

/**
 * @file ttycom.h
 * @brief BSD terminal ioctl commands (<sys/ttycom.h>).
 */

/* clang-format off */
#include <sys/cdefs.h>
#if (defined(__GNUC__) || defined(__clang__)) && !defined(_WIN32)
#if defined(__has_include_next)
#if __has_include_next(<sys/ttycom.h>)
#pragma GCC system_header
#include_next <sys/ttycom.h>
#endif
#endif
#endif
#include <sys/ioccom.h>
#include <sys/ioctl.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TIOCGWINSZ
#define TIOCGWINSZ _IOR('t', 104, struct winsize)
#endif
#ifndef TIOCSWINSZ
#define TIOCSWINSZ _IOW('t', 103, struct winsize)
#endif
#ifndef TIOCSCTTY
#define TIOCSCTTY _IO('t', 97)
#endif
#ifndef TIOCNOTTY
#define TIOCNOTTY _IO('t', 113)
#endif
#ifndef TIOCGPGRP
#define TIOCGPGRP _IOR('t', 119, int)
#endif
#ifndef TIOCSPGRP
#define TIOCSPGRP _IOW('t', 118, int)
#endif
#ifndef TIOCOUTQ
#define TIOCOUTQ _IOR('t', 115, int)
#endif
#ifndef TIOCSTI
#define TIOCSTI _IOW('t', 114, char)
#endif
#ifndef TIOCMGET
#define TIOCMGET _IOR('t', 106, int)
#endif
#ifndef TIOCMSET
#define TIOCMSET _IOW('t', 109, int)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_TTYCOM_H */
