#ifndef RPCSVC_YPCLNT_H
#define RPCSVC_YPCLNT_H

/**
 * @file ypclnt.h
 * @brief NIS/YP client interface (<rpcsvc/ypclnt.h>).
 */

#ifdef __cplusplus
extern "C" {
#endif

#define YPERR_BADARGS 1
#define YPERR_RPC 2
#define YPERR_DOMAIN 3
#define YPERR_MAP 4
#define YPERR_KEY 5
#define YPERR_YPERR 6
#define YPERR_RESRC 7
#define YPERR_NOMORE 8
#define YPERR_PMAP 9
#define YPERR_YPBIND 10
#define YPERR_YPSERV 11
#define YPERR_NODOM 12
#define YPERR_BADDB 13
#define YPERR_VERS 14
#define YPERR_ACCESS 15
#define YPERR_BUSY 16

struct ypall_callback {
  int (*foreach)(int status, char *key, int keylen, char *val, int vallen,
                 char *data);
  char *data;
};

struct ypmaplist;

int yp_bind(const char *indomain);
void yp_unbind(const char *indomain);
int yp_get_default_domain(char **outdomain);
int yp_match(const char *indomain, const char *inmap, const char *inkey,
             int inkeylen, char **outval, int *outvallen);
int yp_first(const char *indomain, const char *inmap, char **outkey,
             int *outkeylen, char **outval, int *outvallen);
int yp_next(const char *indomain, const char *inmap, const char *inkey,
            int inkeylen, char **outkey, int *outkeylen, char **outval,
            int *outvallen);
int yp_master(const char *indomain, const char *inmap, char **outname);
int yp_order(const char *indomain, const char *inmap, int *outorder);
int yp_all(const char *indomain, const char *inmap,
           struct ypall_callback *incallback);
const char *yperr_string(int incode);
int ypprot_err(unsigned int incode);
int yp_maplist(const char *indomain, struct ypmaplist **outmaplist);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RPCSVC_YPCLNT_H */
