#ifndef SYS_DISK_H
#define SYS_DISK_H

/**
 * @file disk.h
 * @brief FreeBSD disk device ioctl commands (<sys/disk.h>).
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/ioccom.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define DIOCGSECTORSIZE _IOR('d', 128, unsigned int)
#define DIOCGMEDIASIZE _IOR('d', 129, off_t)
#define DIOCGFWSECTORS _IOR('d', 130, unsigned int)
#define DIOCGFWHEADS _IOR('d', 131, unsigned int)
#define DIOCGFLUSH _IO('d', 135)
#define DIOCGIDENT _IOR('d', 137, char[256])
#define DIOCGSTRIPESIZE _IOR('d', 139, off_t)
#define DIOCGSTRIPEOFFSET _IOR('d', 140, off_t)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_DISK_H */
