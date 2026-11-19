#ifndef CAM_CAM_CCB_H
#define CAM_CAM_CCB_H

/**
 * @file cam_ccb.h
 * @brief FreeBSD CAM Common Control Block (<cam/cam_ccb.h>) header stub.
 */

/* clang-format off */
#include <cam/cam.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

union ccb {
  struct ccb_hdr ccb_h;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CAM_CAM_CCB_H */
