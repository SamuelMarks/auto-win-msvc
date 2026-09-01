/* systemd/sd-daemon.h - Strict C89 Header */
#ifndef SYSTEMD_SD_DAEMON_H
#define SYSTEMD_SD_DAEMON_H

/* clang-format off */
#include <posix-stdint.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EPOLLIN
#define EPOLLIN 0x001
#define EPOLLPRI 0x002
#define EPOLLOUT 0x004
#define EPOLLERR 0x008
#define EPOLLHUP 0x010
#endif

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
typedef int error_type_t;
#define ERR_NONE 0
#endif

typedef struct sd_event sd_event;
typedef struct sd_event_source sd_event_source;
typedef int (*sd_event_io_handler_t)(sd_event_source *s, int fd,
                                     uint32_t revents, void *userdata);

/** \brief Notify service manager about state changes. */
error_type_t sd_notify(int unset_environment, const char *state,
                       int *out_result);

/** \brief Acquire the default event loop object. */
error_type_t sd_event_default(sd_event **e);

/** \brief Add an I/O event source to an event loop. */
error_type_t sd_event_add_io(sd_event *e, sd_event_source **s, int fd,
                             uint32_t events, sd_event_io_handler_t callback,
                             void *userdata);

/** \brief Run the event loop. */
error_type_t sd_event_loop(sd_event *e);

/** \brief Decrease the reference count of an event loop object. */
sd_event *sd_event_unref(sd_event *e);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYSTEMD_SD_DAEMON_H */
