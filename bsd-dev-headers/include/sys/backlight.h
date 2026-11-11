#ifndef SYS_BACKLIGHT_H
#define SYS_BACKLIGHT_H

/**
 * @file backlight.h
 * @brief FreeBSD display backlight control (<sys/backlight.h>).
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

#define BACKLIGHT_MAX_INFO 128
#define BACKLIGHTMAXLEVELS 100

enum backlight_info_type {
  BACKLIGHT_TYPE_PANEL = 0,
  BACKLIGHT_TYPE_KEYBOARD = 1
};

struct backlight_props {
  uint32_t brightness;
  uint32_t nlevels;
  uint32_t levels[BACKLIGHTMAXLEVELS];
};

struct backlight_info {
  enum backlight_info_type type;
  char name[BACKLIGHT_MAX_INFO];
};

#define BACKLIGHTGETSTATUS _IOWR('B', 0, struct backlight_props)
#define BACKLIGHTUPDATESTATUS _IOWR('B', 1, struct backlight_props)
#define BACKLIGHTGETINFO _IOWR('B', 2, struct backlight_info)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_BACKLIGHT_H */
