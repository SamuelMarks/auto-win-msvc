/* posix-time.c - Strict C89 Implementation */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "posix-time.h"

#ifdef _WIN32
#if defined(_M_IX86)
#ifndef _X86_
#define _X86_
#endif
#elif defined(_M_AMD64)
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

#if defined(_MSC_VER) && _MSC_VER < 1700
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <minwindef.h>
#include <sysinfoapi.h>
#include <timezoneapi.h>
#include <fileapi.h>
#include <handleapi.h>
#endif

#ifndef FILE_FLAG_BACKUP_SEMANTICS
#define FILE_FLAG_BACKUP_SEMANTICS 0x02000000
#endif

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)(long)-1)
#endif

#if defined(_MSC_VER)
#define POSIX_TIME_EPOCH 116444736000000000ui64
#define POSIX_TIME_10M   10000000ui64
#define POSIX_TIME_10    10ui64
#elif defined(__GNUC__)
#define POSIX_TIME_EPOCH __extension__ 116444736000000000ULL
#define POSIX_TIME_10M   __extension__ 10000000ULL
#define POSIX_TIME_10    __extension__ 10ULL
#else
#define POSIX_TIME_EPOCH 116444736000000000ULL
#define POSIX_TIME_10M   10000000ULL
#define POSIX_TIME_10    10ULL
#endif

/* Polyfill for gettimeofday using GetSystemTimeAsFileTime */
int gettimeofday(struct timeval *tv, struct timezone *tz) {
    if (tv) {
        FILETIME ft;
        ULARGE_INTEGER uli;
        GetSystemTimeAsFileTime(&ft);
        uli.LowPart = ft.dwLowDateTime;
        uli.HighPart = ft.dwHighDateTime;
        /* Convert from 100-nanosecond intervals since 1601 to microseconds since 1970 */
        uli.QuadPart -= POSIX_TIME_EPOCH;
        tv->tv_sec = (long)(uli.QuadPart / POSIX_TIME_10M);
        tv->tv_usec = (long)((uli.QuadPart % POSIX_TIME_10M) / POSIX_TIME_10);
    }

    if (tz) {
        TIME_ZONE_INFORMATION tzi;
        DWORD res;
        res = GetTimeZoneInformation(&tzi);
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
        uli.QuadPart = (ULONGLONG)times[0].tv_sec * POSIX_TIME_10M + (ULONGLONG)times[0].tv_usec * POSIX_TIME_10 + POSIX_TIME_EPOCH;
        ftAccess.dwLowDateTime = uli.LowPart;
        ftAccess.dwHighDateTime = uli.HighPart;
        pAccess = &ftAccess;

        uli.QuadPart = (ULONGLONG)times[1].tv_sec * POSIX_TIME_10M + (ULONGLONG)times[1].tv_usec * POSIX_TIME_10 + POSIX_TIME_EPOCH;
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