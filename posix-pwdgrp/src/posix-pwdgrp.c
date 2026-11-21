/* posix-pwdgrp.c - Strict C89 Implementation */
#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)

/**
 * @file posix-pwdgrp.c
 * @brief Implementation of POSIX pwd.h and grp.h user/group database polyfills.
 */

/* clang-format off */
#if !defined(_WIN32)
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif
#ifndef _BSD_SOURCE
#define _BSD_SOURCE 1
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#endif

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <errno.h>
#include <lm.h>
#include <sddl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winnls.h>
#include "posix-pwdgrp.h"
#elif defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "posix-pwdgrp.h"
#else
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "posix-pwdgrp.h"
#endif
/* clang-format on */

#ifndef PWDGRP_U_FMT
#define PWDGRP_U_FMT "%u"
#endif

#ifdef _WIN32

#ifndef ENOENT
#define ENOENT 2
#endif
#ifndef ERANGE
#define ERANGE 34
#endif
#ifndef EINVAL
#define EINVAL 22
#endif
#ifndef ENOMEM
#define ENOMEM 12
#endif

#if defined(_MSC_VER)
#define PWDGRP_THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
#define PWDGRP_THREAD_LOCAL __thread
#else
#define PWDGRP_THREAD_LOCAL
#endif

static void *default_pwdgrp_malloc(size_t sz) { return malloc(sz); }

static int default_is_valid_sid(void *sid) {
  return (int)IsValidSid((PSID)sid);
}

static unsigned char *default_sid_sub_auth_count(void *sid) {
  return (unsigned char *)GetSidSubAuthorityCount((PSID)sid);
}

static unsigned long *default_sid_sub_auth(void *sid, unsigned long idx) {
  return (unsigned long *)GetSidSubAuthority((PSID)sid, (DWORD)idx);
}

static int default_lookup_account(const char *name, void *sid,
                                  unsigned long *cb_sid, char *dom,
                                  unsigned long *cb_dom, int *pe_use) {
  SID_NAME_USE u = SidTypeUnknown;
  BOOL ok = LookupAccountNameA(NULL, name, (PSID)sid, (LPDWORD)cb_sid, dom,
                               (LPDWORD)cb_dom, &u);
  *pe_use = (int)u;
  return (int)ok;
}

static unsigned long default_get_last_error(void) {
  return (unsigned long)GetLastError();
}

static int default_wide_to_utf8(const unsigned short *wstr, char *out,
                                int out_sz) {
  return WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wstr, -1, out, out_sz, NULL,
                             NULL);
}

static int default_utf8_to_wide(const char *str, unsigned short *out,
                                int out_sz) {
  return MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)out, out_sz);
}

static unsigned long default_net_group_enum(unsigned char **buf,
                                            unsigned long *entries_read,
                                            unsigned long *total_entries,
                                            size_t *resume) {
  DWORD_PTR res = (DWORD_PTR)(*resume);
  NET_API_STATUS st =
      NetLocalGroupEnum(NULL, 0, buf, MAX_PREFERRED_LENGTH,
                        (LPDWORD)entries_read, (LPDWORD)total_entries, &res);
  *resume = (size_t)res;
  return (unsigned long)st;
}

static unsigned long default_net_user_enum(unsigned char **buf,
                                           unsigned long *entries_read,
                                           unsigned long *total_entries,
                                           size_t *resume) {
  DWORD res = (DWORD)(*resume);
  NET_API_STATUS st =
      NetUserEnum(NULL, 0, FILTER_NORMAL_ACCOUNT, buf, MAX_PREFERRED_LENGTH,
                  (LPDWORD)entries_read, (LPDWORD)total_entries, &res);
  *resume = (size_t)res;
  return (unsigned long)st;
}

static unsigned long default_net_user_get_info(const unsigned short *wname,
                                               unsigned char **buf) {
  return (unsigned long)NetUserGetInfo(NULL, (LPCWSTR)wname, 4, buf);
}

static unsigned long default_net_buf_free(void *buf) {
  return (unsigned long)NetApiBufferFree(buf);
}

static void *(*g_pwdgrp_malloc)(size_t) = default_pwdgrp_malloc;
static int (*g_is_valid_sid)(void *) = default_is_valid_sid;
static unsigned char *(*g_sid_sub_auth_count)(void *) =
    default_sid_sub_auth_count;
static unsigned long *(*g_sid_sub_auth)(void *,
                                        unsigned long) = default_sid_sub_auth;
static int (*g_lookup_account)(const char *, void *, unsigned long *, char *,
                               unsigned long *, int *) = default_lookup_account;
static unsigned long (*g_get_last_error)(void) = default_get_last_error;
static int (*g_wide_to_utf8)(const unsigned short *, char *,
                             int) = default_wide_to_utf8;
static int (*g_utf8_to_wide)(const char *, unsigned short *,
                             int) = default_utf8_to_wide;
static unsigned long (*g_net_group_enum)(unsigned char **, unsigned long *,
                                         unsigned long *,
                                         size_t *) = default_net_group_enum;
static unsigned long (*g_net_user_enum)(unsigned char **, unsigned long *,
                                        unsigned long *,
                                        size_t *) = default_net_user_enum;
static unsigned long (*g_net_user_get_info)(
    const unsigned short *, unsigned char **) = default_net_user_get_info;
static unsigned long (*g_net_buf_free)(void *) = default_net_buf_free;

void posix_pwdgrp_set_win32_hooks(
    const struct posix_pwdgrp_win32_hooks *hooks) {
  if (hooks == NULL) {
    g_pwdgrp_malloc = default_pwdgrp_malloc;
    g_is_valid_sid = default_is_valid_sid;
    g_sid_sub_auth_count = default_sid_sub_auth_count;
    g_sid_sub_auth = default_sid_sub_auth;
    g_lookup_account = default_lookup_account;
    g_get_last_error = default_get_last_error;
    g_wide_to_utf8 = default_wide_to_utf8;
    g_utf8_to_wide = default_utf8_to_wide;
    g_net_group_enum = default_net_group_enum;
    g_net_user_enum = default_net_user_enum;
    g_net_user_get_info = default_net_user_get_info;
    g_net_buf_free = default_net_buf_free;
  } else {
    g_pwdgrp_malloc = hooks->malloc_fn;
    g_is_valid_sid = hooks->is_valid_sid_fn;
    g_sid_sub_auth_count = hooks->sid_sub_auth_count_fn;
    g_sid_sub_auth = hooks->sid_sub_auth_fn;
    g_lookup_account = hooks->lookup_account_fn;
    g_get_last_error = hooks->get_last_error_fn;
    g_wide_to_utf8 = hooks->wide_to_utf8_fn;
    g_utf8_to_wide = hooks->utf8_to_wide_fn;
    g_net_group_enum = hooks->net_group_enum_fn;
    g_net_user_enum = hooks->net_user_enum_fn;
    g_net_user_get_info = hooks->net_user_get_info_fn;
    g_net_buf_free = hooks->net_buf_free_fn;
  }
}

static PWDGRP_THREAD_LOCAL struct passwd static_pwd;
static PWDGRP_THREAD_LOCAL char static_pwd_buffer[2048];

static PWDGRP_THREAD_LOCAL struct group static_grp;
static PWDGRP_THREAD_LOCAL char static_grp_buffer[2048];

static PWDGRP_THREAD_LOCAL size_t pwd_enum_resume_handle = 0;
static PWDGRP_THREAD_LOCAL LPUSER_INFO_0 current_pwd_info = NULL;
static PWDGRP_THREAD_LOCAL unsigned long current_pwd_entries_read = 0;
static PWDGRP_THREAD_LOCAL unsigned long current_pwd_entry_index = 0;

static PWDGRP_THREAD_LOCAL size_t grp_enum_resume_handle = 0;
static PWDGRP_THREAD_LOCAL LPLOCALGROUP_INFO_0 current_grp_info = NULL;
static PWDGRP_THREAD_LOCAL unsigned long current_grp_entries_read = 0;
static PWDGRP_THREAD_LOCAL unsigned long current_grp_entry_index = 0;

static error_type_t copy_string(char **buffer, size_t *bufsize, const char *src,
                                char **out_res) {
  size_t len = strlen(src);
  char *res;
  if (len + 1 > *bufsize) {
    return ERANGE;
  }
  res = *buffer;
  if (len > 0) {
#if defined(_MSC_VER)
    memcpy_s(res, *bufsize, src, len);
#else
    memcpy(res, src, len);
#endif
  }
  res[len] = '\0';
  *buffer += (len + 1);
  *bufsize -= (len + 1);
  *out_res = res;
  return ERR_NONE;
}

static error_type_t sid_to_rid(PSID sid, uid_t *out_rid) {
  unsigned char *count;
  if (!g_is_valid_sid(sid)) {
    return EINVAL;
  }
  count = g_sid_sub_auth_count(sid);
  if (count == NULL || *count == 0) {
    return EINVAL;
  }
  *out_rid = (uid_t)(*g_sid_sub_auth(sid, (unsigned long)(*count - 1)));
  return ERR_NONE;
}

static error_type_t get_sid_from_name(const char *name, int *peUse,
                                      PSID *out_sid) {
  unsigned long cbSid = 0;
  unsigned long cbDomain = 0;
  PSID sid = NULL;
  char *domain = NULL;

  (void)g_lookup_account(name, NULL, &cbSid, NULL, &cbDomain, peUse);
  if (g_get_last_error() != ERROR_INSUFFICIENT_BUFFER) {
    return ENOENT;
  }
  sid = (PSID)g_pwdgrp_malloc((size_t)cbSid);
  if (sid == NULL) {
    return ENOMEM;
  }
  domain = (char *)g_pwdgrp_malloc((size_t)cbDomain);
  if (domain == NULL) {
    free(sid);
    return ENOMEM;
  }
  if (!g_lookup_account(name, sid, &cbSid, domain, &cbDomain, peUse)) {
    free(sid);
    free(domain);
    return ENOENT;
  }
  free(domain);
  *out_sid = sid;
  return ERR_NONE;
}

static error_type_t utf16_to_utf8(const unsigned short *wstr, char **out_str) {
  int size_needed;
  char *strTo;
  if (wstr == NULL) {
    return EINVAL;
  }
  size_needed = g_wide_to_utf8(wstr, NULL, 0);
  if (size_needed <= 0) {
    return EINVAL;
  }
  strTo = (char *)g_pwdgrp_malloc((size_t)size_needed);
  if (strTo == NULL) {
    return ENOMEM;
  }
  (void)g_wide_to_utf8(wstr, strTo, size_needed);
  *out_str = strTo;
  return ERR_NONE;
}

static int populate_group_record(struct group *grp, gid_t gid, const char *name,
                                 char *buffer, size_t bufsize,
                                 struct group **result) {
  size_t align_offset;
  grp->gr_gid = gid;
  if (copy_string(&buffer, &bufsize, name, &grp->gr_name) != ERR_NONE) {
    return ERANGE;
  }
  if (copy_string(&buffer, &bufsize, "*", &grp->gr_passwd) != ERR_NONE) {
    return ERANGE;
  }
  align_offset = (size_t)buffer % sizeof(char *);
  if (align_offset != 0) {
    align_offset = sizeof(char *) - align_offset;
    if (bufsize < align_offset) {
      return ERANGE;
    }
    buffer += align_offset;
    bufsize -= align_offset;
  }
  if (bufsize < sizeof(char *)) {
    return ERANGE;
  }
  grp->gr_mem = (char **)buffer;
  grp->gr_mem[0] = NULL;
  *result = grp;
  return 0;
}

static int populate_passwd_record(struct passwd *pwd, uid_t uid,
                                  const char *name, const char *gecos,
                                  const char *dir, char *buffer, size_t bufsize,
                                  struct passwd **result) {
  pwd->pw_uid = uid;
  pwd->pw_gid = uid;
  pwd->pw_change = 0;
  pwd->pw_expire = 0;
  pwd->pw_fields = 0;
  if (copy_string(&buffer, &bufsize, name, &pwd->pw_name) != ERR_NONE) {
    return ERANGE;
  }
  if (copy_string(&buffer, &bufsize, "*", &pwd->pw_passwd) != ERR_NONE) {
    return ERANGE;
  }
  pwd->pw_class = pwd->pw_passwd + 1;
  if (copy_string(&buffer, &bufsize, dir, &pwd->pw_dir) != ERR_NONE) {
    return ERANGE;
  }
  if (copy_string(&buffer, &bufsize, gecos, &pwd->pw_gecos) != ERR_NONE) {
    return ERANGE;
  }
  if (copy_string(&buffer, &bufsize, "cmd.exe", &pwd->pw_shell) != ERR_NONE) {
    return ERANGE;
  }
  *result = pwd;
  return 0;
}

void endgrent(void) {
  if (current_grp_info != NULL) {
    (void)g_net_buf_free(current_grp_info);
    current_grp_info = NULL;
  }
  current_grp_entries_read = 0;
  current_grp_entry_index = 0;
  grp_enum_resume_handle = 0;
}

void setgrent(void) { endgrent(); }

struct group *getgrent(void) {
  unsigned long total_entries = 0;
  unsigned long nStatus;
  char *utf8_name = NULL;
  struct group *res;
  error_type_t rc;

  if (current_grp_entry_index >= current_grp_entries_read) {
    if (current_grp_info != NULL) {
      (void)g_net_buf_free(current_grp_info);
      current_grp_info = NULL;
    }
    current_grp_entries_read = 0;
    current_grp_entry_index = 0;

    nStatus = g_net_group_enum((unsigned char **)&current_grp_info,
                               &current_grp_entries_read, &total_entries,
                               &grp_enum_resume_handle);
    if (nStatus != NERR_Success && nStatus != ERROR_MORE_DATA) {
      return NULL;
    }
    if (current_grp_entries_read == 0) {
      return NULL;
    }
  }

  rc = utf16_to_utf8(
      (const unsigned short *)current_grp_info[current_grp_entry_index]
          .lgrpi0_name,
      &utf8_name);
  current_grp_entry_index++;
  if (rc != ERR_NONE) {
    return NULL;
  }
  res = getgrnam(utf8_name);
  free(utf8_name);
  return res;
}

int getgrnam_r(const char *name, struct group *grp, char *buffer,
               size_t bufsize, struct group **result) {
  int peUse = 0;
  PSID sid = NULL;
  gid_t gid = 0;

  if (!name || !grp || !buffer || !result) {
    return EINVAL;
  }
  *result = NULL;

  if (get_sid_from_name(name, &peUse, &sid) != ERR_NONE) {
    if (strcmp(name, "root") == 0 || strcmp(name, "wheel") == 0 ||
        strcmp(name, "users") == 0) {
      gid = (strcmp(name, "users") == 0) ? (gid_t)100 : (gid_t)0;
      return populate_group_record(grp, gid, name, buffer, bufsize, result);
    }
    return ENOENT;
  }

  if (peUse != SidTypeAlias && peUse != SidTypeGroup &&
      peUse != SidTypeWellKnownGroup) {
    free(sid);
    return ENOENT;
  }
  if (sid_to_rid(sid, &gid) != ERR_NONE) {
    free(sid);
    return EINVAL;
  }
  free(sid);
  return populate_group_record(grp, gid, name, buffer, bufsize, result);
}

struct group *getgrnam(const char *name) {
  struct group *res = NULL;
  if (getgrnam_r(name, &static_grp, static_grp_buffer,
                 sizeof(static_grp_buffer), &res) == 0) {
    return res;
  }
  return NULL;
}

int getgrgid_r(gid_t gid, struct group *grp, char *buffer, size_t bufsize,
               struct group **result) {
  LPLOCALGROUP_INFO_0 groupInfo = NULL;
  unsigned long entriesRead = 0;
  unsigned long totalEntries = 0;
  size_t resumeHandle = 0;
  unsigned long nStatus;
  unsigned long i;
  char *found_name = NULL;

  if (!grp || !buffer || !result) {
    return EINVAL;
  }
  *result = NULL;

  do {
    nStatus = g_net_group_enum((unsigned char **)&groupInfo, &entriesRead,
                               &totalEntries, &resumeHandle);
    if (nStatus == NERR_Success || nStatus == ERROR_MORE_DATA) {
      for (i = 0; i < entriesRead && found_name == NULL; ++i) {
        char *utf8_name = NULL;
        if (utf16_to_utf8((const unsigned short *)groupInfo[i].lgrpi0_name,
                          &utf8_name) == ERR_NONE) {
          int peUse = 0;
          PSID sid = NULL;
          if (get_sid_from_name(utf8_name, &peUse, &sid) == ERR_NONE) {
            uid_t r_uid = 0;
            if (sid_to_rid(sid, &r_uid) == ERR_NONE && r_uid == gid) {
              found_name = utf8_name;
            }
            free(sid);
          }
          if (found_name == NULL) {
            free(utf8_name);
          }
        }
      }
      if (groupInfo != NULL) {
        (void)g_net_buf_free(groupInfo);
        groupInfo = NULL;
      }
    }
  } while (nStatus == ERROR_MORE_DATA && found_name == NULL);

  if (found_name != NULL) {
    int res = getgrnam_r(found_name, grp, buffer, bufsize, result);
    free(found_name);
    return res;
  }

  if (gid < (gid_t)65536) {
    const char *fb_grp = (gid == 0) ? "root" : "users";
    int r = getgrnam_r(fb_grp, grp, buffer, bufsize, result);
    if (r == 0) {
      grp->gr_gid = gid;
    }
    return r;
  }
  return ENOENT;
}

struct group *getgrgid(gid_t gid) {
  struct group *res = NULL;
  if (getgrgid_r(gid, &static_grp, static_grp_buffer, sizeof(static_grp_buffer),
                 &res) == 0) {
    return res;
  }
  return NULL;
}

void endpwent(void) {
  if (current_pwd_info != NULL) {
    (void)g_net_buf_free(current_pwd_info);
    current_pwd_info = NULL;
  }
  current_pwd_entries_read = 0;
  current_pwd_entry_index = 0;
  pwd_enum_resume_handle = 0;
}

void setpwent(void) { endpwent(); }

struct passwd *getpwent(void) {
  unsigned long total_entries = 0;
  unsigned long nStatus;
  char *utf8_name = NULL;
  struct passwd *res;
  error_type_t rc;

  if (current_pwd_entry_index >= current_pwd_entries_read) {
    if (current_pwd_info != NULL) {
      (void)g_net_buf_free(current_pwd_info);
      current_pwd_info = NULL;
    }
    current_pwd_entries_read = 0;
    current_pwd_entry_index = 0;

    nStatus = g_net_user_enum((unsigned char **)&current_pwd_info,
                              &current_pwd_entries_read, &total_entries,
                              &pwd_enum_resume_handle);
    if (nStatus != NERR_Success && nStatus != ERROR_MORE_DATA) {
      return NULL;
    }
    if (current_pwd_entries_read == 0) {
      return NULL;
    }
  }

  rc = utf16_to_utf8(
      (const unsigned short *)current_pwd_info[current_pwd_entry_index]
          .usri0_name,
      &utf8_name);
  current_pwd_entry_index++;
  if (rc != ERR_NONE) {
    return NULL;
  }
  res = getpwnam(utf8_name);
  free(utf8_name);
  return res;
}

int getpwnam_r(const char *name, struct passwd *pwd, char *buffer,
               size_t bufsize, struct passwd **result) {
  int peUse = 0;
  PSID sid = NULL;
  LPUSER_INFO_4 userInfo = NULL;
  int wlen;
  uid_t uid = 0;
  char *dir = NULL;
  char *gecos = NULL;
  int rc;

  if (!name || !pwd || !buffer || !result) {
    return EINVAL;
  }
  *result = NULL;

  if (get_sid_from_name(name, &peUse, &sid) != ERR_NONE) {
    if (strcmp(name, "root") == 0) {
      return populate_passwd_record(pwd, (uid_t)0, "root", "root", "C:", buffer,
                                    bufsize, result);
    }
    return ENOENT;
  }

  if (peUse != SidTypeUser) {
    free(sid);
    return ENOENT;
  }

  wlen = g_utf8_to_wide(name, NULL, 0);
  if (wlen > 0) {
    unsigned short *wname = (unsigned short *)g_pwdgrp_malloc(
        (size_t)wlen * sizeof(unsigned short));
    if (wname != NULL) {
      unsigned long net_st;
      (void)g_utf8_to_wide(name, wname, wlen);
      net_st = g_net_user_get_info(wname, (unsigned char **)&userInfo);
      free(wname);
      if (net_st == NERR_UserNotFound) {
        free(sid);
        return ENOENT;
      }
    }
  }

  if (sid_to_rid(sid, &uid) != ERR_NONE) {
    free(sid);
    if (userInfo != NULL) {
      (void)g_net_buf_free(userInfo);
    }
    return EINVAL;
  }
  free(sid);

  if (userInfo != NULL) {
    if (userInfo->usri4_home_dir != NULL &&
        utf16_to_utf8((const unsigned short *)userInfo->usri4_home_dir, &dir) !=
            ERR_NONE) {
      (void)g_net_buf_free(userInfo);
      return ENOMEM;
    }
    if (userInfo->usri4_full_name != NULL &&
        utf16_to_utf8((const unsigned short *)userInfo->usri4_full_name,
                      &gecos) != ERR_NONE) {
      if (dir != NULL) {
        free(dir);
      }
      (void)g_net_buf_free(userInfo);
      return ENOMEM;
    }
    (void)g_net_buf_free(userInfo);
  }

  rc = populate_passwd_record(pwd, uid, name, (gecos != NULL) ? gecos : "",
                              (dir != NULL) ? dir : "C:", buffer, bufsize,
                              result);
  if (dir != NULL) {
    free(dir);
  }
  if (gecos != NULL) {
    free(gecos);
  }
  return rc;
}

struct passwd *getpwnam(const char *name) {
  struct passwd *res = NULL;
  if (getpwnam_r(name, &static_pwd, static_pwd_buffer,
                 sizeof(static_pwd_buffer), &res) == 0) {
    return res;
  }
  return NULL;
}

int getpwuid_r(uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize,
               struct passwd **result) {
  LPUSER_INFO_0 userInfo = NULL;
  unsigned long entriesRead = 0;
  unsigned long totalEntries = 0;
  size_t resumeHandle = 0;
  unsigned long nStatus;
  unsigned long i;
  char *found_name = NULL;

  if (!pwd || !buffer || !result) {
    return EINVAL;
  }
  *result = NULL;

  do {
    nStatus = g_net_user_enum((unsigned char **)&userInfo, &entriesRead,
                              &totalEntries, &resumeHandle);
    if (nStatus == NERR_Success || nStatus == ERROR_MORE_DATA) {
      for (i = 0; i < entriesRead && found_name == NULL; ++i) {
        char *utf8_name = NULL;
        if (utf16_to_utf8((const unsigned short *)userInfo[i].usri0_name,
                          &utf8_name) == ERR_NONE) {
          int peUse = 0;
          PSID sid = NULL;
          if (get_sid_from_name(utf8_name, &peUse, &sid) == ERR_NONE) {
            uid_t r_uid = 0;
            if (sid_to_rid(sid, &r_uid) == ERR_NONE && r_uid == uid) {
              found_name = utf8_name;
            }
            free(sid);
          }
          if (found_name == NULL) {
            free(utf8_name);
          }
        }
      }
      if (userInfo != NULL) {
        (void)g_net_buf_free(userInfo);
        userInfo = NULL;
      }
    }
  } while (nStatus == ERROR_MORE_DATA && found_name == NULL);

  if (found_name != NULL) {
    int res = getpwnam_r(found_name, pwd, buffer, bufsize, result);
    free(found_name);
    return res;
  }

  if (uid < (uid_t)65536) {
    int r = getpwnam_r("root", pwd, buffer, bufsize, result);
    if (r == 0) {
      pwd->pw_uid = uid;
    }
    return r;
  }
  return ENOENT;
}

struct passwd *getpwuid(uid_t uid) {
  struct passwd *res = NULL;
  if (getpwuid_r(uid, &static_pwd, static_pwd_buffer, sizeof(static_pwd_buffer),
                 &res) == 0) {
    return res;
  }
  return NULL;
}

int getgrouplist(const char *user, gid_t group, gid_t *groups, int *ngroups) {
  if (user == NULL || ngroups == NULL) {
    return -1;
  }
  if (*ngroups < 1) {
    *ngroups = 1;
    return -1;
  }
  if (groups != NULL) {
    groups[0] = group;
  }
  *ngroups = 1;
  return 1;
}

#elif defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)

static int s_dos_pw_idx = 0;
static int s_dos_gr_idx = 0;
static struct passwd s_dos_pwd;
static char s_dos_pwd_buf[256];
static struct group s_dos_grp;
static char s_dos_grp_buf[256];

static int fill_dos_passwd(struct passwd *pwd, char *buffer, size_t bufsize,
                           struct passwd **result) {
  if (!pwd || !buffer || !result) {
    return EINVAL;
  }
  *result = NULL;
  if (bufsize < 64) {
    return ERANGE;
  }
  strcpy(buffer, "root");
  pwd->pw_name = buffer;
  strcpy(buffer + 5, "*");
  pwd->pw_passwd = buffer + 5;
  strcpy(buffer + 7, "root");
  pwd->pw_gecos = buffer + 7;
  strcpy(buffer + 12, "C:\134");
  pwd->pw_dir = buffer + 12;
  strcpy(buffer + 16, "COMMAND.COM");
  pwd->pw_shell = buffer + 16;
  pwd->pw_uid = 0;
  pwd->pw_gid = 0;
  *result = pwd;
  return 0;
}

static int fill_dos_group(struct group *grp, char *buffer, size_t bufsize,
                          struct group **result) {
  size_t align_off;
  if (!grp || !buffer || !result) {
    return EINVAL;
  }
  *result = NULL;
  if (bufsize < 64) {
    return ERANGE;
  }
  strcpy(buffer, "root");
  grp->gr_name = buffer;
  strcpy(buffer + 5, "*");
  grp->gr_passwd = buffer + 5;
  grp->gr_gid = 0;
  align_off = ((size_t)(buffer + 8)) % sizeof(char *);
  if (align_off != 0) {
    align_off = sizeof(char *) - align_off;
  }
  grp->gr_mem = (char **)(buffer + 8 + align_off);
  grp->gr_mem[0] = NULL;
  *result = grp;
  return 0;
}

void setpwent(void) { s_dos_pw_idx = 0; }
void endpwent(void) { s_dos_pw_idx = 0; }
struct passwd *getpwent(void) {
  struct passwd *res = NULL;
  if (s_dos_pw_idx++ == 0) {
    (void)fill_dos_passwd(&s_dos_pwd, s_dos_pwd_buf, sizeof(s_dos_pwd_buf),
                          &res);
  }
  return res;
}

int getpwnam_r(const char *name, struct passwd *pwd, char *buffer,
               size_t bufsize, struct passwd **result) {
  if (!name || !pwd || !buffer || !result) {
    return EINVAL;
  }
  *result = NULL;
  if (strcmp(name, "root") == 0 || strcmp(name, "dos") == 0) {
    return fill_dos_passwd(pwd, buffer, bufsize, result);
  }
  return 0;
}

struct passwd *getpwnam(const char *name) {
  struct passwd *res = NULL;
  if (name && (strcmp(name, "root") == 0 || strcmp(name, "dos") == 0)) {
    (void)fill_dos_passwd(&s_dos_pwd, s_dos_pwd_buf, sizeof(s_dos_pwd_buf),
                          &res);
  }
  return res;
}

int getpwuid_r(uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize,
               struct passwd **result) {
  if (!pwd || !buffer || !result) {
    return EINVAL;
  }
  *result = NULL;
  if (uid == 0) {
    return fill_dos_passwd(pwd, buffer, bufsize, result);
  }
  return 0;
}

struct passwd *getpwuid(uid_t uid) {
  struct passwd *res = NULL;
  if (uid == 0) {
    (void)fill_dos_passwd(&s_dos_pwd, s_dos_pwd_buf, sizeof(s_dos_pwd_buf),
                          &res);
  }
  return res;
}

void setgrent(void) { s_dos_gr_idx = 0; }
void endgrent(void) { s_dos_gr_idx = 0; }
struct group *getgrent(void) {
  struct group *res = NULL;
  if (s_dos_gr_idx++ == 0) {
    (void)fill_dos_group(&s_dos_grp, s_dos_grp_buf, sizeof(s_dos_grp_buf),
                         &res);
  }
  return res;
}

int getgrnam_r(const char *name, struct group *grp, char *buffer,
               size_t bufsize, struct group **result) {
  if (!name || !grp || !buffer || !result) {
    return EINVAL;
  }
  *result = NULL;
  if (strcmp(name, "root") == 0 || strcmp(name, "wheel") == 0 ||
      strcmp(name, "users") == 0) {
    return fill_dos_group(grp, buffer, bufsize, result);
  }
  return 0;
}

struct group *getgrnam(const char *name) {
  struct group *res = NULL;
  if (name && (strcmp(name, "root") == 0 || strcmp(name, "wheel") == 0 ||
               strcmp(name, "users") == 0)) {
    (void)fill_dos_group(&s_dos_grp, s_dos_grp_buf, sizeof(s_dos_grp_buf),
                         &res);
  }
  return res;
}

int getgrgid_r(gid_t gid, struct group *grp, char *buffer, size_t bufsize,
               struct group **result) {
  if (!grp || !buffer || !result) {
    return EINVAL;
  }
  *result = NULL;
  if (gid == 0) {
    return fill_dos_group(grp, buffer, bufsize, result);
  }
  return 0;
}

struct group *getgrgid(gid_t gid) {
  struct group *res = NULL;
  if (gid == 0) {
    (void)fill_dos_group(&s_dos_grp, s_dos_grp_buf, sizeof(s_dos_grp_buf),
                         &res);
  }
  return res;
}

int getgrouplist(const char *user, gid_t group, gid_t *groups, int *ngroups) {
  if (user == NULL || ngroups == NULL) {
    return -1;
  }
  if (*ngroups < 1) {
    *ngroups = 1;
    return -1;
  }
  if (groups != NULL) {
    groups[0] = group;
  }
  *ngroups = 1;
  return 1;
}

#else /* _WIN32 */

typedef int dummy_posix_pwdgrp_c_typedef;

#endif /* _WIN32 */

#if !defined(__APPLE__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&   \
    !defined(__NetBSD__)

static char g_uid_num_buf[32];
static char g_gid_num_buf[32];

const char *user_from_uid(uid_t uid, int nouser) {
  struct passwd *pw = getpwuid(uid);
  if (pw != NULL) {
    return pw->pw_name;
  }
  if (nouser) {
    return NULL;
  }
#if defined(_MSC_VER)
  sprintf_s(g_uid_num_buf, sizeof(g_uid_num_buf), PWDGRP_U_FMT,
            (unsigned int)uid);
#else
  sprintf(g_uid_num_buf, PWDGRP_U_FMT, (unsigned int)uid);
#endif
  return g_uid_num_buf;
}

const char *group_from_gid(gid_t gid, int nogroup) {
  struct group *gr = getgrgid(gid);
  if (gr != NULL) {
    return gr->gr_name;
  }
  if (nogroup) {
    return NULL;
  }
#if defined(_MSC_VER)
  sprintf_s(g_gid_num_buf, sizeof(g_gid_num_buf), PWDGRP_U_FMT,
            (unsigned int)gid);
#else
  sprintf(g_gid_num_buf, PWDGRP_U_FMT, (unsigned int)gid);
#endif
  return g_gid_num_buf;
}

int setpassent(int stayopen) {
  (void)stayopen;
  setpwent();
  return 1;
}

int setgroupent(int stayopen) {
  (void)stayopen;
  setgrent();
  return 1;
}

#endif /* !__APPLE__ && !BSD */

#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__)

int uid_from_user(const char *name, uid_t *uid) {
  struct passwd *pw;
  if (name == NULL || uid == NULL) {
    return -1;
  }
  pw = getpwnam(name);
  if (pw == NULL) {
    return -1;
  }
  *uid = pw->pw_uid;
  return 0;
}

int gid_from_group(const char *name, gid_t *gid) {
  struct group *gr;
  if (name == NULL || gid == NULL) {
    return -1;
  }
  gr = getgrnam(name);
  if (gr == NULL) {
    return -1;
  }
  *gid = gr->gr_gid;
  return 0;
}

int pwcache_userdb(int (*a_setpassent)(int), void (*a_endpwent)(void),
                   struct passwd *(*a_getpwnam)(const char *),
                   struct passwd *(*a_getpwuid)(uid_t)) {
  (void)a_setpassent;
  (void)a_endpwent;
  (void)a_getpwnam;
  (void)a_getpwuid;
  return 0;
}

int pwcache_groupdb(int (*a_setgroupent)(int), void (*a_endgrent)(void),
                    struct group *(*a_getgrnam)(const char *),
                    struct group *(*a_getgrgid)(gid_t)) {
  (void)a_setgroupent;
  (void)a_endgrent;
  (void)a_getgrnam;
  (void)a_getgrgid;
  return 0;
}

#endif /* !BSD */

/**
 * @brief No-op helper preventing empty translation unit warnings.
 * @return Always 0.
 */
int dummy_posix_pwdgrp(void);
int dummy_posix_pwdgrp(void) { return 0; }
typedef int make_iso_compilers_happy_tu_posix_pwdgrp;

#endif
typedef int make_iso_compilers_happy_mega_batch3_11;
