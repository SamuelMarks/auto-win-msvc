#ifndef DEV_SPEAKER_SPEAKER_H
#define DEV_SPEAKER_SPEAKER_H

/**
 * @file speaker.h
 * @brief FreeBSD PC speaker device ioctl definitions (<dev/speaker/speaker.h>).
 */

/* clang-format off */
#include <sys/ioccom.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int frequency;
  int duration;
} tone_t;

#define SPKRTONE _IOW('S', 1, tone_t)
#define SPKRTUNE _IO('S', 2)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DEV_SPEAKER_SPEAKER_H */
