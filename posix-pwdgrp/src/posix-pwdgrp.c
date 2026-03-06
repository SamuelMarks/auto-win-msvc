/* posix-pwdgrp.c - Strict C89 Implementation */
#ifdef _WIN32

/* Provide C89 compliant CRT secure overload definitions */
#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif

#include <windows.h>
#include <sddl.h>
#include <lm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "posix-pwdgrp.h"

/* Link required Windows libraries */
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "advapi32.lib")

#define PWD_BUFFER_SIZE 4096

static __declspec(thread) struct passwd static_pwd;
static __declspec(thread) char static_pwd_buffer[PWD_BUFFER_SIZE];

static __declspec(thread) struct group static_grp;
static __declspec(thread) char static_grp_buffer[PWD_BUFFER_SIZE];

static __declspec(thread) DWORD current_resume_handle = 0;
static __declspec(thread) PUSER_INFO_0 current_user_info = NULL;
static __declspec(thread) DWORD current_entries_read = 0;
static __declspec(thread) DWORD current_entry_index = 0;

static __declspec(thread) DWORD current_grp_resume_handle = 0;
static __declspec(thread) PLOCALGROUP_INFO_0 current_grp_info = NULL;
static __declspec(thread) DWORD current_grp_entries_read = 0;
static __declspec(thread) DWORD current_grp_entry_index = 0;

/** Helper: safely copy string to the supplied buffer, adhering to strict bounds */
static char *copy_string(char **buffer, size_t *bufsize, const char *src) {
    size_t len = src ? strlen(src) : 0;
    char *res;
    if (len + 1 > *bufsize) return NULL;
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
    return res;
}

/** Helper: Obtain the RID from a SID */
static uid_t sid_to_rid(PSID sid) {
    PUCHAR count;
    if (!IsValidSid(sid)) return 0;
    count = GetSidSubAuthorityCount(sid);
    if (!count || *count == 0) return 0;
    return (uid_t)(*GetSidSubAuthority(sid, *count - 1));
}

/** Helper: Look up a Windows SID by name */
static PSID get_sid_from_name(const char *name, SID_NAME_USE *peUse) {
    DWORD cbSid = 0;
    DWORD cbDomain = 0;
    PSID sid = NULL;
    char *domain = NULL;

    LookupAccountNameA(NULL, name, NULL, &cbSid, NULL, &cbDomain, peUse);
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        sid = (PSID)malloc(cbSid);
        domain = (char *)malloc(cbDomain);
        if (sid && domain) {
            if (!LookupAccountNameA(NULL, name, sid, &cbSid, domain, &cbDomain, peUse)) {
                free(sid);
                sid = NULL;
            }
        } else {
            if (sid) free(sid);
        }
        if (domain) free(domain);
    }
    return sid;
}

/** Helper: Convert UTF-16 to UTF-8 */
static char *utf16_to_utf8(LPCWSTR wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char *strTo = (char *)malloc(size_needed);
    if (strTo) {
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, strTo, size_needed, NULL, NULL);
    }
    return strTo;
}

/* --- GROUP DATABASE --- */

void endgrent(void) {
    setgrent();
}

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
    if (current_grp_entry_index >= current_grp_entries_read) {
        if (current_grp_info) {
            NetApiBufferFree(current_grp_info);
            current_grp_info = NULL;
        }
        if (NetLocalGroupEnum(NULL, 0, (LPBYTE*)&current_grp_info, MAX_PREFERRED_LENGTH, &current_grp_entries_read, NULL, &current_grp_resume_handle) != NERR_Success) {
            return NULL;
        }
        current_grp_entry_index = 0;
        if (current_grp_entries_read == 0) return NULL;
    }
    
    {
        char *utf8_name = utf16_to_utf8(current_grp_info[current_grp_entry_index].lgrpi0_name);
        struct group *res = NULL;
        current_grp_entry_index++;
        if (utf8_name) {
            res = getgrnam(utf8_name);
            free(utf8_name);
        }
        return res;
    }
}

int getgrnam_r(const char *name, struct group *grp, char *buffer, size_t bufsize, struct group **result) {
    SID_NAME_USE peUse;
    PSID sid;
    size_t align_offset;

    if (!name || !grp || !buffer || !result) return EINVAL;
    *result = NULL;

    sid = get_sid_from_name(name, &peUse);
    if (!sid) return ENOENT;

    if (peUse != SidTypeAlias && peUse != SidTypeGroup && peUse != SidTypeWellKnownGroup) {
        free(sid);
        return ENOENT;
    }

    grp->gr_gid = sid_to_rid(sid);
    free(sid);

    grp->gr_name = copy_string(&buffer, &bufsize, name);
    grp->gr_passwd = copy_string(&buffer, &bufsize, "*");
    
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

int getgrgid_r(gid_t gid, struct group *grp, char *buffer, size_t bufsize, struct group **result) {
    DWORD entriesRead = 0, totalEntries = 0, resumeHandle = 0;
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
                char *utf8_name = utf16_to_utf8(groupInfo[i].lgrpi0_name);
                if (utf8_name) {
                    SID_NAME_USE peUse;
                    PSID sid = get_sid_from_name(utf8_name, &peUse);
                    if (sid) {
                        if (sid_to_rid(sid) == gid) {
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
    if (current_entry_index >= current_entries_read) {
        if (current_user_info) {
            NetApiBufferFree(current_user_info);
            current_user_info = NULL;
        }
        if (NetUserEnum(NULL, 0, FILTER_NORMAL_ACCOUNT, (LPBYTE*)&current_user_info, MAX_PREFERRED_LENGTH, &current_entries_read, NULL, &current_resume_handle) != NERR_Success) {
            return NULL;
        }
        current_entry_index = 0;
        if (current_entries_read == 0) return NULL;
    }
    
    {
        char *utf8_name = utf16_to_utf8(current_user_info[current_entry_index].usri0_name);
        struct passwd *res = NULL;
        current_entry_index++;
        if (utf8_name) {
            res = getpwnam(utf8_name);
            free(utf8_name);
        }
        return res;
    }
}

int getpwnam_r(const char *name, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result) {
    SID_NAME_USE peUse;
    PSID sid;
    int wlen;
    WCHAR *wname;
    PUSER_INFO_4 userInfo = NULL;
    NET_API_STATUS nStatus;

    if (!name || !pwd || !buffer || !result) return EINVAL;
    *result = NULL;

    sid = get_sid_from_name(name, &peUse);
    if (!sid) return ENOENT;

    if (peUse != SidTypeUser) {
        free(sid);
        return ENOENT;
    }

    wlen = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
    wname = (WCHAR *)malloc(wlen * sizeof(WCHAR));
    if (wname) {
        MultiByteToWideChar(CP_UTF8, 0, name, -1, wname, wlen);
        nStatus = NetUserGetInfo(NULL, wname, 4, (LPBYTE *)&userInfo);
        free(wname);
    }

    pwd->pw_uid = sid_to_rid(sid);
    free(sid);

    if (userInfo) {
        char *dir = NULL, *gecos = NULL;
        pwd->pw_gid = userInfo->usri4_primary_group_id;
        
        if (userInfo->usri4_home_dir) dir = utf16_to_utf8(userInfo->usri4_home_dir);
        if (userInfo->usri4_full_name) gecos = utf16_to_utf8(userInfo->usri4_full_name);

        pwd->pw_name = copy_string(&buffer, &bufsize, name);
        pwd->pw_passwd = copy_string(&buffer, &bufsize, "*");
        pwd->pw_dir = copy_string(&buffer, &bufsize, dir ? dir : "C:\\");
        pwd->pw_gecos = copy_string(&buffer, &bufsize, gecos ? gecos : "");
        pwd->pw_shell = copy_string(&buffer, &bufsize, "cmd.exe");

        if (dir) free(dir);
        if (gecos) free(gecos);
        
        NetApiBufferFree(userInfo);
    } else {
        pwd->pw_gid = 513; /* Fallback */
        pwd->pw_name = copy_string(&buffer, &bufsize, name);
        pwd->pw_passwd = copy_string(&buffer, &bufsize, "*");
        pwd->pw_dir = copy_string(&buffer, &bufsize, "C:\\");
        pwd->pw_gecos = copy_string(&buffer, &bufsize, "");
        pwd->pw_shell = copy_string(&buffer, &bufsize, "cmd.exe");
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
                char *utf8_name = utf16_to_utf8(userInfo[i].usri0_name);
                if (utf8_name) {
                    SID_NAME_USE peUse;
                    PSID sid = get_sid_from_name(utf8_name, &peUse);
                    if (sid) {
                        if (sid_to_rid(sid) == uid) {
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

