/* posix-mman.c - Strict C89 Implementation */

/* clang-format off */
#if defined(_WIN32) || defined(_WIN64)
#include <fcntl.h>

#ifndef SAFE_GET_OSFHANDLE
#define SAFE_GET_OSFHANDLE
#include <stddef.h>
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)_get_osfhandle(fd))
#else
#define safe_get_osfhandle(fd) ((fd) < 0 ? (ptrdiff_t)-1 : (ptrdiff_t)(fd))
#endif
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <sys/stat.h>
#if defined(_MSC_VER)
#include <share.h>
#endif
#include "posix-mman.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#elif defined(__MSDOS__) || defined(__WATCOMC__)
#include "posix-mman.h"
#include <errno.h>
#include <stddef.h>
#include <sys/types.h>
#endif

#if defined(_WIN32) || defined(_WIN64)

/* Win32 Type Definitions to avoid windows.h inflating binary size */
#if defined(_MSC_VER) && !defined(_WIN64)
#define WIN_STDCALL __stdcall
#else
#define WIN_STDCALL
#endif

typedef void *WIN_HANDLE;
typedef unsigned long WIN_DWORD;
typedef WIN_HANDLE WIN_LPVOID;
typedef const void *WIN_LPCVOID;
typedef unsigned int WIN_UINT;

/** \brief WIN_INVALID_HANDLE_VALUE macro. */
#define WIN_INVALID_HANDLE_VALUE ((WIN_HANDLE)(ptrdiff_t)-1)
#define WIN_PAGE_NOACCESS 0x01
#define WIN_PAGE_READONLY 0x02
#define WIN_PAGE_READWRITE 0x04
#define WIN_PAGE_WRITECOPY 0x08
#define WIN_PAGE_EXECUTE 0x10
#define WIN_PAGE_EXECUTE_READ 0x20
#define WIN_PAGE_EXECUTE_READWRITE 0x40
#define WIN_PAGE_EXECUTE_WRITECOPY 0x80
#define WIN_PAGE_GUARD 0x100

#define WIN_FILE_MAP_COPY 0x0001
#define WIN_FILE_MAP_WRITE 0x0002
#define WIN_FILE_MAP_READ 0x0004
#define WIN_FILE_MAP_EXECUTE 0x0020

#define WIN_MEM_COMMIT 0x1000

#define WIN_MAX_PATH 260

typedef struct _WIN_MEMORY_BASIC_INFORMATION {
  WIN_LPVOID BaseAddress;
  WIN_LPVOID AllocationBase;
  WIN_DWORD AllocationProtect;
  size_t RegionSize;
  WIN_DWORD State;
  WIN_DWORD Protect;
  WIN_DWORD Type;
} WIN_MEMORY_BASIC_INFORMATION;

typedef struct _WIN_SYSTEM_INFO {
  WIN_DWORD dwOemId;
  WIN_DWORD dwPageSize;
  WIN_LPVOID lpMinimumApplicationAddress;
  WIN_LPVOID lpMaximumApplicationAddress;
  size_t dwActiveProcessorMask;
  WIN_DWORD dwNumberOfProcessors;
  WIN_DWORD dwProcessorType;
  WIN_DWORD dwAllocationGranularity;
  unsigned short wProcessorLevel;
  unsigned short wProcessorRevision;
} WIN_SYSTEM_INFO;

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
__declspec(dllimport) void WIN_STDCALL
    GetSystemInfo(WIN_SYSTEM_INFO *lpSystemInfo);
__declspec(dllimport) size_t WIN_STDCALL
    VirtualQuery(WIN_LPCVOID lpAddress, WIN_MEMORY_BASIC_INFORMATION *lpBuffer,
                 size_t dwLength);
__declspec(dllimport) WIN_DWORD WIN_STDCALL
    GetTempPathA(WIN_DWORD nBufferLength, char *lpBuffer);
__declspec(dllimport) WIN_HANDLE WIN_STDCALL
    CreateFileMappingA(WIN_HANDLE hFile, WIN_LPVOID lpFileMappingAttributes,
                       WIN_DWORD flProtect, WIN_DWORD dwMaximumSizeHigh,
                       WIN_DWORD dwMaximumSizeLow, const char *lpName);
__declspec(dllimport) WIN_LPVOID WIN_STDCALL
    MapViewOfFileEx(WIN_HANDLE hFileMappingObject, WIN_DWORD dwDesiredAccess,
                    WIN_DWORD dwFileOffsetHigh, WIN_DWORD dwFileOffsetLow,
                    size_t dwNumberOfBytesToMap, WIN_LPVOID lpBaseAddress);
__declspec(dllimport) int WIN_STDCALL
    UnmapViewOfFile(WIN_LPCVOID lpBaseAddress);
__declspec(dllimport) int WIN_STDCALL
    FlushViewOfFile(WIN_LPCVOID lpBaseAddress, size_t dwNumberOfBytesToFlush);
__declspec(dllimport) WIN_LPVOID WIN_STDCALL
    VirtualAlloc(WIN_LPVOID lpAddress, size_t dwSize,
                 WIN_DWORD flAllocationType, WIN_DWORD flProtect);
__declspec(dllimport) int WIN_STDCALL
    VirtualLock(WIN_LPVOID lpAddress, size_t dwSize);
__declspec(dllimport) int WIN_STDCALL
    VirtualUnlock(WIN_LPVOID lpAddress, size_t dwSize);
__declspec(dllimport) int WIN_STDCALL
    VirtualProtect(WIN_LPVOID lpAddress, size_t dwSize, WIN_DWORD flNewProtect,
                   WIN_DWORD *lpflOldProtect);
__declspec(dllimport) int WIN_STDCALL CloseHandle(WIN_HANDLE hObject);
#else
/** \brief GetSystemInfo function. */
void WIN_STDCALL GetSystemInfo(WIN_SYSTEM_INFO *lpSystemInfo);
/** \brief VirtualQuery function. */
size_t WIN_STDCALL VirtualQuery(WIN_LPCVOID lpAddress,
                                WIN_MEMORY_BASIC_INFORMATION *lpBuffer,
                                size_t dwLength);
/** \brief GetTempPathA function. */
WIN_DWORD WIN_STDCALL GetTempPathA(WIN_DWORD nBufferLength, char *lpBuffer);
/** \brief CreateFileMappingA function. */
WIN_HANDLE WIN_STDCALL CreateFileMappingA(WIN_HANDLE hFile,
                                          WIN_LPVOID lpFileMappingAttributes,
                                          WIN_DWORD flProtect,
                                          WIN_DWORD dwMaximumSizeHigh,
                                          WIN_DWORD dwMaximumSizeLow,
                                          const char *lpName);
/** \brief MapViewOfFileEx function. */
WIN_LPVOID WIN_STDCALL MapViewOfFileEx(WIN_HANDLE hFileMappingObject,
                                       WIN_DWORD dwDesiredAccess,
                                       WIN_DWORD dwFileOffsetHigh,
                                       WIN_DWORD dwFileOffsetLow,
                                       size_t dwNumberOfBytesToMap,
                                       WIN_LPVOID lpBaseAddress);
/** \brief UnmapViewOfFile function. */
int WIN_STDCALL UnmapViewOfFile(WIN_LPCVOID lpBaseAddress);
/** \brief FlushViewOfFile function. */
int WIN_STDCALL FlushViewOfFile(WIN_LPCVOID lpBaseAddress,
                                size_t dwNumberOfBytesToFlush);
/** \brief VirtualAlloc function. */
WIN_LPVOID WIN_STDCALL VirtualAlloc(WIN_LPVOID lpAddress, size_t dwSize,
                                    WIN_DWORD flAllocationType,
                                    WIN_DWORD flProtect);
/** \brief VirtualLock function. */
int WIN_STDCALL VirtualLock(WIN_LPVOID lpAddress, size_t dwSize);
/** \brief VirtualUnlock function. */
int WIN_STDCALL VirtualUnlock(WIN_LPVOID lpAddress, size_t dwSize);
/** \brief VirtualProtect function. */
int WIN_STDCALL VirtualProtect(WIN_LPVOID lpAddress, size_t dwSize,
                               WIN_DWORD flNewProtect,
                               WIN_DWORD *lpflOldProtect);
/** \brief CloseHandle function. */
int WIN_STDCALL CloseHandle(WIN_HANDLE hObject);
#endif

#ifdef __cplusplus
}
#endif

/* Provide 64-bit int fallback for C89 */
#if defined(_MSC_VER) || defined(__WATCOMC__)
typedef unsigned __int64 posix_mman_uint64_t;
#elif defined(__GNUC__)
__extension__ typedef unsigned long long posix_mman_uint64_t;
#else
typedef unsigned long long posix_mman_uint64_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllimport) void *__stdcall GetCurrentProcess(void);
__declspec(dllimport) void *__stdcall GetModuleHandleA(
    const char *lpModuleName);
__declspec(dllimport) void *__stdcall GetProcAddress(void *hModule,
                                                     const char *lpProcName);

#ifdef __cplusplus
}
#endif

/*
 * madvise - give advice about use of memory
 */
int madvise(void *addr, size_t length, int advice) {
#if defined(_WIN32)
  void *hProcess;
  void *hKernel32;
  typedef int(__stdcall * PrefetchVirtualMemory_t)(void *, size_t, void *,
                                                   unsigned long);
  PrefetchVirtualMemory_t pPrefetchVirtualMemory = NULL;

  if (!addr || length == 0) {
    errno = EINVAL;
    return -1;
  }

  if (advice == 4 /* MADV_DONTNEED */ || advice == 8 /* MADV_FREE */) {
    /* POSIX advice: the application doesn't need this memory.
       On Windows, MEM_RESET tells the OS it can discard the pages if needed
       without writing them to the paging file. It doesn't free the address
       space. */
    VirtualAlloc((WIN_LPVOID)(size_t)addr, length, 0x00080000 /* MEM_RESET */,
                 0x04 /* PAGE_READWRITE */);
    return 0;
  }

  if (advice != 3 /* MADV_WILLNEED */) {
    return 0;
  }

  hProcess = GetCurrentProcess();
  hKernel32 = GetModuleHandleA("kernel32.dll");

  if (hKernel32) {
    pPrefetchVirtualMemory = (PrefetchVirtualMemory_t)(size_t)GetProcAddress(
        hKernel32, "PrefetchVirtualMemory");
  }

  if (pPrefetchVirtualMemory) {
    struct {
      void *VirtualAddress;
      size_t NumberOfBytes;
    } entry;
    entry.VirtualAddress = addr;
    entry.NumberOfBytes = length;
    pPrefetchVirtualMemory(hProcess, (size_t)1, &entry, 0);
  }
  return 0;
#else
  (void)addr;
  (void)length;
  (void)advice;
  errno = ENOSYS;
  return -1;
#endif
}

/*
 * mlock - lock a range of process address space

 */
int mlock(const void *addr, size_t len) {
  if (VirtualLock((WIN_LPVOID)(size_t)addr, len) != 0) {
    return 0;
  }
  errno = ENOMEM;
  return -1;
}

/*
 * mlockall - lock all process address space
 */
int mlockall(int flags) {
  WIN_SYSTEM_INFO si;
  WIN_LPVOID addr = 0;
  WIN_MEMORY_BASIC_INFORMATION mbi;

  if (flags & ~(1 | 2)) {
    errno = EINVAL;
    return -1;
  }

  if (!(flags & 1 /* MCL_CURRENT */)) {
    return 0;
  }

  GetSystemInfo(&si);

  while (addr < si.lpMaximumApplicationAddress) {
    if (VirtualQuery(addr, &mbi, sizeof(mbi)) == 0) {
      break;
    }

    if (mbi.State == WIN_MEM_COMMIT && !(mbi.Protect & WIN_PAGE_NOACCESS) &&
        !(mbi.Protect & WIN_PAGE_GUARD)) {
      VirtualLock(mbi.BaseAddress, mbi.RegionSize);
    }

    {
      WIN_LPVOID next_addr =
          (WIN_LPVOID)((char *)mbi.BaseAddress + mbi.RegionSize);
      if (next_addr <= addr) {
        break;
      }
      addr = next_addr;
    }
  }

  return 0;
}

/*
 * mmap - map files or devices into memory


 */
void *mmap(void *addr, size_t length, int prot, int flags, int fd,
           off_t offset) {
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
    hFile = (WIN_HANDLE)(ptrdiff_t)safe_get_osfhandle(fd);
#else
    hFile = (WIN_HANDLE)(ptrdiff_t)safe_get_osfhandle(fd);
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

  hMap = CreateFileMappingA(hFile, NULL, flProtect, dwMaximumSizeHigh,
                            dwMaximumSizeLow, NULL);
  if (hMap == NULL) {
    errno = EINVAL;
    return MAP_FAILED;
  }

  {
    posix_mman_uint64_t off64 = (posix_mman_uint64_t)offset;
    map = MapViewOfFileEx(hMap, dwDesiredAccess,
                          (WIN_DWORD)((off64 >> 32) & 0xFFFFFFFF),
                          (WIN_DWORD)(off64 & 0xFFFFFFFF), length, addr);
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

  if (VirtualProtect((WIN_LPVOID)(size_t)addr, len, flProtect, &oldProtect)) {
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
  if (VirtualUnlock((WIN_LPVOID)(size_t)addr, len) != 0) {
    return 0;
  }
  errno = ENOMEM;
  return -1;
}

/*
 * munlockall - unlock all process address space
 */
int munlockall(void) {
  WIN_SYSTEM_INFO si;
  WIN_LPVOID addr = 0;
  WIN_MEMORY_BASIC_INFORMATION mbi;

  GetSystemInfo(&si);

  while (addr < si.lpMaximumApplicationAddress) {
    if (VirtualQuery(addr, &mbi, sizeof(mbi)) == 0) {
      break;
    }

    if (mbi.State == WIN_MEM_COMMIT && !(mbi.Protect & WIN_PAGE_NOACCESS) &&
        !(mbi.Protect & WIN_PAGE_GUARD)) {
      VirtualUnlock(mbi.BaseAddress, mbi.RegionSize);
    }

    {
      WIN_LPVOID next_addr =
          (WIN_LPVOID)((char *)mbi.BaseAddress + mbi.RegionSize);
      if (next_addr <= addr) {
        break;
      }
      addr = next_addr;
    }
  }

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
static int generate_shm_path(const char *name, char *file_path,
                             size_t file_path_len) {
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

#if defined(_MSC_VER)
  if (_sopen_s(&fd, file_path, oflag | _O_BINARY, _SH_DENYNO,
               _S_IREAD | _S_IWRITE) != 0) {
    return -1;
  }
#else
  fd = _open(file_path, oflag | _O_BINARY, _S_IREAD | _S_IWRITE);
  if (fd == -1) {
    return -1;
  }
#endif
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

#elif defined(__MSDOS__) || defined(__WATCOMC__)

int madvise(void *addr, size_t length, int advice) {
  (void)addr;
  (void)length;
  (void)advice;
  errno = ENOSYS;
  return -1;
}

int mlock(const void *addr, size_t len) {
  (void)addr;
  (void)len;
  errno = ENOSYS;
  return -1;
}

int mlockall(int flags) {
  (void)flags;
  errno = ENOSYS;
  return -1;
}

void *mmap(void *addr, size_t length, int prot, int flags, int fd,
           off_t offset) {
  (void)addr;
  (void)length;
  (void)prot;
  (void)flags;
  (void)fd;
  (void)offset;
  errno = ENOSYS;
  return (void *)-1;
}

int mprotect(void *addr, size_t len, int prot) {
  (void)addr;
  (void)len;
  (void)prot;
  errno = ENOSYS;
  return -1;
}

int msync(void *addr, size_t length, int flags) {
  (void)addr;
  (void)length;
  (void)flags;
  errno = ENOSYS;
  return -1;
}

int munlock(const void *addr, size_t len) {
  (void)addr;
  (void)len;
  errno = ENOSYS;
  return -1;
}

int munlockall(void) {
  errno = ENOSYS;
  return -1;
}

int munmap(void *addr, size_t length) {
  (void)addr;
  (void)length;
  errno = ENOSYS;
  return -1;
}

int shm_open(const char *name, int oflag, mode_t mode) {
  (void)name;
  (void)oflag;
  (void)mode;
  errno = ENOSYS;
  return -1;
}

int shm_unlink(const char *name) {
  (void)name;
  errno = ENOSYS;
  return -1;
}

#endif

#if defined(__CYGWIN__)
#include <errno.h>
/* clang-format on */

/** rief munlockall function. */
int munlockall(void) { return 0; }

#endif

/* Ensure strict C compliance requires at least one declaration in translation
 * unit */
typedef int dummy_posix_mman;

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

typedef int make_iso_compilers_happy_tu_posix_mman;
