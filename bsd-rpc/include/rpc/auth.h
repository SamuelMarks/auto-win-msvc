#ifndef RPC_AUTH_H
#define RPC_AUTH_H

/**
 * @file auth.h
 * @brief SunRPC authentication (<rpc/auth.h>) polyfill.
 */

/* clang-format off */
#include <rpc/xdr.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

struct opaque_auth {
  enum_t oa_flavor;
  caddr_t oa_base;
  u_int oa_length;
};

typedef struct __rpc_auth {
  struct opaque_auth ah_cred;
  struct opaque_auth ah_verf;
  void *ah_private;
} AUTH;

AUTH *authunix_create_default(void);
AUTH *authnone_create(void);
void auth_destroy(AUTH *auth);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPC_AUTH_H */
