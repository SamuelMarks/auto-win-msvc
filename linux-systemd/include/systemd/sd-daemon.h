#ifndef SYSTEMD_SD_DAEMON_H
#define SYSTEMD_SD_DAEMON_H

/**
 * @file systemd/sd-daemon.h
 * @brief Polyfill for systemd <systemd/sd-daemon.h> daemon notification and
 * event loop.
 */

/* clang-format off */
#include <stddef.h>
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
/** @brief Generic error type code. */
typedef int error_type_t;
/** @brief Success indicator for error_type_t. */
#define ERR_NONE 0
#endif

/**
 * @brief Error codes returned by linux-systemd functions.
 */
enum linux_systemd_error_code {
  /** @brief Successful operation. */
  LINUX_SYSTEMD_SUCCESS = 0,
  /** @brief Null pointer passed as argument. */
  LINUX_SYSTEMD_ERROR_NULL_POINTER = 1,
  /** @brief Invalid argument or descriptor passed to function. */
  LINUX_SYSTEMD_ERROR_INVALID_ARGUMENT = 2,
  /** @brief Operation not supported on this platform. */
  LINUX_SYSTEMD_ERROR_NOT_SUPPORTED = 3
};

/**
 * @brief Initializes and validates the linux-systemd module.
 * @param[out] out_status Pointer to an integer receiving the initialized
 * status.
 * @return LINUX_SYSTEMD_SUCCESS on success, or an error code on failure.
 */
enum linux_systemd_error_code linux_systemd_init(int *out_status);

typedef struct sd_event sd_event;
typedef struct sd_event_source sd_event_source;
typedef int (*sd_event_io_handler_t)(sd_event_source *s, int fd,
                                     uint32_t revents, void *userdata);

/**
 * @brief Notify service manager about state changes.
 * @param unset_environment If true, unsets NOTIFY_SOCKET.
 * @param state State change string.
 * @param[out] out_result Output pointer receiving 1 on notification sent, 0 on
 * skip.
 * @return ERR_NONE on success, or an error code.
 */
error_type_t sd_notify(int unset_environment, const char *state,
                       int *out_result);

/**
 * @brief Acquire the default event loop object.
 * @param[out] e Pointer receiving the allocated event loop.
 * @return ERR_NONE on success, or an error code.
 */
error_type_t sd_event_default(sd_event **e);

/**
 * @brief Add an I/O event source to an event loop.
 * @param e Event loop.
 * @param[out] s Pointer receiving event source.
 * @param fd File descriptor.
 * @param events Bitmask of events.
 * @param callback Callback function.
 * @param userdata User data passed to callback.
 * @return ERR_NONE on success, or an error code.
 */
error_type_t sd_event_add_io(sd_event *e, sd_event_source **s, int fd,
                             uint32_t events, sd_event_io_handler_t callback,
                             void *userdata);

/**
 * @brief Run the event loop.
 * @param e Event loop.
 * @return ERR_NONE on success, or an error code.
 */
error_type_t sd_event_loop(sd_event *e);

/**
 * @brief Decrease the reference count of an event loop object.
 * @param e Event loop.
 * @return NULL.
 */
sd_event *sd_event_unref(sd_event *e);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYSTEMD_SD_DAEMON_H */
