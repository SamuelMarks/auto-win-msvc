#ifndef SYS_EVENT_H
#define SYS_EVENT_H

/**
 * @file event.h
 * @brief Polyfill forwarding header for BSD <sys/event.h>.
 */

/* clang-format off */
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || \
    defined(__NetBSD__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#include_next <sys/event.h>
#endif
#endif
#include "../bsd-sys-event.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_EVENT_H */
