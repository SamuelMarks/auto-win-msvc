#ifndef CAM_SCSI_SCSI_SA_H
#define CAM_SCSI_SCSI_SA_H

/**
 * @file scsi_sa.h
 * @brief FreeBSD CAM SCSI sequential access (<cam/scsi/scsi_sa.h>) header stub.
 */

/* clang-format off */
#if defined(_MSC_VER) && _MSC_VER < 1600
#include <posix-stdint.h>
#else
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define SA_CTRL_DP_REED_SOLOMON 1
#define SA_CTRL_DP_METHOD_MAX 0xff
#define SA_CTRL_DP_RS_LENGTH 4
#define SA_CTRL_DP_PI_LENGTH_MASK 0x3f

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CAM_SCSI_SCSI_SA_H */
