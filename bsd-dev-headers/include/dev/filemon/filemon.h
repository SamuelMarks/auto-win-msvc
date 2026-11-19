#ifndef DEV_FILEMON_FILEMON_H
#define DEV_FILEMON_FILEMON_H

/**
 * @file filemon.h
 * @brief FreeBSD filemon (<dev/filemon/filemon.h>) ioctl definitions.
 */

/* clang-format off */
#include <sys/ioccom.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _PATH_FILEMON
#define _PATH_FILEMON "/dev/filemon"
#endif

#define FILEMON_SET_FD _IOWR('S', 1, int)
#define FILEMON_SET_PID _IOWR('S', 2, pid_t)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DEV_FILEMON_FILEMON_H */
