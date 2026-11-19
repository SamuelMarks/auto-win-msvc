#ifndef NETIPSEC_IPSEC_H
#define NETIPSEC_IPSEC_H

/**
 * @file ipsec.h
 * @brief FreeBSD IPsec policy management (<netipsec/ipsec.h>) compatibility
 * header.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IP_IPSEC_POLICY
#define IP_IPSEC_POLICY 21
#endif
#ifndef IPV6_IPSEC_POLICY
#define IPV6_IPSEC_POLICY 28
#endif
#ifndef IPSEC_POLICY_IPSEC
#define IPSEC_POLICY_IPSEC 2
#endif

/** @brief Parse textual IPsec policy specification into binary policy buffer.
 */
char *ipsec_set_policy(const char *policy, int policylen);
/** @brief Return byte length of compiled IPsec policy buffer. */
int ipsec_get_policylen(const char *buf);
/** @brief Return human-readable error message for last libipsec failure. */
const char *ipsec_strerror(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NETIPSEC_IPSEC_H */
