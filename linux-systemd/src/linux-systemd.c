/* linux-systemd.c - Strict C89 Implementation */

/* clang-format off */
#include "systemd/sd-daemon.h"
/* clang-format on */

/** \brief Polyfill for sd_notify
 * \return 0 or appropriate default
 * value

 */
int sd_notify(int unset_environment, const char *state) {
  (void)unset_environment;
  (void)state;
  return 0;
}

/** \brief Polyfill for sd_event_default
 * \return 0 or appropriate
 * default value
 */
int sd_event_default(sd_event **e) {
  (void)e;
  return 0;
}

/** \brief Polyfill for sd_event_add_io
 * \return 0 or appropriate
 * default value
 */
int sd_event_add_io(sd_event *e, sd_event_source **s, int fd, uint32_t events,
                    sd_event_io_handler_t callback, void *userdata) {
  (void)e;
  (void)s;
  (void)fd;
  (void)events;
  (void)callback;
  (void)userdata;
  return 0;
}

/** \brief Polyfill for sd_event_loop
 * \return 0 or appropriate default
 * value
 */
int sd_event_loop(sd_event *e) {
  (void)e;
  return 0;
}

/** \brief Polyfill for sd_event_unref
 * \return 0 or appropriate
 * default value
 */
sd_event *sd_event_unref(sd_event *e) {
  (void)e;
  return 0;
}
