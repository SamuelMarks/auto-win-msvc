/**
 * @file posix-utsname.c
 * @brief Implementation of POSIX uname() for MSVC.
 */

#include "posix-utsname.h"
#include <errno.h>

#ifndef EFAULT
#define EFAULT 14
#endif

#ifndef ENOSYS
#define ENOSYS 38
#endif

#if defined(_WIN32) || defined(_WIN64)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <stdio.h>
#include <string.h>

/* Use safe CRT functions if available */
#if defined(_MSC_VER) && _MSC_VER >= 1400
#define USE_SAFE_CRT
#endif

#ifndef PROCESSOR_ARCHITECTURE_ARM
#define PROCESSOR_ARCHITECTURE_ARM 5
#endif

#ifndef PROCESSOR_ARCHITECTURE_ARM64
#define PROCESSOR_ARCHITECTURE_ARM64 12
#endif

typedef struct _RTL_OSVERSIONINFOW {
    ULONG dwOSVersionInfoSize;
    ULONG dwMajorVersion;
    ULONG dwMinorVersion;
    ULONG dwBuildNumber;
    ULONG dwPlatformId;
    WCHAR szCSDVersion[128];
} RTL_OSVERSIONINFOW;

typedef LONG (WINAPI *RtlGetVersion_Func)(RTL_OSVERSIONINFOW*);

/**
 * @brief Get system identification.
 *
 * Populates the provided utsname structure with system information.
 * On Windows, it uses GetComputerNameA, RtlGetVersion/GetVersionExA, and GetSystemInfo.
 *
 * @param name Pointer to a utsname structure to be filled.
 * @return 0 on success, or -1 on error (with errno set appropriately).
 */
int uname(struct utsname *name) {
    OSVERSIONINFOA osvi;
    SYSTEM_INFO si;
    DWORD nodename_len = _UTSNAME_LENGTH;
    HMODULE hModNtdll;
    RtlGetVersion_Func pRtlGetVersion;
    RTL_OSVERSIONINFOW rosvi;

    if (!name) {
        errno = EFAULT;
        return -1;
    }

    memset(name, 0, sizeof(struct utsname));

#ifdef USE_SAFE_CRT
    strncpy_s(name->sysname, _UTSNAME_LENGTH, "Windows_NT", _TRUNCATE);
#else
    strncpy(name->sysname, "Windows_NT", _UTSNAME_LENGTH - 1);
    name->sysname[_UTSNAME_LENGTH - 1] = '\0';
#endif

    if (!GetComputerNameA(name->nodename, &nodename_len)) {
#ifdef USE_SAFE_CRT
        strncpy_s(name->nodename, _UTSNAME_LENGTH, "unknown", _TRUNCATE);
#else
        strncpy(name->nodename, "unknown", _UTSNAME_LENGTH - 1);
        name->nodename[_UTSNAME_LENGTH - 1] = '\0';
#endif
    }

    memset(&osvi, 0, sizeof(OSVERSIONINFOA));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

    hModNtdll = GetModuleHandleA("ntdll.dll");
    pRtlGetVersion = NULL;
    if (hModNtdll) {
        pRtlGetVersion = (RtlGetVersion_Func)(void*)GetProcAddress(hModNtdll, "RtlGetVersion");
    }

    if (pRtlGetVersion) {
        memset(&rosvi, 0, sizeof(RTL_OSVERSIONINFOW));
        rosvi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
        if (pRtlGetVersion(&rosvi) == 0) { /* STATUS_SUCCESS */
            osvi.dwMajorVersion = rosvi.dwMajorVersion;
            osvi.dwMinorVersion = rosvi.dwMinorVersion;
            osvi.dwBuildNumber = rosvi.dwBuildNumber;
        } else {
            osvi.dwMajorVersion = 0;
        }
    }

    if (osvi.dwMajorVersion == 0) {
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996) /* GetVersionEx is deprecated */
#endif
        GetVersionExA(&osvi);
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
    }

#ifdef USE_SAFE_CRT
    sprintf_s(name->release, _UTSNAME_LENGTH, "%lu.%lu",
              (unsigned long)osvi.dwMajorVersion,
              (unsigned long)osvi.dwMinorVersion);
    sprintf_s(name->version, _UTSNAME_LENGTH, "%lu",
              (unsigned long)osvi.dwBuildNumber);
#else
    sprintf(name->release, "%lu.%lu",
            (unsigned long)osvi.dwMajorVersion,
            (unsigned long)osvi.dwMinorVersion);
    sprintf(name->version, "%lu",
            (unsigned long)osvi.dwBuildNumber);
#endif

    GetSystemInfo(&si);
    switch (si.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_INTEL:
#ifdef USE_SAFE_CRT
            strncpy_s(name->machine, _UTSNAME_LENGTH, "i686", _TRUNCATE);
#else
            strncpy(name->machine, "i686", _UTSNAME_LENGTH - 1);
            name->machine[_UTSNAME_LENGTH - 1] = '\0';
#endif
            break;
        case PROCESSOR_ARCHITECTURE_AMD64:
#ifdef USE_SAFE_CRT
            strncpy_s(name->machine, _UTSNAME_LENGTH, "x86_64", _TRUNCATE);
#else
            strncpy(name->machine, "x86_64", _UTSNAME_LENGTH - 1);
            name->machine[_UTSNAME_LENGTH - 1] = '\0';
#endif
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
#ifdef USE_SAFE_CRT
            strncpy_s(name->machine, _UTSNAME_LENGTH, "arm", _TRUNCATE);
#else
            strncpy(name->machine, "arm", _UTSNAME_LENGTH - 1);
            name->machine[_UTSNAME_LENGTH - 1] = '\0';
#endif
            break;
        case PROCESSOR_ARCHITECTURE_ARM64:
#ifdef USE_SAFE_CRT
            strncpy_s(name->machine, _UTSNAME_LENGTH, "aarch64", _TRUNCATE);
#else
            strncpy(name->machine, "aarch64", _UTSNAME_LENGTH - 1);
            name->machine[_UTSNAME_LENGTH - 1] = '\0';
#endif
            break;
        case PROCESSOR_ARCHITECTURE_IA64:
#ifdef USE_SAFE_CRT
            strncpy_s(name->machine, _UTSNAME_LENGTH, "ia64", _TRUNCATE);
#else
            strncpy(name->machine, "ia64", _UTSNAME_LENGTH - 1);
            name->machine[_UTSNAME_LENGTH - 1] = '\0';
#endif
            break;
        default:
#ifdef USE_SAFE_CRT
            strncpy_s(name->machine, _UTSNAME_LENGTH, "unknown", _TRUNCATE);
#else
            strncpy(name->machine, "unknown", _UTSNAME_LENGTH - 1);
            name->machine[_UTSNAME_LENGTH - 1] = '\0';
#endif
            break;
    }

    return 0;
}

#else

/**
 * @brief Get system identification (stub for non-Windows).
 *
 * @param name Pointer to a utsname structure to be filled.
 * @return -1 and sets errno to ENOSYS.
 */
int uname(struct utsname *name) {
    if (!name) {
        errno = EFAULT;
        return -1;
    }
    errno = ENOSYS;
    return -1;
}

#endif
