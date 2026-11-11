#ifndef SYS_JAIL_H
#define SYS_JAIL_H

/**
 * @file jail.h
 * @brief FreeBSD <sys/jail.h> definitions.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define JAIL_API_VERSION 2

#define JAIL_CREATE 0x01
#define JAIL_UPDATE 0x02
#define JAIL_ATTACH 0x04
#define JAIL_DYING 0x08

struct iovec;

int jail_get(struct iovec *iov, unsigned int niov, int flags);
int jail_set(struct iovec *iov, unsigned int niov, int flags);
int jail_attach(int jid);
int jail_remove(int jid);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_JAIL_H */
