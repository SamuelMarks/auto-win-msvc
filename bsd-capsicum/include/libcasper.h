#ifndef LIBCASPER_H
#define LIBCASPER_H

/**
 * @file libcasper.h
 * @brief FreeBSD Casper capability daemon client shim (passthrough mode).
 */

/* clang-format off */
#include <sys/types.h>
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Casper support indicator (0 = passthrough mode). */
#define CASPER_SUPPORT 0

#ifndef _NVLIST_T_DECLARED
#define _NVLIST_T_DECLARED
/** @brief Opaque name-value list structure. */
struct nvlist;
typedef struct nvlist nvlist_t;
#endif

#ifndef _CAP_CHANNEL_T_DECLARED
#define _CAP_CHANNEL_T_DECLARED
/** @brief Opaque Casper communication channel. */
struct cap_channel {
  int fd;
};
typedef struct cap_channel cap_channel_t;
#endif

/**
 * @brief Initialize Casper channel.
 * @return Allocated dummy channel pointer.
 */
cap_channel_t *cap_init(void);

/**
 * @brief Wrap an existing socket descriptor into a Casper channel.
 * @param[in] sock Socket descriptor.
 * @param[in] flags Wrapping flags.
 * @return Allocated channel pointer.
 */
cap_channel_t *cap_wrap(int sock, int flags);

/**
 * @brief Unwrap a Casper channel and return its underlying socket.
 * @param[in] chan Casper channel.
 * @param[out] flags Optional pointer receiving flags.
 * @return Underlying socket descriptor.
 */
int cap_unwrap(cap_channel_t *chan, int *flags);

/**
 * @brief Retrieve underlying socket descriptor of a Casper channel.
 * @param[in] chan Casper channel.
 * @return Socket descriptor.
 */
int cap_sock(const cap_channel_t *chan);

/**
 * @brief Clone a Casper channel.
 * @param[in] chan Source channel.
 * @return Cloned channel pointer.
 */
cap_channel_t *cap_clone(const cap_channel_t *chan);

/**
 * @brief Close and free a Casper channel.
 * @param[in] chan Channel to close.
 */
void cap_close(cap_channel_t *chan);

/**
 * @brief Open a named Casper service.
 * @param[in] chan Parent Casper channel.
 * @param[in] name Service name (e.g. "system.dns").
 * @return Service channel pointer.
 */
cap_channel_t *cap_service_open(const cap_channel_t *chan, const char *name);

/**
 * @brief Set service limits via nvlist.
 * @param[in] chan Service channel.
 * @param[in] limits Limits nvlist.
 * @return Always 0.
 */
int cap_limit_set(const cap_channel_t *chan, nvlist_t *limits);

/**
 * @brief Get service limits via nvlist.
 * @param[in] chan Service channel.
 * @param[out] limitsp Pointer receiving limits nvlist.
 * @return Always 0.
 */
int cap_limit_get(const cap_channel_t *chan, nvlist_t **limitsp);

/**
 * @brief Send nvlist over Casper channel.
 * @param[in] chan Service channel.
 * @param[in] nvl nvlist to send.
 * @return Always 0.
 */
int cap_send_nvlist(const cap_channel_t *chan, const nvlist_t *nvl);

/**
 * @brief Receive nvlist from Casper channel.
 * @param[in] chan Service channel.
 * @return NULL.
 */
nvlist_t *cap_recv_nvlist(const cap_channel_t *chan);

/**
 * @brief Exchange nvlist over Casper channel.
 * @param[in] chan Service channel.
 * @param[in] nvl nvlist to send.
 * @return NULL.
 */
nvlist_t *cap_xfer_nvlist(const cap_channel_t *chan, nvlist_t *nvl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCASPER_H */
