#ifndef SYS_TTY_H
#define SYS_TTY_H

/**
 * @file tty.h
 * @brief FreeBSD kernel/user tty structure (<sys/tty.h>).
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/ttycom.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

struct xtty {
  size_t xt_size;
  size_t xt_insize;
  size_t xt_incc;
  size_t xt_inlow;
  size_t xt_outsize;
  size_t xt_outcc;
  size_t xt_outlow;
  unsigned int xt_column;
  pid_t xt_pgid;
  pid_t xt_sid;
  unsigned int xt_flags;
  uint32_t xt_dev;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_TTY_H */
