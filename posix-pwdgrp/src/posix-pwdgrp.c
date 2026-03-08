/* posix-pwdgrp.c - Strict C89 Implementation */
#ifdef _WIN32

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

#include <windef.h>
#include <winbase.h>
#include <winnls.h>
#include <sddl.h>
#include <lm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "posix-pwdgrp.h"

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

/** Helper: safely copy string to the supplied buffer, adhering to strict bounds */
static int copy_string(char **buffer, size_t *bufsize, const char *src, char **out_res) {
    size_t len = src ? strlen(src) : 0;
    char *res;
    if (len + 1 > *bufsize) return ERANGE;
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
    if (out_res) *out_res = res;
    return 0;
}

/** Helper: Obtain the RID from a SID */
static int sid_to_rid(PSID sid, uid_t *out_rid) {
    PUCHAR count;
    if (!IsValidSid(sid)) return EINVAL;
    count = GetSidSubAuthorityCount(sid);
    if (!count || *count == 0) return EINVAL;
    if (out_rid) *out_rid = (uid_t)(*GetSidSubAuthority(sid, *count - 1));
    return 0;
}

/** Helper: Look up a Windows SID by name */
static int get_sid_from_name(const char *name, SID_NAME_USE *peUse, PSID *out_sid) {
    DWORD cbSid = 0;
    DWORD cbDomain = 0;
    PSID sid = NULL;
    char *domain = NULL;

    if (!out_sid) return EINVAL;
    *out_sid = NULL;

    LookupAccountNameA(NULL, name, NULL, &cbSid, NULL, &cbDomain, peUse);
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        sid = (PSID)malloc(cbSid);
        domain = (char *)malloc(cbDomain);
        if (sid && domain) {
            if (!LookupAccountNameA(NULL, name, sid, &cbSid, domain, &cbDomain, peUse)) {
                free(sid);
                free(domain);
                return ENOENT;
            }
        } else {
            if (sid) free(sid);
            if (domain) free(domain);
            return ENOMEM;
        }
        if (domain) free(domain);
    } else {
        return ENOENT;
    }
    *out_sid = sid;
    return 0;
}

/** Helper: Convert UTF-16 to UTF-8 */
static int utf16_to_utf8(LPCWSTR wstr, char **out_str) {
    int size_needed;
    char *strTo;

    if (!out_str) return EINVAL;
    *out_str = NULL;

    size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    strTo = (char *)malloc(size_needed);
    if (strTo) {
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, strTo, size_needed, NULL, NULL);
        *out_str = strTo;
        return 0;
    }
    return ENOMEM;
}

/* --- GROUP DATABASE --- */

void endgrent(void) {
    setgrent();
}

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
        if (NetLocalGroupEnum(NULL, 0, (LPBYTE*)&current_grp_info, MAX_PREFERRED_LENGTH, &current_grp_entries_read, &total_entries, &current_grp_resume_handle) != NERR_Success) {
            return NULL;
        }
        current_grp_entry_index = 0;
        if (current_grp_entries_read == 0) return NULL;
    }
    
    {
        char *utf8_name = NULL;
        struct group *res = NULL;
        utf16_to_utf8(current_grp_info[current_grp_entry_index].lgrpi0_name, &utf8_name);
        current_grp_entry_index++;
        if (utf8_name) {
            res = getgrnam(utf8_name);
            free(utf8_name);
        }
        return res;
    }
}

/** \brief getgrnam_r function. */
int getgrnam_r(const char *name, struct group *grp, char *buffer, size_t bufsize, struct group **result) {
    SID_NAME_USE peUse;
    PSID sid;
    size_t align_offset;

    if (!name || !grp || !buffer || !result) return EINVAL;
    *result = NULL;

    get_sid_from_name(name, &peUse, &sid);
    if (!sid) return ENOENT;

    if (peUse != SidTypeAlias && peUse != SidTypeGroup && peUse != SidTypeWellKnownGroup) {
        free(sid);
        return ENOENT;
    }

    sid_to_rid(sid, &grp->gr_gid);
    free(sid);

    copy_string(&buffer, &bufsize, name, &grp->gr_name);
    copy_string(&buffer, &bufsize, "*", &grp->gr_passwd);
    
    /* Align buffer for pointer arrays */
    align_offset = ((size_t)buffer) % sizeof(char*);
    if (align_offset != 0) {
        align_offset = sizeof(char*) - align_offset;
        if (bufsize < align_offset) return ERANGE;
        buffer += align_offset;
        bufsize -= align_offset;
    }
    if (bufsize < sizeof(char*)) return ERANGE;
    
    grp->gr_mem = (char **)buffer;
    grp->gr_mem[0] = NULL;
    
    if (!grp->gr_name || !grp->gr_passwd) return ERANGE;

    *result = grp;
    return 0;
}

struct group *getgrnam(const char *name) {
    struct group *res;
    if (getgrnam_r(name, &static_grp, static_grp_buffer, sizeof(static_grp_buffer), &res) == 0) {
        return res;
    }
    return NULL;
}

/** \brief getgrgid_r function. */
int getgrgid_r(gid_t gid, struct group *grp, char *buffer, size_t bufsize, struct group **result) {
    DWORD entriesRead = 0, totalEntries = 0;
    DWORD_PTR resumeHandle = 0;
    PLOCALGROUP_INFO_0 groupInfo = NULL;
    NET_API_STATUS nStatus;
    DWORD i;
    int found = 0;
    char *found_name = NULL;

    if (!grp || !buffer || !result) return EINVAL;
    *result = NULL;

    do {
        nStatus = NetLocalGroupEnum(NULL, 0, (LPBYTE*)&groupInfo, MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries, &resumeHandle);
        if (nStatus == NERR_Success || nStatus == ERROR_MORE_DATA) {
            for (i = 0; i < entriesRead; i++) {
                char *utf8_name = NULL;
                utf16_to_utf8(groupInfo[i].lgrpi0_name, &utf8_name);
                if (utf8_name) {
                    SID_NAME_USE peUse;
                    PSID sid = NULL;
                    get_sid_from_name(utf8_name, &peUse, &sid);
                    if (sid) {
                        uid_t r_uid = 0;
                        sid_to_rid(sid, &r_uid);
                        if (r_uid == gid) {
                            found_name = utf8_name;
                            free(sid);
                            break;
                        }
                        free(sid);
                    }
                    if (!found_name) free(utf8_name);
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
    if (getgrgid_r(gid, &static_grp, static_grp_buffer, sizeof(static_grp_buffer), &res) == 0) {
        return res;
    }
    return NULL;
}

/* --- USER DATABASE --- */

void endpwent(void) {
    setpwent();
}

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
        if (NetUserEnum(NULL, 0, FILTER_NORMAL_ACCOUNT, (LPBYTE*)&current_user_info, MAX_PREFERRED_LENGTH, &current_entries_read, &total_entries, &current_resume_handle) != NERR_Success) {
            return NULL;
        }
        current_entry_index = 0;
        if (current_entries_read == 0) return NULL;
    }
    
    {
        char *utf8_name = NULL;
        struct passwd *res = NULL;
        utf16_to_utf8(current_user_info[current_entry_index].usri0_name, &utf8_name);
        current_entry_index++;
        if (utf8_name) {
            res = getpwnam(utf8_name);
            free(utf8_name);
        }
        return res;
    }
}

/** \brief getpwnam_r function. */
int getpwnam_r(const char *name, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result) {
    SID_NAME_USE peUse;
    PSID sid;
    int wlen;
    WCHAR *wname;
    PUSER_INFO_4 userInfo = NULL;

    if (!name || !pwd || !buffer || !result) return EINVAL;
    *result = NULL;

    get_sid_from_name(name, &peUse, &sid);
    if (!sid) return ENOENT;

    if (peUse != SidTypeUser) {
        free(sid);
        return ENOENT;
    }

    wlen = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
    wname = (WCHAR *)malloc(wlen * sizeof(WCHAR));
    if (wname) {
        MultiByteToWideChar(CP_UTF8, 0, name, -1, wname, wlen);
        NetUserGetInfo(NULL, wname, 4, (LPBYTE *)&userInfo);
        free(wname);
    }

    sid_to_rid(sid, &pwd->pw_uid);
    free(sid);

    if (userInfo) {
        char *dir = NULL, *gecos = NULL;
        pwd->pw_gid = userInfo->usri4_primary_group_id;
        
        if (userInfo->usri4_home_dir) utf16_to_utf8(userInfo->usri4_home_dir, &dir);
        if (userInfo->usri4_full_name) utf16_to_utf8(userInfo->usri4_full_name, &gecos);

        copy_string(&buffer, &bufsize, name, &pwd->pw_name);
        copy_string(&buffer, &bufsize, "*", &pwd->pw_passwd);
        copy_string(&buffer, &bufsize, dir ? dir : "C:\\", &pwd->pw_dir);
        copy_string(&buffer, &bufsize, gecos ? gecos : "", &pwd->pw_gecos);
        copy_string(&buffer, &bufsize, "cmd.exe", &pwd->pw_shell);

        if (dir) free(dir);
        if (gecos) free(gecos);
        
        NetApiBufferFree(userInfo);
    } else {
        pwd->pw_gid = 513; /* Fallback */
        copy_string(&buffer, &bufsize, name, &pwd->pw_name);
        copy_string(&buffer, &bufsize, "*", &pwd->pw_passwd);
        copy_string(&buffer, &bufsize, "C:\\", &pwd->pw_dir);
        copy_string(&buffer, &bufsize, "", &pwd->pw_gecos);
        copy_string(&buffer, &bufsize, "cmd.exe", &pwd->pw_shell);
    }

    if (!pwd->pw_name || !pwd->pw_passwd || !pwd->pw_dir || !pwd->pw_gecos || !pwd->pw_shell) {
        return ERANGE;
    }

    *result = pwd;
    return 0;
}

struct passwd *getpwnam(const char *name) {
    struct passwd *res;
    if (getpwnam_r(name, &static_pwd, static_pwd_buffer, sizeof(static_pwd_buffer), &res) == 0) {
        return res;
    }
    return NULL;
}

/** \brief getpwuid_r function. */
int getpwuid_r(uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result) {
    DWORD entriesRead = 0, totalEntries = 0, resumeHandle = 0;
    PUSER_INFO_0 userInfo = NULL;
    NET_API_STATUS nStatus;
    DWORD i;
    int found = 0;
    char *found_name = NULL;

    if (!pwd || !buffer || !result) return EINVAL;
    *result = NULL;

    do {
        nStatus = NetUserEnum(NULL, 0, FILTER_NORMAL_ACCOUNT, (LPBYTE*)&userInfo, MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries, &resumeHandle);
        if (nStatus == NERR_Success || nStatus == ERROR_MORE_DATA) {
            for (i = 0; i < entriesRead; i++) {
                char *utf8_name = NULL;
                utf16_to_utf8(userInfo[i].usri0_name, &utf8_name);
                if (utf8_name) {
                    SID_NAME_USE peUse;
                    PSID sid = NULL;
                    get_sid_from_name(utf8_name, &peUse, &sid);
                    if (sid) {
                        uid_t r_uid = 0;
                        sid_to_rid(sid, &r_uid);
                        if (r_uid == uid) {
                            found_name = utf8_name;
                            free(sid);
                            break;
                        }
                        free(sid);
                    }
                    if (!found_name) free(utf8_name);
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
    if (getpwuid_r(uid, &static_pwd, static_pwd_buffer, sizeof(static_pwd_buffer), &res) == 0) {
        return res;
    }
    return NULL;
}

#else /* _WIN32 */

/* Prevent 'empty translation unit' warning on strictly compliant compilers */
typedef int dummy_posix_pwdgrp_c_typedef;

#endif /* _WIN32 */


/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

typedef int make_iso_compilers_happy_tu_posix_pwdgrp;

