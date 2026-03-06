/* posix-time.c - Strict C89 Implementation */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "posix-time.h"

#ifdef _WIN32
#include <windows.h>

/* Polyfill for gettimeofday using GetSystemTimeAsFileTime */
int gettimeofday(struct timeval *tv, struct timezone *tz) {
    if (tv) {
        FILETIME ft;
        ULARGE_INTEGER uli;
        GetSystemTimeAsFileTime(&ft);
        uli.LowPart = ft.dwLowDateTime;
        uli.HighPart = ft.dwHighDateTime;
        /* Convert from 100-nanosecond intervals since 1601 to microseconds since 1970 */
        uli.QuadPart -= 116444736000000000ui64;
        tv->tv_sec = (long)(uli.QuadPart / 10000000ui64);
        tv->tv_usec = (long)((uli.QuadPart % 10000000ui64) / 10ui64);
    }
    
    if (tz) {
        TIME_ZONE_INFORMATION tzi;
        DWORD res = GetTimeZoneInformation(&tzi);
        tz->tz_minuteswest = tzi.Bias;
        tz->tz_dsttime = (res == TIME_ZONE_ID_DAYLIGHT) ? 1 : 0;
    }
    
    return 0;
}

/* Polyfill for utimes using SetFileTime */
int utimes(const char *filename, const struct timeval times[2]) {
    HANDLE hFile;
    FILETIME ftAccess, ftWrite;
    FILETIME *pAccess = NULL, *pWrite = NULL;
    ULARGE_INTEGER uli;

    if (!filename) {
        errno = EINVAL;
        return -1;
    }

    hFile = CreateFileA(filename, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        errno = ENOENT;
        return -1;
    }

    if (times) {
        uli.QuadPart = (ULONGLONG)times[0].tv_sec * 10000000ui64 + (ULONGLONG)times[0].tv_usec * 10ui64 + 116444736000000000ui64;
        ftAccess.dwLowDateTime = uli.LowPart;
        ftAccess.dwHighDateTime = uli.HighPart;
        pAccess = &ftAccess;

        uli.QuadPart = (ULONGLONG)times[1].tv_sec * 10000000ui64 + (ULONGLONG)times[1].tv_usec * 10ui64 + 116444736000000000ui64;
        ftWrite.dwLowDateTime = uli.LowPart;
        ftWrite.dwHighDateTime = uli.HighPart;
        pWrite = &ftWrite;
    } else {
        GetSystemTimeAsFileTime(&ftAccess);
        ftWrite = ftAccess;
        pAccess = &ftAccess;
        pWrite = &ftWrite;
    }

    if (!SetFileTime(hFile, NULL, pAccess, pWrite)) {
        CloseHandle(hFile);
        errno = EACCES;
        return -1;
    }

    CloseHandle(hFile);
    return 0;
}

/* Minimal stubs for getitimer and setitimer since MSVC has no SIGALRM */
static struct itimerval g_timers[3] = {{{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}};

int getitimer(int which, struct itimerval *value) {
    if (which < ITIMER_REAL || which > ITIMER_PROF || !value) {
        errno = EINVAL;
        return -1;
    }
    *value = g_timers[which];
    return 0;
}

int setitimer(int which, const struct itimerval *value, struct itimerval *ovalue) {
    if (which < ITIMER_REAL || which > ITIMER_PROF || !value) {
        errno = EINVAL;
        return -1;
    }
    if (ovalue) {
        *ovalue = g_timers[which];
    }
    g_timers[which] = *value;
    return 0;
}

#endif /* _WIN32 */
