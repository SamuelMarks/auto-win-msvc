#include "bsd-sys-file.h"

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <io.h>
#include <errno.h>

#ifndef EWOULDBLOCK
#define EWOULDBLOCK EAGAIN
#endif

/** \brief posix_flock function. */
int posix_flock(int fd, int operation) {
    HANDLE hFile;
    OVERLAPPED overlapped;
    DWORD flags = 0;
    BOOL result;

    if (fd < 0) {
        errno = EBADF;
        return -1;
    }

    hFile = (HANDLE)_get_osfhandle(fd);
    if (hFile == INVALID_HANDLE_VALUE) {
        errno = EBADF;
        return -1;
    }

    /* Initialize overlapped structure for byte 0 locking */
    memset(&overlapped, 0, sizeof(overlapped));

    if (operation & LOCK_UN) {
        /* Unlock */
        result = UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &overlapped);
        if (!result) {
            /* If it was already unlocked, this might fail, but POSIX flock ignores unlocking unlocked files. */
            DWORD err = GetLastError();
            if (err != ERROR_NOT_LOCKED) {
                errno = EACCES;
                return -1;
            }
        }
        return 0;
    }

    /* Lock */
    if (operation & LOCK_NB) {
        flags |= LOCKFILE_FAIL_IMMEDIATELY;
    }

    if (operation & LOCK_EX) {
        flags |= LOCKFILE_EXCLUSIVE_LOCK;
    } else if (operation & LOCK_SH) {
        /* Shared lock is default when exclusive is not specified */
    } else {
        errno = EINVAL;
        return -1;
    }

    result = LockFileEx(hFile, flags, 0, MAXDWORD, MAXDWORD, &overlapped);
    if (!result) {
        DWORD err = GetLastError();
        if (err == ERROR_LOCK_VIOLATION || err == ERROR_SHARING_VIOLATION) {
            errno = EWOULDBLOCK;
        } else {
            errno = EACCES;
        }
        return -1;
    }

    return 0;
}

#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_bsd_sys_file(void) { return 0; }

typedef int make_iso_compilers_happy_tu_bsd_sys_file;

