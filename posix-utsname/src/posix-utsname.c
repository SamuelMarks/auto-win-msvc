/**
 * @file posix-utsname.c
 * @brief Implementation of POSIX uname() for MSVC.
 */

#include "posix-utsname.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifndef EFAULT
#define EFAULT 14
#endif

#ifndef ENOSYS
#define ENOSYS 38
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)

/* Use safe CRT functions if available */
#if defined(_MSC_VER) && _MSC_VER >= 1400
#define USE_SAFE_CRT
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#define UTS_WINAPI __stdcall
#define UTS_DECLSPEC_IMPORT __declspec(dllimport)
#else
#define UTS_WINAPI __attribute__((stdcall))
#define UTS_DECLSPEC_IMPORT __attribute__((dllimport))
#endif

typedef unsigned long UTS_DWORD;
typedef unsigned short UTS_WORD;
typedef int UTS_BOOL;
typedef void *UTS_HANDLE;
typedef UTS_HANDLE UTS_HMODULE;
typedef const char *UTS_LPCSTR;
typedef char *UTS_LPSTR;
typedef UTS_DWORD *UTS_LPDWORD;
typedef long UTS_LONG;
typedef unsigned short UTS_WCHAR;
typedef unsigned long UTS_ULONG;

typedef struct _UTS_OSVERSIONINFOA {
    UTS_DWORD dwOSVersionInfoSize;
    UTS_DWORD dwMajorVersion;
    UTS_DWORD dwMinorVersion;
    UTS_DWORD dwBuildNumber;
    UTS_DWORD dwPlatformId;
    char      szCSDVersion[128];
} UTS_OSVERSIONINFOA;

typedef struct _UTS_SYSTEM_INFO {
    union {
        UTS_DWORD dwOemId;
        struct {
            UTS_WORD wProcessorArchitecture;
            UTS_WORD wReserved;
        } s;
    } u;
    UTS_DWORD dwPageSize;
    void      *lpMinimumApplicationAddress;
    void      *lpMaximumApplicationAddress;
    UTS_DWORD *dwActiveProcessorMask;
    UTS_DWORD dwNumberOfProcessors;
    UTS_DWORD dwProcessorType;
    UTS_DWORD dwAllocationGranularity;
    UTS_WORD  wProcessorLevel;
    UTS_WORD  wProcessorRevision;
} UTS_SYSTEM_INFO;

typedef struct _UTS_RTL_OSVERSIONINFOW {
    UTS_ULONG dwOSVersionInfoSize;
    UTS_ULONG dwMajorVersion;
    UTS_ULONG dwMinorVersion;
    UTS_ULONG dwBuildNumber;
    UTS_ULONG dwPlatformId;
    UTS_WCHAR szCSDVersion[128];
} UTS_RTL_OSVERSIONINFOW;

typedef UTS_LONG (UTS_WINAPI *RtlGetVersion_Func)(UTS_RTL_OSVERSIONINFOW*);
typedef UTS_LONG (UTS_WINAPI *UTS_FARPROC)(void);

UTS_DECLSPEC_IMPORT UTS_BOOL UTS_WINAPI GetComputerNameA(UTS_LPSTR lpBuffer, UTS_LPDWORD nSize);
UTS_DECLSPEC_IMPORT UTS_HMODULE UTS_WINAPI GetModuleHandleA(UTS_LPCSTR lpModuleName);
UTS_DECLSPEC_IMPORT UTS_FARPROC UTS_WINAPI GetProcAddress(UTS_HMODULE hModule, UTS_LPCSTR lpProcName);
UTS_DECLSPEC_IMPORT void UTS_WINAPI GetSystemInfo(UTS_SYSTEM_INFO* lpSystemInfo);
UTS_DECLSPEC_IMPORT UTS_BOOL UTS_WINAPI GetVersionExA(UTS_OSVERSIONINFOA* lpVersionInformation);

#define UTS_PROCESSOR_ARCHITECTURE_INTEL 0
#define UTS_PROCESSOR_ARCHITECTURE_ARM 5
#define UTS_PROCESSOR_ARCHITECTURE_IA64 6
#define UTS_PROCESSOR_ARCHITECTURE_AMD64 9
#define UTS_PROCESSOR_ARCHITECTURE_ARM64 12

#ifdef __cplusplus
}
#endif

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
    UTS_OSVERSIONINFOA osvi;
    UTS_SYSTEM_INFO si;
    UTS_DWORD nodename_len = _UTSNAME_LENGTH;
    UTS_HMODULE hModNtdll;
    RtlGetVersion_Func pRtlGetVersion;
    UTS_RTL_OSVERSIONINFOW rosvi;

    if (!name) {
        errno = EFAULT;
        return -1;
    }

    memset(name, 0, sizeof(struct utsname));

#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
    strncpy_s(name->sysname, _UTSNAME_LENGTH, "Windows_NT", _TRUNCATE);
#else
    strncpy(name->sysname, "Windows_NT", _UTSNAME_LENGTH - 1);
    name->sysname[_UTSNAME_LENGTH - 1] = '\0';
#endif

    if (!GetComputerNameA(name->nodename, &nodename_len)) {
#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
        strncpy_s(name->nodename, _UTSNAME_LENGTH, "unknown", _TRUNCATE);
#else
        strncpy(name->nodename, "unknown", _UTSNAME_LENGTH - 1);
        name->nodename[_UTSNAME_LENGTH - 1] = '\0';
#endif
    }

    memset(&osvi, 0, sizeof(UTS_OSVERSIONINFOA));
    osvi.dwOSVersionInfoSize = sizeof(UTS_OSVERSIONINFOA);

    hModNtdll = GetModuleHandleA("ntdll.dll");
    pRtlGetVersion = NULL;
    if (hModNtdll) {
        pRtlGetVersion = (RtlGetVersion_Func)(void (*)(void))GetProcAddress(hModNtdll, "RtlGetVersion");
    }

    if (pRtlGetVersion) {
        memset(&rosvi, 0, sizeof(UTS_RTL_OSVERSIONINFOW));
        rosvi.dwOSVersionInfoSize = sizeof(UTS_RTL_OSVERSIONINFOW);
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

#ifndef UTS_NUM_FORMAT
#if defined(_MSC_VER)
#define UTS_NUM_FORMAT "%lu"
#else
#define UTS_NUM_FORMAT "%lu"
#endif
#endif

#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
    sprintf_s(name->release, _UTSNAME_LENGTH, UTS_NUM_FORMAT "." UTS_NUM_FORMAT,
              (unsigned long)osvi.dwMajorVersion,
              (unsigned long)osvi.dwMinorVersion);
    sprintf_s(name->version, _UTSNAME_LENGTH, UTS_NUM_FORMAT,
              (unsigned long)osvi.dwBuildNumber);
#else
    sprintf(name->release, UTS_NUM_FORMAT "." UTS_NUM_FORMAT,
            (unsigned long)osvi.dwMajorVersion,
            (unsigned long)osvi.dwMinorVersion);
    sprintf(name->version, UTS_NUM_FORMAT,
            (unsigned long)osvi.dwBuildNumber);
#endif

    GetSystemInfo(&si);
    switch (si.u.s.wProcessorArchitecture) {
        case UTS_PROCESSOR_ARCHITECTURE_INTEL:
#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
            strncpy_s(name->machine, _UTSNAME_LENGTH, "i686", _TRUNCATE);
#else
            strncpy(name->machine, "i686", _UTSNAME_LENGTH - 1);
            name->machine[_UTSNAME_LENGTH - 1] = '\0';
#endif
            break;
        case UTS_PROCESSOR_ARCHITECTURE_AMD64:
#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
            strncpy_s(name->machine, _UTSNAME_LENGTH, "x86_64", _TRUNCATE);
#else
            strncpy(name->machine, "x86_64", _UTSNAME_LENGTH - 1);
            name->machine[_UTSNAME_LENGTH - 1] = '\0';
#endif
            break;
        case UTS_PROCESSOR_ARCHITECTURE_ARM:
#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
            strncpy_s(name->machine, _UTSNAME_LENGTH, "arm", _TRUNCATE);
#else
            strncpy(name->machine, "arm", _UTSNAME_LENGTH - 1);
            name->machine[_UTSNAME_LENGTH - 1] = '\0';
#endif
            break;
        case UTS_PROCESSOR_ARCHITECTURE_ARM64:
#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
            strncpy_s(name->machine, _UTSNAME_LENGTH, "aarch64", _TRUNCATE);
#else
            strncpy(name->machine, "aarch64", _UTSNAME_LENGTH - 1);
            name->machine[_UTSNAME_LENGTH - 1] = '\0';
#endif
            break;
        case UTS_PROCESSOR_ARCHITECTURE_IA64:
#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
            strncpy_s(name->machine, _UTSNAME_LENGTH, "ia64", _TRUNCATE);
#else
            strncpy(name->machine, "ia64", _UTSNAME_LENGTH - 1);
            name->machine[_UTSNAME_LENGTH - 1] = '\0';
#endif
            break;
        default:
#if defined(__STDC_SECURE_LIB__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
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

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_utsname(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_utsname;

