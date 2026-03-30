#include "solaris-port.h"
#include <errno.h>
#if defined(_MSC_VER) && !defined(__clang__)
/** \brief port_create function. */
int port_create(void) {
  errno = ENOSYS;
  return -1;
}
/** \brief port_associate function. */
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
/** \brief port_getn function. */
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
#endif

/* Dummy function to prevent empty translation unit */
int dummy_solaris_port(void) { return 0; }

typedef int make_iso_compilers_happy_tu_solaris_port;
