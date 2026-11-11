#ifndef SYS_MTIO_H
#define SYS_MTIO_H

/**
 * @file mtio.h
 * @brief BSD magnetic tape ioctl structures and commands (<sys/mtio.h>).
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/ioccom.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

struct mtop {
  short mt_op;
  int32_t mt_count;
};

#define MTWEOF 0
#define MTFSF 1
#define MTBSF 2
#define MTFSR 3
#define MTBSR 4
#define MTREW 5
#define MTOFFL 6
#define MTNOP 7
#define MTCACHE 8
#define MTNOCACHE 9
#define MTSETBSIZ 10
#define MTSETDNSTY 11
#define MTERASE 12
#define MTEOD 13
#define MTCOMP 14
#define MTRETENS 15
#define MTWSM 16
#define MTEWARN 17

struct mtget {
  short mt_type;
  short mt_dsreg;
  short mt_erreg;
  short mt_resid;
  int32_t mt_blksiz;
  int32_t mt_density;
  uint32_t mt_comp;
  int32_t mt_blksiz0;
  int32_t mt_blksiz1;
  int32_t mt_blksiz2;
  int32_t mt_blksiz3;
  int32_t mt_density0;
  int32_t mt_density1;
  int32_t mt_density2;
  int32_t mt_density3;
  uint32_t mt_comp0;
  uint32_t mt_comp1;
  uint32_t mt_comp2;
  uint32_t mt_comp3;
  int32_t mt_fileno;
  int32_t mt_blkno;
};

#define MTIOCTOP _IOW('m', 1, struct mtop)
#define MTIOCGET _IOR('m', 2, struct mtget)
#define MTIOCIEOT _IO('m', 3)
#define MTIOCEEOT _IO('m', 4)
#define MTIOCRDSPOS _IOR('m', 5, uint32_t)
#define MTIOCRDHPOS _IOR('m', 6, uint32_t)
#define MTIOCSLOCATE _IOW('m', 5, uint32_t)
#define MTIOCHLOCATE _IOW('m', 6, uint32_t)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_MTIO_H */
