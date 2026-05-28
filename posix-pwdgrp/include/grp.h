/* Strict C89 grp.h wrapper */
#ifndef POSIX_PWDGRP_GRP_H
#define POSIX_PWDGRP_GRP_H
#include "auto_win_msvc_export.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <posix-types.h>

struct group {
  char *gr_name;
  char *gr_passwd;
  gid_t gr_gid;
  char **gr_mem;
};

struct group *getgrnam(const char *name);
struct group *getgrgid(gid_t gid);
AUTO_WIN_MSVC_EXPORT int getgrnam_r(const char *name, struct group *grp,
                                    char *buffer, size_t bufsize,
                                    struct group **result);
AUTO_WIN_MSVC_EXPORT int getgrgid_r(gid_t gid, struct group *grp, char *buffer,
                                    size_t bufsize, struct group **result);
AUTO_WIN_MSVC_EXPORT void endgrent(void);
struct group *getgrent(void);
AUTO_WIN_MSVC_EXPORT void setgrent(void);

#ifdef __cplusplus
}
#endif

#endif /* POSIX_PWDGRP_GRP_H */
