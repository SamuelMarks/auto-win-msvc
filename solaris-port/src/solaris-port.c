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
  (void)port;
  (void)source;
  (void)object;
  (void)events;
  (void)user;
  errno = ENOSYS;
  return -1;
}

/**
 * @brief Retrieves one or more events from an event port.
 */
int port_getn(int port, struct port_event *list, unsigned int max,
              unsigned int *nget, const struct timespec *timeout) {
  (void)port;
  (void)list;
  (void)max;
  (void)nget;
  (void)timeout;
  errno = ENOSYS;
  return -1;
}

#endif /* defined(_MSC_VER) || defined(_WIN32) */

/**
 * @brief Retrieves information on solaris-port availability.
 */
enum solaris_port_error_code solaris_port_get_info(int *out_available) {
  if (out_available == NULL) {
    return SOLARIS_PORT_ERROR_NULL_POINTER;
  }
  *out_available = 0;
  return SOLARIS_PORT_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_solaris_port;
