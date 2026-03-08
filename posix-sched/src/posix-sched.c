#include "posix-sched.h"

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <errno.h>

int posix_sched_yield(void) {
    Sleep(0);
    return 0;
}

int posix_sched_setaffinity(int pid, size_t cpusetsize, const cpu_set_t *mask) {
    HANDLE hProcess;
    BOOL res;

    if (!mask || cpusetsize < sizeof(cpu_set_t)) {
        errno = EINVAL;
        return -1;
    }

    if (pid == 0) {
        hProcess = GetCurrentProcess();
    } else {
        hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, (DWORD)pid);
        if (!hProcess) {
            errno = ESRCH;
            return -1;
        }
    }

    res = SetProcessAffinityMask(hProcess, (DWORD_PTR)mask->bits);
    if (pid != 0) {
        CloseHandle(hProcess);
    }

    if (!res) {
        errno = EINVAL;
        return -1;
    }

    return 0;
}

int posix_sched_getaffinity(int pid, size_t cpusetsize, cpu_set_t *mask) {
    HANDLE hProcess;
    BOOL res;
    DWORD_PTR processMask = 0;
    DWORD_PTR systemMask = 0;

    if (!mask || cpusetsize < sizeof(cpu_set_t)) {
        errno = EINVAL;
        return -1;
    }

    if (pid == 0) {
        hProcess = GetCurrentProcess();
    } else {
#ifdef PROCESS_QUERY_LIMITED_INFORMATION
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD)pid);
#else
        hProcess = NULL;
#endif
        if (!hProcess) {
            hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, (DWORD)pid);
        }
        if (!hProcess) {
            errno = ESRCH;
            return -1;
        }
    }

    res = GetProcessAffinityMask(hProcess, &processMask, &systemMask);
    if (pid != 0) {
        CloseHandle(hProcess);
    }

    if (!res) {
        errno = EINVAL;
        return -1;
    }

    mask->bits = (mask_bit_type)processMask;
    return 0;
}

#endif

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_sched(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_sched;

