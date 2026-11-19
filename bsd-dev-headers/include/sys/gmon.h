#ifndef SYS_GMON_H
#define SYS_GMON_H

/**
 * @file gmon.h
 * @brief FreeBSD execution profiling header (<sys/gmon.h>).
 */

/* clang-format off */
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
#include <sys/types.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

struct gmonhdr {
  unsigned long lpc;
  unsigned long hpc;
  int ncnt;
  int version;
  int profrate;
  int histcounter_type;
  int spare[2];
};

#define GMONVERSION 0x00051879
#define HISTCOUNTER unsigned short
#define FUNCTION_ALIGNMENT 4
#define HISTFRACTION 2
#define HASHFRACTION 2
#define ARCDENSITY 2
#define MINARCS 50
#define MAXARCS ((1UL << 16) - 2)

struct tostruct {
  unsigned long selfpc;
  long count;
  unsigned short link;
  unsigned short pad;
};

struct rawarc {
  unsigned long raw_frompc;
  unsigned long raw_selfpc;
  long raw_count;
};

#define GMON_PROF_ON 0
#define GMON_PROF_BUSY 1
#define GMON_PROF_ERROR 2
#define GMON_PROF_OFF 3

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_GMON_H */
