/* clang-format off */
#ifndef SYS_SENDFILE_H_
#define SYS_SENDFILE_H_

#if defined(__GNUC__) || defined(__clang__)
#endif

#if defined(_MSC_VER)

#endif
#ifndef DEFINED_MSC_VER_FOR_TEST
#include "posix-types.h"
#endif
#if defined(_WIN32)

#ifndef _OFF_T
typedef long off_t;
#endif
#if !defined(_SSIZE_T) && !defined(_SSIZE_T_DEFINED)
typedef long ssize_t;
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief sendfile() copies data between one file descriptor and another.  Because this copying is done within the kernel, sendfile() is more  efficient than the combination of read(2) and write(2), which  would require transferring data to and from user space.
 */
ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* SYS_SENDFILE_H_ */
/* clang-format on */

/* API Contract symbols: __dependencies__ */
