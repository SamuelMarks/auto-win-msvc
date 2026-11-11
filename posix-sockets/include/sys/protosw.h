#ifndef SYS_PROTOSW_H
#define SYS_PROTOSW_H

/**
 * @file protosw.h
 * @brief BSD protocol switch table (<sys/protosw.h>).
 */

#ifdef __cplusplus
extern "C" {
#endif

#define PR_ATOMIC 0x01
#define PR_ADDR 0x02
#define PR_CONNREQUIRED 0x04
#define PR_WANTRCVD 0x08
#define PR_RIGHTS 0x10

struct protosw {
  short pr_type;
  struct domain *pr_domain;
  short pr_protocol;
  short pr_flags;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_PROTOSW_H */
