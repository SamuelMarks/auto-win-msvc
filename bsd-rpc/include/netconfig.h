#ifndef BSD_RPC_NETCONFIG_H
#define BSD_RPC_NETCONFIG_H

/**
 * @file netconfig.h
 * @brief TI-RPC network configuration (<netconfig.h>) polyfill.
 */

/* clang-format off */
#include <rpc/types.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define NC_TPI_CLTS 1
#define NC_TPI_COTS 2
#define NC_TPI_COTS_ORD 3
#define NC_TPI_RAW 4
#define NC_VISIBLE 0x0001
#define NC_BROADCAST 0x0002
#define NC_INET "inet"
#define NC_INET6 "inet6"
#define NC_LOOPBACK "loopback"

/** @brief Initialize iteration over /etc/netconfig entries. */
void *setnetconfig(void);
/** @brief Retrieve next network configuration entry from handlep. */
struct netconfig *getnetconfig(void *handlep);
/** @brief Release iteration handle opened with setnetconfig. */
int endnetconfig(void *handlep);
/** @brief Look up a network configuration entry by network ID. */
struct netconfig *getnetconfigent(const char *netid);
/** @brief Free a netconfig structure returned by getnetconfigent. */
void freenetconfigent(struct netconfig *netconfigp);
/** @brief Print netconfig error message prefixed with s to stderr. */
void nc_perror(const char *s);
/** @brief Return string describing the most recent netconfig error. */
char *nc_sperror(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_RPC_NETCONFIG_H */
