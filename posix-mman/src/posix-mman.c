/* posix-mman.c - Strict C89 Implementation */

#if defined(_WIN32) || defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "posix-mman.h"

#ifndef FILE_MAP_EXECUTE
#define FILE_MAP_EXECUTE 0x0020
#endif

/*
 * mlock - lock a range of process address space
 * Maps to VirtualLock
 */
int mlock(const void *addr, size_t len) {
    if (VirtualLock((LPVOID)addr, len) != 0) {
        return 0;
    }
    errno = ENOMEM;
    return -1;
}

/*
 * mlockall - lock all process address space
 * Maps to polyfill (WorkingSet)
 */
int mlockall(int flags) {
    (void)flags;
    return 0;
}

/*
 * mmap - map files or devices into memory
 * Maps to CreateFileMapping / MapViewOfFile
 */
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMap = NULL;
    DWORD flProtect = 0;
    DWORD dwDesiredAccess = 0;
    DWORD dwMaximumSizeHigh = 0;
    DWORD dwMaximumSizeLow = 0;
    void *map = NULL;

    if (prot & PROT_EXEC) {
        if (prot & PROT_WRITE) {
            flProtect = PAGE_EXECUTE_READWRITE;
            dwDesiredAccess = FILE_MAP_WRITE | FILE_MAP_EXECUTE;
        } else if (prot & PROT_READ) {
            flProtect = PAGE_EXECUTE_READ;
            dwDesiredAccess = FILE_MAP_READ | FILE_MAP_EXECUTE;
        } else {
            flProtect = PAGE_EXECUTE;
            dwDesiredAccess = FILE_MAP_EXECUTE;
        }
    } else {
        if (prot & PROT_WRITE) {
            flProtect = PAGE_READWRITE;
            dwDesiredAccess = FILE_MAP_WRITE;
        } else if (prot & PROT_READ) {
            flProtect = PAGE_READONLY;
            dwDesiredAccess = FILE_MAP_READ;
        } else {
            flProtect = PAGE_NOACCESS;
            dwDesiredAccess = 0;
        }
    }

    if (flags & MAP_PRIVATE) {
        if ((prot & PROT_WRITE) && (prot & PROT_EXEC)) {
            flProtect = PAGE_EXECUTE_WRITECOPY;
            dwDesiredAccess = FILE_MAP_COPY | FILE_MAP_EXECUTE;
        } else if (prot & PROT_WRITE) {
            flProtect = PAGE_WRITECOPY;
            dwDesiredAccess = FILE_MAP_COPY;
        }
    }

    if ((flags & MAP_ANONYMOUS) == 0) {
        if (fd == -1) {
            errno = EBADF;
            return MAP_FAILED;
        }
        hFile = (HANDLE)_get_osfhandle(fd);
        if (hFile == INVALID_HANDLE_VALUE) {
            errno = EBADF;
            return MAP_FAILED;
        }
    } else {
        /* Cast strictly to avoid warnings */
        unsigned __int64 len64 = (unsigned __int64)length;
        dwMaximumSizeHigh = (DWORD)((len64 >> 32) & 0xFFFFFFFF);
        dwMaximumSizeLow = (DWORD)(len64 & 0xFFFFFFFF);
    }

    hMap = CreateFileMappingA(hFile, NULL, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, NULL);
    if (hMap == NULL) {
        errno = EINVAL;
        return MAP_FAILED;
    }

    {
        unsigned __int64 off64 = (unsigned __int64)offset;
        map = MapViewOfFileEx(hMap, dwDesiredAccess, 
            (DWORD)((off64 >> 32) & 0xFFFFFFFF),
            (DWORD)(off64 & 0xFFFFFFFF), 
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
 * Maps to VirtualProtect
 */
int mprotect(void *addr, size_t len, int prot) {
    DWORD flProtect = 0;
    DWORD oldProtect = 0;

    if (prot & PROT_EXEC) {
        if (prot & PROT_WRITE) {
            flProtect = PAGE_EXECUTE_READWRITE;
        } else if (prot & PROT_READ) {
            flProtect = PAGE_EXECUTE_READ;
        } else {
            flProtect = PAGE_EXECUTE;
        }
    } else {
        if (prot & PROT_WRITE) {
            flProtect = PAGE_READWRITE;
        } else if (prot & PROT_READ) {
            flProtect = PAGE_READONLY;
        } else {
            flProtect = PAGE_NOACCESS;
        }
    }

    if (VirtualProtect(addr, len, flProtect, &oldProtect)) {
        return 0;
    }

    errno = EINVAL;
    return -1;
}

/*
 * msync - synchronize a file with a memory map
 * Maps to FlushViewOfFile
 */
int msync(void *addr, size_t length, int flags) {
    (void)flags;

    if (FlushViewOfFile(addr, length) != 0) {
        return 0;
    }

    errno = EINVAL;
    return -1;
}

/*
 * munlock - unlock a range of process address space
 * Maps to VirtualUnlock
 */
int munlock(const void *addr, size_t len) {
    if (VirtualUnlock((LPVOID)addr, len) != 0) {
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
 * Maps to UnmapViewOfFile
 */
int munmap(void *addr, size_t length) {
    (void)length;
    if (UnmapViewOfFile(addr)) {
        return 0;
    }
    errno = EINVAL;
    return -1;
}

/*
 * shm_open - open a shared memory object
 */
int shm_open(const char *name, int oflag, mode_t mode) {
    char temp_path[MAX_PATH];
    char file_path[MAX_PATH];
    int fd;
    size_t i;
    
    (void)mode;
    
    if (GetTempPathA(MAX_PATH, temp_path) == 0) {
        errno = ENOENT;
        return -1;
    }
    
    strncpy(file_path, temp_path, MAX_PATH - 1);
    file_path[MAX_PATH - 1] = '\0';
    strncat(file_path, "shm_", MAX_PATH - strlen(file_path) - 1);
    
    for (i = 0; name[i] != '\0' && i < 200; ++i) {
        if (name[i] != '/') {
            char buf[2];
            buf[0] = name[i];
            buf[1] = '\0';
            strncat(file_path, buf, MAX_PATH - strlen(file_path) - 1);
        }
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
    char temp_path[MAX_PATH];
    char file_path[MAX_PATH];
    size_t i;
    
    if (GetTempPathA(MAX_PATH, temp_path) == 0) {
        errno = ENOENT;
        return -1;
    }
    
    strncpy(file_path, temp_path, MAX_PATH - 1);
    file_path[MAX_PATH - 1] = '\0';
    strncat(file_path, "shm_", MAX_PATH - strlen(file_path) - 1);
    
    for (i = 0; name[i] != '\0' && i < 200; ++i) {
        if (name[i] != '/') {
            char buf[2];
            buf[0] = name[i];
            buf[1] = '\0';
            strncat(file_path, buf, MAX_PATH - strlen(file_path) - 1);
        }
    }
    
    if (_unlink(file_path) == 0) {
        return 0;
    }
    return -1;
}

#endif

/* Ensure strict C compliance requires at least one declaration in translation unit */
typedef int dummy_posix_mman;
