#ifndef SOLARIS_PORT_H
#define SOLARIS_PORT_H
#if defined(_MSC_VER) && !defined(__clang__)
struct port_event { int portev_events; unsigned short portev_source; unsigned short portev_pad; unsigned int portev_object; void *portev_user; };
int port_create(void);
int port_associate(int port, int source, unsigned int object, int events, void *user);
struct timespec;
int port_getn(int port, struct port_event *list, unsigned int max, unsigned int *nget, const struct timespec *timeout);
#endif
#endif
