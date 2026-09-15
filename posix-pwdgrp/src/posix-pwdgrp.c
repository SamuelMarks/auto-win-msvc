/* posix-pwdgrp.c - Strict C89 Implementation */

/* Provide C89 compliant CRT secure overload definitions */
#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif

#if defined(_M_IX86)
#ifndef _X86_
#define _X86_
#endif
#elif defined(_M_AMD64) || defined(_M_X64)
#ifndef _AMD64_
#define _AMD64_
#endif
#elif defined(_M_ARM64)
#ifndef _ARM64_
#define _ARM64_
#endif
#elif defined(_M_ARM)
#ifndef _ARM_
#define _ARM_
#endif
#endif

/* clang-format off */
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
#else
#include "posix-pwdgrp.h"
#endif
/* clang-format on */

#ifdef _WIN32

#ifndef _ERROR_TYPE_T_DEFINED
#define _ERROR_TYPE_T_DEFINED
typedef int error_type_t;
/** @brief Success indicator for error_type_t. */
#define ERR_NONE 0
#endif

/* Link required Windows libraries */
#if defined(_MSC_VER)
#define PWDGRP_THREAD_LOCAL __declspec(thread)
#else
#define PWDGRP_THREAD_LOCAL __thread
#endif

#define PWD_BUFFER_SIZE 4096

static PWDGRP_THREAD_LOCAL struct passwd static_pwd;
static PWDGRP_THREAD_LOCAL char static_pwd_buffer[PWD_BUFFER_SIZE];

static PWDGRP_THREAD_LOCAL struct group static_grp;
static PWDGRP_THREAD_LOCAL char static_grp_buffer[PWD_BUFFER_SIZE];

static PWDGRP_THREAD_LOCAL DWORD current_resume_handle = 0;
static PWDGRP_THREAD_LOCAL PUSER_INFO_0 current_user_info = NULL;
static PWDGRP_THREAD_LOCAL DWORD current_entries_read = 0;
static PWDGRP_THREAD_LOCAL DWORD current_entry_index = 0;

static PWDGRP_THREAD_LOCAL DWORD_PTR current_grp_resume_handle = 0;
static PWDGRP_THREAD_LOCAL PLOCALGROUP_INFO_0 current_grp_info = NULL;
static PWDGRP_THREAD_LOCAL DWORD current_grp_entries_read = 0;
static PWDGRP_THREAD_LOCAL DWORD current_grp_entry_index = 0;

/**
 * @brief Safely copy string to the supplied buffer, adhering to strict bounds.
 * @param buffer Pointer to buffer pointer.
 * @param bufsize Pointer to available buffer size.
 * @param src Source string.
 * @param out_res Output pointer to destination string.
 * @return ERR_NONE on success, or ERANGE/EINVAL.
 */
static error_type_t copy_string(char **buffer, size_t *bufsize, const char *src,
                                char **out_res) {
  size_t len;
  char *res;

  if (buffer == NULL || *buffer == NULL || bufsize == NULL) {
    return EINVAL;
  }

  len = src ? strlen(src) : 0;
  if (len + 1 > *bufsize) {
    return ERANGE;
  }
  res = *buffer;
  if (src) {
#if defined(_MSC_VER)
    strcpy_s(res, *bufsize, src);
#else
    strncpy(res, src, *bufsize - 1);
    res[*bufsize - 1] = '\0';
#endif
  } else {
    res[0] = '\0';
  }
  *buffer += len + 1;
  *bufsize -= len + 1;
  if (out_res) {
    *out_res = res;
  }
  return ERR_NONE;
}

/**
 * @brief Obtain the RID from a SID.
 * @param sid Security identifier.
 * @param out_rid Pointer to receive numerical ID.
 * @return ERR_NONE on success, or EINVAL.
 */
static error_type_t sid_to_rid(PSID sid, uid_t *out_rid) {
  PUCHAR count;
  if (!IsValidSid(sid)) {
    return EINVAL;
  }
  count = GetSidSubAuthorityCount(sid);
  if (!count || *count == 0) {
    return EINVAL;
  }
  if (out_rid) {
    *out_rid = (uid_t)(*GetSidSubAuthority(sid, *count - 1));
  }
  return ERR_NONE;
}

/**
 * @brief Look up a Windows SID by name.
 * @param name User or group name.
 * @param peUse Pointer to receive SID use type.
 * @param out_sid Pointer to receive allocated SID.
 * @return ERR_NONE on success, or EINVAL/ENOENT/ENOMEM.
 */
static error_type_t get_sid_from_name(const char *name, SID_NAME_USE *peUse,
                                      PSID *out_sid) {
  DWORD cbSid = 0;
  DWORD cbDomain = 0;
  PSID sid = NULL;
  char *domain = NULL;

  if (name == NULL || out_sid == NULL || peUse == NULL) {
    return EINVAL;
  }
  *out_sid = NULL;

  LookupAccountNameA(NULL, name, NULL, &cbSid, NULL, &cbDomain, peUse);
  if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
    sid = (PSID)malloc(cbSid);
    domain = (char *)malloc(cbDomain);
    if (sid && domain) {
      if (!LookupAccountNameA(NULL, name, sid, &cbSid, domain, &cbDomain,
                              peUse)) {
        free(sid);
        free(domain);
        return ENOENT;
      }
    } else {
      if (sid)
        free(sid);
      if (domain)
        free(domain);
      return ENOMEM;
    }
    if (domain)
      free(domain);
  } else {
    return ENOENT;
  }
  *out_sid = sid;
  return ERR_NONE;
}

/**
 * @brief Convert UTF-16 to UTF-8.
 * @param wstr Wide string.
 * @param out_str Pointer to receive allocated UTF-8 string.
 * @return ERR_NONE on success, or EINVAL/ENOMEM.
 */
static error_type_t utf16_to_utf8(LPCWSTR wstr, char **out_str) {
  int size_needed;
  char *strTo;

  if (wstr == NULL || out_str == NULL) {
    return EINVAL;
  }
  *out_str = NULL;

  size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
  if (size_needed <= 0) {
    return EINVAL;
  }
  strTo = (char *)malloc((size_t)size_needed);
  if (strTo) {
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, strTo, size_needed, NULL, NULL);
    *out_str = strTo;
    return ERR_NONE;
  }
  return ENOMEM;
}

/* --- GROUP DATABASE --- */

void endgrent(void) { setgrent(); }

/** \brief setgrent function. */
void setgrent(void) {
  if (current_grp_info) {
    NetApiBufferFree(current_grp_info);
    current_grp_info = NULL;
  }
  current_grp_resume_handle = 0;
  current_grp_entries_read = 0;
  current_grp_entry_index = 0;
}

struct group *getgrent(void) {
  DWORD total_entries = 0;
  if (current_grp_entry_index >= current_grp_entries_read) {
    if (current_grp_info) {
      NetApiBufferFree(current_grp_info);
      current_grp_info = NULL;
    }
    if (NetLocalGroupEnum(NULL, 0, (LPBYTE *)&current_grp_info,
                          MAX_PREFERRED_LENGTH, &current_grp_entries_read,
                          &total_entries,
                          &current_grp_resume_handle) != NERR_Success) {
      return NULL;
    }
    current_grp_entry_index = 0;
    if (current_grp_entries_read == 0)
      return NULL;
  }

  {
    char *utf8_name = NULL;
    struct group *res = NULL;
    error_type_t rc;
    rc = utf16_to_utf8(current_grp_info[current_grp_entry_index].lgrpi0_name,
                       &utf8_name);
    current_grp_entry_index++;
    if (rc == ERR_NONE && utf8_name) {
      res = getgrnam(utf8_name);
      free(utf8_name);
    }
    return res;
  }
}

/** \brief getgrnam_r function. */
int getgrnam_r(const char *name, struct group *grp, char *buffer,
               size_t bufsize, struct group **result) {
  SID_NAME_USE peUse;
  PSID sid = NULL;
  size_t align_offset;
  error_type_t rc;

  if (!name || !grp || !buffer || !result)
    return EINVAL;
  *result = NULL;

  rc = get_sid_from_name(name, &peUse, &sid);
  if (rc != ERR_NONE)
    return ENOENT;

  if (peUse != SidTypeAlias && peUse != SidTypeGroup &&
      peUse != SidTypeWellKnownGroup) {
    free(sid);
    return ENOENT;
  }

  rc = sid_to_rid(sid, &grp->gr_gid);
  free(sid);
  if (rc != ERR_NONE)
    return EINVAL;

  rc = copy_string(&buffer, &bufsize, name, &grp->gr_name);
  if (rc != ERR_NONE)
    return rc;

  rc = copy_string(&buffer, &bufsize, "*", &grp->gr_passwd);
  if (rc != ERR_NONE)
    return rc;

  /* Align buffer for pointer arrays */
  align_offset = ((size_t)buffer) % sizeof(char *);
  if (align_offset != 0) {
    align_offset = sizeof(char *) - align_offset;
    if (bufsize < align_offset)
      return ERANGE;
    buffer += align_offset;
    bufsize -= align_offset;
  }
  if (bufsize < sizeof(char *))
    return ERANGE;

  grp->gr_mem = (char **)buffer;
  grp->gr_mem[0] = NULL;

  *result = grp;
  return 0;
}

struct group *getgrnam(const char *name) {
  struct group *res;
  if (getgrnam_r(name, &static_grp, static_grp_buffer,
                 sizeof(static_grp_buffer), &res) == 0) {
    return res;
  }
  return NULL;
}

/** \brief getgrgid_r function. */
int getgrgid_r(gid_t gid, struct group *grp, char *buffer, size_t bufsize,
               struct group **result) {
  DWORD entriesRead = 0, totalEntries = 0;
  DWORD_PTR resumeHandle = 0;
  PLOCALGROUP_INFO_0 groupInfo = NULL;
  NET_API_STATUS nStatus;
  DWORD i;
  int found = 0;
  char *found_name = NULL;
  error_type_t rc;

  if (!grp || !buffer || !result)
    return EINVAL;
  *result = NULL;

  do {
    nStatus =
        NetLocalGroupEnum(NULL, 0, (LPBYTE *)&groupInfo, MAX_PREFERRED_LENGTH,
                          &entriesRead, &totalEntries, &resumeHandle);
    if (nStatus == NERR_Success || nStatus == ERROR_MORE_DATA) {
      for (i = 0; i < entriesRead; i++) {
        char *utf8_name = NULL;
        rc = utf16_to_utf8(groupInfo[i].lgrpi0_name, &utf8_name);
        if (rc == ERR_NONE && utf8_name) {
          SID_NAME_USE peUse;
          PSID sid = NULL;
          rc = get_sid_from_name(utf8_name, &peUse, &sid);
          if (rc == ERR_NONE && sid) {
            uid_t r_uid = 0;
            rc = sid_to_rid(sid, &r_uid);
            if (rc == ERR_NONE && r_uid == gid) {
              found_name = utf8_name;
              free(sid);
              break;
            }
            free(sid);
          }
          if (!found_name)
            free(utf8_name);
        }
      }
      if (groupInfo) {
        NetApiBufferFree(groupInfo);
        groupInfo = NULL;
      }
      if (found_name) {
        found = 1;
        break;
      }
    }
  } while (nStatus == ERROR_MORE_DATA && !found);

  if (found && found_name) {
    int r = getgrnam_r(found_name, grp, buffer, bufsize, result);
    free(found_name);
    return r;
  }

  return ENOENT;
}

struct group *getgrgid(gid_t gid) {
  struct group *res;
  if (getgrgid_r(gid, &static_grp, static_grp_buffer, sizeof(static_grp_buffer),
                 &res) == 0) {
    return res;
  }
  return NULL;
}

/* --- USER DATABASE --- */

void endpwent(void) { setpwent(); }

/** \brief setpwent function. */
void setpwent(void) {
  if (current_user_info) {
    NetApiBufferFree(current_user_info);
    current_user_info = NULL;
  }
  current_resume_handle = 0;
  current_entries_read = 0;
  current_entry_index = 0;
}

struct passwd *getpwent(void) {
  DWORD total_entries = 0;
  if (current_entry_index >= current_entries_read) {
    if (current_user_info) {
      NetApiBufferFree(current_user_info);
      current_user_info = NULL;
    }
    if (NetUserEnum(NULL, 0, FILTER_NORMAL_ACCOUNT,
                    (LPBYTE *)&current_user_info, MAX_PREFERRED_LENGTH,
                    &current_entries_read, &total_entries,
                    &current_resume_handle) != NERR_Success) {
      return NULL;
    }
    current_entry_index = 0;
    if (current_entries_read == 0)
      return NULL;
  }

  {
    char *utf8_name = NULL;
    struct passwd *res = NULL;
    error_type_t rc;
    rc = utf16_to_utf8(current_user_info[current_entry_index].usri0_name,
                       &utf8_name);
    current_entry_index++;
    if (rc == ERR_NONE && utf8_name) {
      res = getpwnam(utf8_name);
      free(utf8_name);
    }
    return res;
  }
}

/** \brief getpwnam_r function. */
int getpwnam_r(const char *name, struct passwd *pwd, char *buffer,
               size_t bufsize, struct passwd **result) {
  SID_NAME_USE peUse;
  PSID sid = NULL;
  int wlen;
  WCHAR *wname;
  PUSER_INFO_4 userInfo = NULL;
  error_type_t rc;

  if (!name || !pwd || !buffer || !result)
    return EINVAL;
  *result = NULL;

  rc = get_sid_from_name(name, &peUse, &sid);
  if (rc != ERR_NONE || !sid)
    return ENOENT;

  if (peUse != SidTypeUser) {
    free(sid);
    return ENOENT;
  }

  wlen = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
  if (wlen > 0) {
    wname = (WCHAR *)malloc((size_t)wlen * sizeof(WCHAR));
    if (wname) {
      MultiByteToWideChar(CP_UTF8, 0, name, -1, wname, wlen);
      NetUserGetInfo(NULL, wname, 4, (LPBYTE *)&userInfo);
      free(wname);
    }
  }

  rc = sid_to_rid(sid, &pwd->pw_uid);
  free(sid);
  if (rc != ERR_NONE) {
    if (userInfo)
      NetApiBufferFree(userInfo);
    return EINVAL;
  }

  if (userInfo) {
    char *dir = NULL, *gecos = NULL;
    pwd->pw_gid = userInfo->usri4_primary_group_id;

    if (userInfo->usri4_home_dir) {
      rc = utf16_to_utf8(userInfo->usri4_home_dir, &dir);
      (void)rc;
    }
    if (userInfo->usri4_full_name) {
      rc = utf16_to_utf8(userInfo->usri4_full_name, &gecos);
      (void)rc;
    }

    rc = copy_string(&buffer, &bufsize, name, &pwd->pw_name);
    if (rc == ERR_NONE)
      rc = copy_string(&buffer, &bufsize, "*", &pwd->pw_passwd);
    if (rc == ERR_NONE)
      rc = copy_string(&buffer, &bufsize, dir ? dir : "C:", &pwd->pw_dir);
    if (rc == ERR_NONE)
      rc = copy_string(&buffer, &bufsize, gecos ? gecos : "", &pwd->pw_gecos);
    if (rc == ERR_NONE)
      rc = copy_string(&buffer, &bufsize, "cmd.exe", &pwd->pw_shell);

    if (dir)
      free(dir);
    if (gecos)
      free(gecos);

    NetApiBufferFree(userInfo);

    if (rc != ERR_NONE)
      return rc;
  } else {
    pwd->pw_gid = 513; /* Fallback */
    rc = copy_string(&buffer, &bufsize, name, &pwd->pw_name);
    if (rc == ERR_NONE)
      rc = copy_string(&buffer, &bufsize, "*", &pwd->pw_passwd);
    if (rc == ERR_NONE)
      rc = copy_string(&buffer, &bufsize, "C:", &pwd->pw_dir);
    if (rc == ERR_NONE)
      rc = copy_string(&buffer, &bufsize, "", &pwd->pw_gecos);
    if (rc == ERR_NONE)
      rc = copy_string(&buffer, &bufsize, "cmd.exe", &pwd->pw_shell);
    if (rc != ERR_NONE)
      return rc;
  }

  *result = pwd;
  return 0;
}

struct passwd *getpwnam(const char *name) {
  struct passwd *res;
  if (getpwnam_r(name, &static_pwd, static_pwd_buffer,
                 sizeof(static_pwd_buffer), &res) == 0) {
    return res;
  }
  return NULL;
}

/** \brief getpwuid_r function. */
int getpwuid_r(uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize,
               struct passwd **result) {
  DWORD entriesRead = 0, totalEntries = 0, resumeHandle = 0;
  PUSER_INFO_0 userInfo = NULL;
  NET_API_STATUS nStatus;
  DWORD i;
  int found = 0;
  char *found_name = NULL;
  error_type_t rc;

  if (!pwd || !buffer || !result)
    return EINVAL;
  *result = NULL;

  do {
    nStatus = NetUserEnum(NULL, 0, FILTER_NORMAL_ACCOUNT, (LPBYTE *)&userInfo,
                          MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries,
                          &resumeHandle);
    if (nStatus == NERR_Success || nStatus == ERROR_MORE_DATA) {
      for (i = 0; i < entriesRead; i++) {
        char *utf8_name = NULL;
        rc = utf16_to_utf8(userInfo[i].usri0_name, &utf8_name);
        if (rc == ERR_NONE && utf8_name) {
          SID_NAME_USE peUse;
          PSID sid = NULL;
          rc = get_sid_from_name(utf8_name, &peUse, &sid);
          if (rc == ERR_NONE && sid) {
            uid_t r_uid = 0;
            rc = sid_to_rid(sid, &r_uid);
            if (rc == ERR_NONE && r_uid == uid) {
              found_name = utf8_name;
              free(sid);
              break;
            }
            free(sid);
          }
          if (!found_name)
            free(utf8_name);
        }
      }
      if (userInfo) {
        NetApiBufferFree(userInfo);
        userInfo = NULL;
      }
      if (found_name) {
        found = 1;
        break;
      }
    }
  } while (nStatus == ERROR_MORE_DATA && !found);

  if (found && found_name) {
    int r = getpwnam_r(found_name, pwd, buffer, bufsize, result);
    free(found_name);
    return r;
  }

  return ENOENT;
}

struct passwd *getpwuid(uid_t uid) {
  struct passwd *res;
  if (getpwuid_r(uid, &static_pwd, static_pwd_buffer, sizeof(static_pwd_buffer),
                 &res) == 0) {
    return res;
  }
  return NULL;
}

#else /* _WIN32 */

/* Prevent 'empty translation unit' warning on strictly compliant compilers */
typedef int dummy_posix_pwdgrp_c_typedef;

#endif /* _WIN32 */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu_posix_pwdgrp;
