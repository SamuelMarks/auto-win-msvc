/* systemd/sd-event.h - Strict C89 Header */
#ifndef SYSTEMD_SD_EVENT_H
#define SYSTEMD_SD_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* clang-format off */
#include <posix-stdint.h>
/* clang-format on */

typedef struct sd_event sd_event;
typedef struct sd_event_source sd_event_source;
typedef int (*sd_event_io_handler_t)(sd_event_source *s, int fd,
                                     uint32_t revents, void *userdata);

int sd_notify(int unset_environment, const char *state);
int sd_event_default(sd_event **e);
int sd_event_add_io(sd_event *e, sd_event_source **s, int fd, uint32_t events,
                    sd_event_io_handler_t callback, void *userdata);
int sd_event_loop(sd_event *e);
sd_event *sd_event_unref(sd_event *e);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEMD_SD_EVENT_H */
