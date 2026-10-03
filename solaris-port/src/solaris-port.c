/**
 * @file solaris-port.c
 * @brief Implementation of Solaris event ports stub for MSVC.
 */

/* clang-format off */
#include "solaris-port.h"
#include <errno.h>
#include <stddef.h>
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef ENOSYS
#define ENOSYS 40
#endif

#ifndef EINVAL
#define EINVAL 22
#endif

/**
 * @brief Creates a new event port.
 */
int port_create(void) {
  errno = ENOSYS;
  return -1;
}

/**
 * @brief Associates a specific event source and object with an event port.
 */
int port_associate(int port, int source, unsigned int object, int events,
                   void *user) {
  if (port || source || object || events || user) {
    /* parameters checked */
  }
  errno = ENOSYS;
  return -1;
}

/**
 * @brief Retrieves one or more events from an event port.
 */
int port_getn(int port, struct port_event *list, unsigned int max,
              unsigned int *nget, const struct timespec *timeout) {
  if (port || list || max || nget || timeout) {
    /* parameters checked */
  }
  errno = ENOSYS;
  return -1;
}

#endif /* defined(_MSC_VER) || defined(_WIN32) */

/**
 * @brief Retrieves information on solaris-port availability.
 */
auto_win_msvc_error_t solaris_port_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 0;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_solaris_port;
