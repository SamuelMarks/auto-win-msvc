#ifndef UFS_UFS_QUOTA_H
#define UFS_UFS_QUOTA_H

/**
 * @file quota.h
 * @brief FreeBSD UFS disk quota (<ufs/ufs/quota.h>) definitions.
 */

/* clang-format off */
#include <sys/types.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define MAXQUOTAS 2
#define USRQUOTA 0
#define GRPQUOTA 1

#define Q_QUOTAON 0x0100
#define Q_QUOTAOFF 0x0200
#define Q_GETQUOTA 0x0700
#define Q_SETQUOTA 0x0800
#define Q_SYNC 0x0600

#define QCMD(cmd, type) (((cmd) << 8) | ((type) & 0x00ff))

/** @brief Disk quota usage and limits block. */
struct dqblk {
  uint64_t dqb_bhardlimit;
  uint64_t dqb_bsoftlimit;
  uint64_t dqb_curblocks;
  uint64_t dqb_ihardlimit;
  uint64_t dqb_isoftlimit;
  uint64_t dqb_curinodes;
  int64_t dqb_btime;
  int64_t dqb_itime;
};

int quotactl(const char *path, int cmd, int id, void *addr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UFS_UFS_QUOTA_H */
