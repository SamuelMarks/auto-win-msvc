#ifndef STROPTS_H
#define STROPTS_H

/**
 * @file stropts.h
 * @brief Polyfill for POSIX stropts.h STREAMS interface.
 */

/* clang-format off */
#include <sys/types.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Basic ioctl commands for STREAMS. */
#define I_PUSH 1
#define I_POP 2
#define I_LOOK 3
#define I_FLUSH 4
#define I_SRDOPT 5
#define I_GRDOPT 6
#define I_STR 7
#define I_SETSIG 8
#define I_GETSIG 9
#define I_FIND 10
#define I_LINK 11
#define I_UNLINK 12

/**
 * @struct strbuf
 * @brief Buffer structure for STREAMS messages.
 */
struct strbuf {
  /** @brief Maximum buffer length. */
  int maxlen;
  /** @brief Length of data. */
  int len;
  /** @brief Pointer to buffer. */
  char *buf;
};

#define isastream posix_isastream

/**
 * @brief Determines if a file descriptor is associated with a STREAMS device.
 * @param fildes File descriptor.
 * @return 1 if STREAMS device, 0 if not, -1 on error.
 */
int posix_isastream(int fildes);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STROPTS_H */
