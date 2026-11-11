#ifndef SYS_CONSIO_H
#define SYS_CONSIO_H

/**
 * @file consio.h
 * @brief FreeBSD console ioctl commands (<sys/consio.h>).
 */

/* clang-format off */
#include <sys/ioccom.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define VT_OPENQRY _IOR('v', 1, int)
#define VT_SETMODE _IOW('v', 2, struct vt_mode)
#define VT_GETMODE _IOR('v', 3, struct vt_mode)
#define VT_RELDISP _IOW('v', 4, int)
#define VT_ACTIVATE _IOW('v', 5, int)
#define VT_WAITACTIVE _IOW('v', 6, int)
#define VT_GETACTIVE _IOR('v', 7, int)
#define VT_LOCKSWITCH _IOW('v', 8, int)

struct vt_mode {
  char mode;
  char waitv;
  short relsig;
  short acqsig;
  short frsig;
};

#define VT_AUTO 0
#define VT_PROCESS 1
#define VT_ACKACQ 2

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_CONSIO_H */
