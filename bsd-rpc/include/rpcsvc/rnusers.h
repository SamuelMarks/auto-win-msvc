#ifndef RPCSVC_RNUSERS_H
#define RPCSVC_RNUSERS_H

/**
 * @file rnusers.h
 * @brief Remote users RPC protocol (<rpcsvc/rnusers.h>).
 */

/* clang-format off */
#include <rpc/rpc.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define RUSERSPROG 100002UL
#define RUSERSVERS_IDLE 2UL
#define RUSERSVERS_3 3UL
#define RUSERSPROC_NAMES 1UL
#define RUSERSPROC_ALLNAMES 2UL
#define MAXUSERS 100

#ifdef ut_time
#undef ut_time
#endif
struct ru_utmp {
  char ut_line[8];
  char ut_name[8];
  char ut_host[16];
  int ut_time;
};

struct utmpidle {
  struct ru_utmp ui_utmp;
  u_int ui_idle;
};
typedef struct utmpidle utmpidle;

struct rusers_utmp {
  char *ut_user;
  char *ut_line;
  char *ut_host;
  int ut_type;
  int ut_time;
  u_int ut_idle;
};
typedef struct rusers_utmp rusers_utmp;

struct utmp_array {
  u_int utmp_array_len;
  rusers_utmp *utmp_array_val;
};
typedef struct utmp_array utmp_array;

struct utmpidlearr {
  struct utmpidle **uia_arr;
  int uia_cnt;
  u_int utmpidlearr_len;
  utmpidle *utmpidlearr_val;
};
typedef struct utmpidlearr utmpidlearr;

bool_t xdr_utmpidlearr(XDR *xdrs, struct utmpidlearr *objp);
bool_t xdr_utmp_array(XDR *xdrs, struct utmp_array *objp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPCSVC_RNUSERS_H */
