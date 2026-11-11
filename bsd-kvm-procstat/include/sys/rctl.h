#ifndef SYS_RCTL_H
#define SYS_RCTL_H

/**
 * @file rctl.h
 * @brief FreeBSD resource limits (<sys/rctl.h>) polyfill.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

int rctl_get_racct(const char *inbufp, size_t inbuflen, char *outbufp,
                   size_t outbuflen);
int rctl_get_rules(const char *inbufp, size_t inbuflen, char *outbufp,
                   size_t outbuflen);
int rctl_get_limits(const char *inbufp, size_t inbuflen, char *outbufp,
                    size_t outbuflen);
int rctl_add_rule(const char *inbufp, size_t inbuflen, char *outbufp,
                  size_t outbuflen);
int rctl_remove_rule(const char *inbufp, size_t inbuflen, char *outbufp,
                     size_t outbuflen);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_RCTL_H */
