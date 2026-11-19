#ifndef BSD_CAM_CAM_H
#define BSD_CAM_CAM_H

/**
 * @file cam.h
 * @brief FreeBSD Common Access Method (CAM) SCSI/ATA subsystem (<cam/cam.h>)
 * polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#include <stddef.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define CAM_REQ_INPROG 0x00
#define CAM_REQ_CMP 0x01
#define CAM_REQ_ABORTED 0x02
#define CAM_REQ_CMP_ERR 0x04
#define CAM_STATUS_MASK 0x3f
#define CAM_DEV_POS_NONE 0x000
#define CAM_DEV_POS_BUS 0x001
#define CAM_DEV_POS_TARGET 0x002
#define CAM_DEV_POS_LUN 0x004
#define CAM_ERRBUF_SIZE 2048

typedef unsigned int path_id_t;
typedef unsigned int target_id_t;
typedef uint64_t lun_id_t;
struct ccb_hdr {
  uint32_t status;
  path_id_t path_id;
  target_id_t target_id;
  lun_id_t target_lun;
};

/** @brief Opaque CAM device handle structure. */
struct cam_device {
  char device_path[256];
  char given_dev_name[64];
  uint32_t given_unit_number;
  char device_name[64];
  uint32_t dev_unit_num;
  path_id_t path_id;
  target_id_t target_id;
  lun_id_t target_lun;
  int fd;
};

struct cam_device *cam_open_device(const char *path, int flags);
struct cam_device *cam_open_spec_device(const char *dev_name, int unit,
                                        int flags, struct cam_device *device);
void cam_close_device(struct cam_device *dev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_CAM_CAM_H */
