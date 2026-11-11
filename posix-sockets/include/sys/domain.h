#ifndef SYS_DOMAIN_H
#define SYS_DOMAIN_H

/**
 * @file domain.h
 * @brief BSD communications domain structure (<sys/domain.h>).
 */

#ifdef __cplusplus
extern "C" {
#endif

struct domain {
  int dom_family;
  const char *dom_name;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_DOMAIN_H */
