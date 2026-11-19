#ifndef BSD_SYS_MAC_H
#define BSD_SYS_MAC_H

/**
 * @file mac.h
 * @brief FreeBSD Mandatory Access Control (<sys/mac.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mac *mac_t;

int mac_prepare_process_label(mac_t *label);
int mac_get_proc(mac_t label);
int mac_get_pid(pid_t pid, mac_t label);
int mac_to_text(mac_t label, char **text);
int mac_free(mac_t label);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_SYS_MAC_H */
