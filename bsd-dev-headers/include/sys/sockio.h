#ifndef BSD_SYS_SOCKIO_H
#define BSD_SYS_SOCKIO_H

/**
 * @file sockio.h
 * @brief BSD socket I/O control constants (<sys/sockio.h>).
 */

/* clang-format off */
#include <sys/ioccom.h>
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) && \
    (defined(__GNUC__) || defined(__clang__))
#if defined(__has_include_next)
#if __has_include_next(<sys/sockio.h>)
#pragma GCC system_header
#include_next <sys/sockio.h>
#endif
#endif
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SIOCSHIWAT
#define SIOCSHIWAT _IOW('s', 0, int)
#endif
#ifndef SIOCGHIWAT
#define SIOCGHIWAT _IOR('s', 1, int)
#endif
#ifndef SIOCSLOWAT
#define SIOCSLOWAT _IOW('s', 2, int)
#endif
#ifndef SIOCGLOWAT
#define SIOCGLOWAT _IOR('s', 3, int)
#endif
#ifndef SIOCATMARK
#define SIOCATMARK _IOR('s', 7, int)
#endif
#ifndef SIOCSPGRP
#define SIOCSPGRP _IOW('s', 8, int)
#endif
#ifndef SIOCGPGRP
#define SIOCGPGRP _IOR('s', 9, int)
#endif
#ifndef SIOCSIFFLAGS
#define SIOCSIFFLAGS _IOW('i', 16, int)
#endif
#ifndef SIOCGIFFLAGS
#define SIOCGIFFLAGS _IOWR('i', 17, int)
#endif
#ifndef SIOCGIFADDR
#define SIOCGIFADDR _IOWR('i', 33, int)
#endif
#ifndef SIOCGIFDSTADDR
#define SIOCGIFDSTADDR _IOWR('i', 34, int)
#endif
#ifndef SIOCGIFBRDADDR
#define SIOCGIFBRDADDR _IOWR('i', 35, int)
#endif
#ifndef SIOCGIFCONF
#define SIOCGIFCONF _IOWR('i', 36, int)
#endif
#ifndef SIOCGIFNETMASK
#define SIOCGIFNETMASK _IOWR('i', 37, int)
#endif
#ifndef SIOCGIFMETRIC
#define SIOCGIFMETRIC _IOWR('i', 23, int)
#endif
#ifndef SIOCGIFMTU
#define SIOCGIFMTU _IOWR('i', 51, int)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_SOCKIO_H */
