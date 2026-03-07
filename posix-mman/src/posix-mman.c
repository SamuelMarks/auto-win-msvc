/* posix-mman.c - Strict C89 Implementation */

#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "posix-mman.h"

/* Win32 Type Definitions to avoid windows.h inflating binary size */
#if defined(_MSC_VER) && !defined(_WIN64)
#define WIN_STDCALL __stdcall
#else
#define WIN_STDCALL
#endif

typedef void* WIN_HANDLE;
typedef unsigned long WIN_DWORD;
typedef WIN_HANDLE WIN_LPVOID;
typedef const void* WIN_LPCVOID;
typedef unsigned int WIN_UINT;

#define WIN_INVALID_HANDLE_VALUE ((WIN_HANDLE)(ptrdiff_t)-1)
#define WIN_PAGE_NOACCESS 0x01
#define WIN_PAGE_READONLY 0x02
#define WIN_PAGE_READWRITE 0x04
#define WIN_PAGE_WRITECOPY 0x08
#define WIN_PAGE_EXECUTE 0x10
#define WIN_PAGE_EXECUTE_READ 0x20
#define WIN_PAGE_EXECUTE_READWRITE 0x40
#define WIN_PAGE_EXECUTE_WRITECOPY 0x80

#define WIN_FILE_MAP_COPY 0x0001
#define WIN_FILE_MAP_WRITE 0x0002
#define WIN_FILE_MAP_READ 0x0004
#define WIN_FILE_MAP_EXECUTE 0x0020

#define WIN_MAX_PATH 260

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
__declspec(dllimport) WIN_DWORD WIN_STDCALL GetTempPathA(WIN_DWORD nBufferLength, char* lpBuffer);
__declspec(dllimport) WIN_HANDLE WIN_STDCALL CreateFileMappingA(WIN_HANDLE hFile, WIN_LPVOID lpFileMappingAttributes, WIN_DWORD flProtect, WIN_DWORD dwMaximumSizeHigh, WIN_DWORD dwMaximumSizeLow, const char* lpName);
__declspec(dllimport) WIN_LPVOID WIN_STDCALL MapViewOfFileEx(WIN_HANDLE hFileMappingObject, WIN_DWORD dwDesiredAccess, WIN_DWORD dwFileOffsetHigh, WIN_DWORD dwFileOffsetLow, size_t dwNumberOfBytesToMap, WIN_LPVOID lpBaseAddress);
__declspec(dllimport) int WIN_STDCALL UnmapViewOfFile(WIN_LPCVOID lpBaseAddress);
__declspec(dllimport) int WIN_STDCALL FlushViewOfFile(WIN_LPCVOID lpBaseAddress, size_t dwNumberOfBytesToFlush);
__declspec(dllimport) int WIN_STDCALL VirtualLock(WIN_LPVOID lpAddress, size_t dwSize);
__declspec(dllimport) int WIN_STDCALL VirtualUnlock(WIN_LPVOID lpAddress, size_t dwSize);
__declspec(dllimport) int WIN_STDCALL VirtualProtect(WIN_LPVOID lpAddress, size_t dwSize, WIN_DWORD flNewProtect, WIN_DWORD* lpflOldProtect);
__declspec(dllimport) int WIN_STDCALL CloseHandle(WIN_HANDLE hObject);
#else
WIN_DWORD WIN_STDCALL GetTempPathA(WIN_DWORD nBufferLength, char* lpBuffer);
WIN_HANDLE WIN_STDCALL CreateFileMappingA(WIN_HANDLE hFile, WIN_LPVOID lpFileMappingAttributes, WIN_DWORD flProtect, WIN_DWORD dwMaximumSizeHigh, WIN_DWORD dwMaximumSizeLow, const char* lpName);
WIN_LPVOID WIN_STDCALL MapViewOfFileEx(WIN_HANDLE hFileMappingObject, WIN_DWORD dwDesiredAccess, WIN_DWORD dwFileOffsetHigh, WIN_DWORD dwFileOffsetLow, size_t dwNumberOfBytesToMap, WIN_LPVOID lpBaseAddress);
int WIN_STDCALL UnmapViewOfFile(WIN_LPCVOID lpBaseAddress);
int WIN_STDCALL FlushViewOfFile(WIN_LPCVOID lpBaseAddress, size_t dwNumberOfBytesToFlush);
int WIN_STDCALL VirtualLock(WIN_LPVOID lpAddress, size_t dwSize);
int WIN_STDCALL VirtualUnlock(WIN_LPVOID lpAddress, size_t dwSize);
int WIN_STDCALL VirtualProtect(WIN_LPVOID lpAddress, size_t dwSize, WIN_DWORD flNewProtect, WIN_DWORD* lpflOldProtect);
int WIN_STDCALL CloseHandle(WIN_HANDLE hObject);
#endif

#ifdef __cplusplus
}
#endif

/* Provide 64-bit int fallback for C89 */
#if defined(_MSC_VER)
typedef unsigned __int64 posix_mman_uint64_t;
#elif defined(__GNUC__)
__extension__ typedef unsigned long long posix_mman_uint64_t;
#else
typedef unsigned long long posix_mman_uint64_t;
#endif

/*
 * mlock - lock a range of process address space
 */
int mlock(const void *addr, size_t len) {
    if (VirtualLock((WIN_LPVOID)addr, len) != 0) {
        return 0;
    }
    errno = ENOMEM;
    return -1;
}

/*
 * mlockall - lock all process address space
 */
int mlockall(int flags) {
    (void)flags;
    return 0;
}

/*
 * mmap - map files or devices into memory
 */
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    WIN_HANDLE hFile = WIN_INVALID_HANDLE_VALUE;
    WIN_HANDLE hMap = NULL;
    WIN_DWORD flProtect = 0;
    WIN_DWORD dwDesiredAccess = 0;
    WIN_DWORD dwMaximumSizeHigh = 0;
    WIN_DWORD dwMaximumSizeLow = 0;
    void *map = NULL;

    if (prot & PROT_EXEC) {
        if (prot & PROT_WRITE) {
            flProtect = WIN_PAGE_EXECUTE_READWRITE;
            dwDesiredAccess = WIN_FILE_MAP_WRITE | WIN_FILE_MAP_EXECUTE;
        } else if (prot & PROT_READ) {
            flProtect = WIN_PAGE_EXECUTE_READ;
            dwDesiredAccess = WIN_FILE_MAP_READ | WIN_FILE_MAP_EXECUTE;
        } else {
            flProtect = WIN_PAGE_EXECUTE;
            dwDesiredAccess = WIN_FILE_MAP_EXECUTE;
        }
    } else {
        if (prot & PROT_WRITE) {
            flProtect = WIN_PAGE_READWRITE;
            dwDesiredAccess = WIN_FILE_MAP_WRITE;
        } else if (prot & PROT_READ) {
            flProtect = WIN_PAGE_READONLY;
            dwDesiredAccess = WIN_FILE_MAP_READ;
        } else {
            flProtect = WIN_PAGE_NOACCESS;
            dwDesiredAccess = 0;
        }
    }

    if (flags & MAP_PRIVATE) {
        if ((prot & PROT_WRITE) && (prot & PROT_EXEC)) {
            flProtect = WIN_PAGE_EXECUTE_WRITECOPY;
            dwDesiredAccess = WIN_FILE_MAP_COPY | WIN_FILE_MAP_EXECUTE;
        } else if (prot & PROT_WRITE) {
            flProtect = WIN_PAGE_WRITECOPY;
            dwDesiredAccess = WIN_FILE_MAP_COPY;
        }
    }

    if ((flags & MAP_ANONYMOUS) == 0) {
        if (fd == -1) {
            errno = EBADF;
            return MAP_FAILED;
        }
#if defined(_MSC_VER)
        hFile = (WIN_HANDLE)(ptrdiff_t)_get_osfhandle(fd);
#else
        hFile = (WIN_HANDLE)(ptrdiff_t)_get_osfhandle(fd);
#endif
        if (hFile == WIN_INVALID_HANDLE_VALUE) {
            errno = EBADF;
            return MAP_FAILED;
        }
    } else {
        posix_mman_uint64_t len64 = (posix_mman_uint64_t)length;
        dwMaximumSizeHigh = (WIN_DWORD)((len64 >> 32) & 0xFFFFFFFF);
        dwMaximumSizeLow = (WIN_DWORD)(len64 & 0xFFFFFFFF);
    }

    hMap = CreateFileMappingA(hFile, NULL, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, NULL);
    if (hMap == NULL) {
        errno = EINVAL;
        return MAP_FAILED;
    }

    {
        posix_mman_uint64_t off64 = (posix_mman_uint64_t)offset;
        map = MapViewOfFileEx(hMap, dwDesiredAccess,
            (WIN_DWORD)((off64 >> 32) & 0xFFFFFFFF),
            (WIN_DWORD)(off64 & 0xFFFFFFFF),
            length, addr);
    }

    CloseHandle(hMap);

    if (map == NULL) {
        errno = EINVAL;
        return MAP_FAILED;
    }

    return map;
}

/*
 * mprotect - set protection on a region of memory
 */
int mprotect(void *addr, size_t len, int prot) {
    WIN_DWORD flProtect = 0;
    WIN_DWORD oldProtect = 0;

    if (prot & PROT_EXEC) {
        if (prot & PROT_WRITE) {
            flProtect = WIN_PAGE_EXECUTE_READWRITE;
        } else if (prot & PROT_READ) {
            flProtect = WIN_PAGE_EXECUTE_READ;
        } else {
            flProtect = WIN_PAGE_EXECUTE;
        }
    } else {
        if (prot & PROT_WRITE) {
            flProtect = WIN_PAGE_READWRITE;
        } else if (prot & PROT_READ) {
            flProtect = WIN_PAGE_READONLY;
        } else {
            flProtect = WIN_PAGE_NOACCESS;
        }
    }

    if (VirtualProtect((WIN_LPVOID)addr, len, flProtect, &oldProtect)) {
        return 0;
    }

    errno = EINVAL;
    return -1;
}

/*
 * msync - synchronize a file with a memory map
 */
int msync(void *addr, size_t length, int flags) {
    (void)flags;

    if (FlushViewOfFile((WIN_LPCVOID)addr, length) != 0) {
        return 0;
    }

    errno = EINVAL;
    return -1;
}

/*
 * munlock - unlock a range of process address space
 */
int munlock(const void *addr, size_t len) {
    if (VirtualUnlock((WIN_LPVOID)addr, len) != 0) {
        return 0;
    }
    errno = ENOMEM;
    return -1;
}

/*
 * munlockall - unlock all process address space
 */
int munlockall(void) {
    return 0;
}

/*
 * munmap - unmap files or devices
 */
int munmap(void *addr, size_t length) {
    (void)length;
    if (UnmapViewOfFile((WIN_LPCVOID)addr)) {
        return 0;
    }
    errno = EINVAL;
    return -1;
}

/* Helper to generate shm file path */
static int generate_shm_path(const char *name, char *file_path, size_t file_path_len) {
    char temp_path[WIN_MAX_PATH];
    size_t i;
    
    if (GetTempPathA(WIN_MAX_PATH, temp_path) == 0) {
        errno = ENOENT;
        return -1;
    }

#if defined(_MSC_VER)
    strncpy_s(file_path, file_path_len, temp_path, _TRUNCATE);
    strncat_s(file_path, file_path_len, "shm_", _TRUNCATE);
#else
    strncpy(file_path, temp_path, file_path_len - 1);
    file_path[file_path_len - 1] = '\0';
    strncat(file_path, "shm_", file_path_len - strlen(file_path) - 1);
#endif

    for (i = 0; name[i] != '\0' && i < 200; ++i) {
        if (name[i] != '/') {
            char buf[2];
            buf[0] = name[i];
            buf[1] = '\0';
#if defined(_MSC_VER)
            strncat_s(file_path, file_path_len, buf, _TRUNCATE);
#else
            strncat(file_path, buf, file_path_len - strlen(file_path) - 1);
#endif
        }
    }
    return 0;
}

/*
 * shm_open - open a shared memory object
 */
int shm_open(const char *name, int oflag, mode_t mode) {
    char file_path[WIN_MAX_PATH];
    int fd;
    
    (void)mode;
    
    if (generate_shm_path(name, file_path, WIN_MAX_PATH) != 0) {
        return -1;
    }
    
    fd = _open(file_path, oflag | _O_BINARY, _S_IREAD | _S_IWRITE);
    if (fd == -1) {
        return -1;
    }
    return fd;
}

/*
 * shm_unlink - remove a shared memory object
 */
int shm_unlink(const char *name) {
    char file_path[WIN_MAX_PATH];
    
    if (generate_shm_path(name, file_path, WIN_MAX_PATH) != 0) {
        return -1;
    }
    
    if (_unlink(file_path) == 0) {
        return 0;
    }
    return -1;
}

#endif

#if defined(__CYGWIN__)
int mlockall(int flags) {
    (void)flags;
    return 0;
}

int munlockall(void) {
    return 0;
}
#endif

/* Ensure strict C compliance requires at least one declaration in translation unit */
typedef int dummy_posix_mman;
