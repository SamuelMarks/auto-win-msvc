/* posix-stat.c - Strict C89 Implementation */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <io.h>
#include <direct.h>
#include <windows.h>
#include "posix-stat.h"

int fchmod(int fd, mode_t mode) {
    HANDLE hFile;
    HMODULE hKernel32;
    DWORD attrs;
    char path[MAX_PATH];
    typedef DWORD (WINAPI *GetFinalPathNameByHandleA_t)(HANDLE, LPSTR, DWORD, DWORD);
    GetFinalPathNameByHandleA_t pGetFinalPathName;

    hFile = (HANDLE)_get_osfhandle(fd);
    if (hFile == INVALID_HANDLE_VALUE) {
        errno = EBADF;
        return -1;
    }

    hKernel32 = GetModuleHandleA("kernel32.dll");
    if (!hKernel32) {
        errno = ENOSYS;
        return -1;
    }

    pGetFinalPathName = (GetFinalPathNameByHandleA_t)GetProcAddress(hKernel32, "GetFinalPathNameByHandleA");
    if (!pGetFinalPathName) {
        errno = ENOSYS;
        return -1;
    }

    if (pGetFinalPathName(hFile, path, MAX_PATH, 0) == 0) {
        errno = EACCES;
        return -1;
    }

    attrs = GetFileAttributesA(path);
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        errno = ENOENT;
        return -1;
    }

    if (!(mode & _S_IWRITE)) {
        attrs |= FILE_ATTRIBUTE_READONLY;
    } else {
        attrs &= ~FILE_ATTRIBUTE_READONLY;
    }

    if (!SetFileAttributesA(path, attrs)) {
        errno = EACCES;
        return -1;
    }

    return 0;
}

int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags) {
    if (dirfd != AT_FDCWD && dirfd != -1) {
        errno = ENOSYS;
        return -1;
    }
    if (flags & AT_SYMLINK_NOFOLLOW) {
        /* _chmod doesn't natively follow symlinks in older Windows, 
           but we do the best we can here. */
    }
    return _chmod(pathname, mode);
}

int fstatat(int dirfd, const char *pathname, struct _stat64 *statbuf, int flags) {
    if (dirfd != AT_FDCWD && dirfd != -1) {
        errno = ENOSYS;
        return -1;
    }
    if (flags & AT_SYMLINK_NOFOLLOW) {
        return lstat(pathname, statbuf);
    }
    return _stat64(pathname, statbuf);
}

static void fill_filetime(const struct timespec *ts, FILETIME *ft, int *omit) {
    if (ts == NULL) {
        SYSTEMTIME st; GetSystemTime(&st); SystemTimeToFileTime(&st, ft);
        *omit = 0;
    } else if (ts->tv_nsec == UTIME_OMIT) {
        *omit = 1;
    } else if (ts->tv_nsec == UTIME_NOW) {
        SYSTEMTIME st; GetSystemTime(&st); SystemTimeToFileTime(&st, ft);
        *omit = 0;
    } else {
        unsigned long long t = ((unsigned long long)ts->tv_sec * 10000000ULL) + 116444736000000000ULL;
        ft->dwLowDateTime = (DWORD)(t & 0xFFFFFFFF);
        ft->dwHighDateTime = (DWORD)(t >> 32);
        *omit = 0;
    }
}

int futimens(int fd, const struct timespec times[2]) {
    HANDLE hFile = (HANDLE)_get_osfhandle(fd);
    FILETIME atime, mtime;
    FILETIME *pAtime = NULL, *pMtime = NULL;
    int omit_a = 0, omit_m = 0;

    if (hFile == INVALID_HANDLE_VALUE) {
        errno = EBADF;
        return -1;
    }

    if (times != NULL) {
        fill_filetime(&times[0], &atime, &omit_a);
        if (!omit_a) pAtime = &atime;
        fill_filetime(&times[1], &mtime, &omit_m);
        if (!omit_m) pMtime = &mtime;
    } else {
        SYSTEMTIME st; GetSystemTime(&st); SystemTimeToFileTime(&st, &atime);
        pAtime = &atime; pMtime = &atime;
    }

    if (!SetFileTime(hFile, NULL, pAtime, pMtime)) {
        errno = EACCES;
        return -1;
    }
    return 0;
}

int lstat(const char *pathname, struct _stat64 *statbuf) {
    WIN32_FILE_ATTRIBUTE_DATA info;
    unsigned long long t;
    if (!GetFileAttributesExA(pathname, GetFileExInfoStandard, &info)) {
        errno = ENOENT;
        return -1;
    }
    if (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        statbuf->st_mode = S_IFLNK | S_IRWXU | S_IRWXG | S_IRWXO;
        statbuf->st_size = 0;
        
        t = ((unsigned long long)info.ftLastAccessTime.dwHighDateTime << 32) | info.ftLastAccessTime.dwLowDateTime;
        statbuf->st_atime = (time_t)((t - 116444736000000000ULL) / 10000000ULL);
        
        t = ((unsigned long long)info.ftLastWriteTime.dwHighDateTime << 32) | info.ftLastWriteTime.dwLowDateTime;
        statbuf->st_mtime = (time_t)((t - 116444736000000000ULL) / 10000000ULL);
        
        t = ((unsigned long long)info.ftCreationTime.dwHighDateTime << 32) | info.ftCreationTime.dwLowDateTime;
        statbuf->st_ctime = (time_t)((t - 116444736000000000ULL) / 10000000ULL);

        statbuf->st_dev = 0;
        statbuf->st_ino = 0;
        statbuf->st_nlink = 1;
        statbuf->st_uid = 0;
        statbuf->st_gid = 0;
        statbuf->st_rdev = 0;
        return 0;
    }
    return _stat64(pathname, statbuf);
}

int mknod(const char *pathname, mode_t mode, unsigned int dev) {
    HANDLE hFile;
    (void)dev;
    if (S_ISDIR(mode)) {
        return _mkdir(pathname);
    }
    if (S_ISCHR(mode) || S_ISBLK(mode) || S_ISFIFO(mode) || S_ISSOCK(mode)) {
        errno = ENOSYS;
        return -1;
    }
    hFile = CreateFileA(pathname, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_EXISTS) {
            errno = EEXIST;
        } else {
            errno = EINVAL;
        }
        return -1;
    }
    CloseHandle(hFile);
    return 0;
}

int mknodat(int dirfd, const char *pathname, mode_t mode, unsigned int dev) {
    if (dirfd != AT_FDCWD && dirfd != -1) {
        errno = ENOSYS;
        return -1;
    }
    return mknod(pathname, mode, dev);
}

int utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
    HANDLE hFile;
    DWORD attrs = FILE_FLAG_BACKUP_SEMANTICS;
    FILETIME atime, mtime;
    FILETIME *pAtime = NULL, *pMtime = NULL;
    int omit_a = 0, omit_m = 0;

    if (dirfd != AT_FDCWD && dirfd != -1) {
        errno = ENOSYS;
        return -1;
    }

    if (flags & AT_SYMLINK_NOFOLLOW) {
        attrs |= 0x00200000; /* FILE_FLAG_OPEN_REPARSE_POINT */
    }

    hFile = CreateFileA(pathname, FILE_WRITE_ATTRIBUTES,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        NULL, OPEN_EXISTING, attrs, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        errno = ENOENT;
        return -1;
    }

    if (times != NULL) {
        fill_filetime(&times[0], &atime, &omit_a);
        if (!omit_a) pAtime = &atime;
        fill_filetime(&times[1], &mtime, &omit_m);
        if (!omit_m) pMtime = &mtime;
    } else {
        SYSTEMTIME st; GetSystemTime(&st); SystemTimeToFileTime(&st, &atime);
        pAtime = &atime; pMtime = &atime;
    }

    if (!SetFileTime(hFile, NULL, pAtime, pMtime)) {
        CloseHandle(hFile);
        errno = EACCES;
        return -1;
    }

    CloseHandle(hFile);
    return 0;
}