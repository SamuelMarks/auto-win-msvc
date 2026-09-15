/* posix-dlfcn.c - Strict C89 Implementation */
/* clang-format off */
#include "posix-dlfcn.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#ifndef NUM_FORMAT_CAST
#if defined(_MSC_VER)
#define NUM_FORMAT_CAST __int64
#else
#define NUM_FORMAT_CAST long
#endif
#endif

#ifndef NUM_FORMAT
#if defined(_MSC_VER)
#define NUM_FORMAT "%I64d"
#else
#define NUM_FORMAT "%ld"
#endif
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) ||                \
    defined(__WIN64__)

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef FORMAT_MESSAGE_FROM_SYSTEM
#define FORMAT_MESSAGE_FROM_SYSTEM 0x00001000
#endif
#ifndef FORMAT_MESSAGE_IGNORE_INSERTS
#define FORMAT_MESSAGE_IGNORE_INSERTS 0x00000200
#endif
#ifndef ERROR_INVALID_PARAMETER
#define ERROR_INVALID_PARAMETER 87L
#endif

#ifndef MAKELANGID
#define MAKELANGID(p, s) ((((unsigned short)(s)) << 10) | (unsigned short)(p))
#define LANG_NEUTRAL 0x00
#define SUBLANG_DEFAULT 0x01
#endif

typedef unsigned long WIN_DWORD;
typedef void *WIN_HANDLE;
typedef const char *WIN_LPCSTR;
typedef char *WIN_LPSTR;
typedef struct HINSTANCE__ *WIN_HINSTANCE;
typedef WIN_HINSTANCE WIN_HMODULE;
typedef int(__stdcall *WIN_FARPROC)(void);

typedef struct _WIN_MEMORY_BASIC_INFORMATION {
  void *BaseAddress;
  void *AllocationBase;
  WIN_DWORD AllocationProtect;
  size_t RegionSize;
  WIN_DWORD State;
  WIN_DWORD Protect;
  WIN_DWORD Type;
} WIN_MEMORY_BASIC_INFORMATION;

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllimport)
WIN_HMODULE __stdcall LoadLibraryA(WIN_LPCSTR lpLibFileName);
__declspec(dllimport)
WIN_HMODULE __stdcall GetModuleHandleA(WIN_LPCSTR lpModuleName);
__declspec(dllimport) int __stdcall FreeLibrary(WIN_HMODULE hLibModule);
__declspec(dllimport)
WIN_FARPROC __stdcall GetProcAddress(WIN_HMODULE hModule,
                                     WIN_LPCSTR lpProcName);
__declspec(dllimport) WIN_DWORD __stdcall GetLastError(void);
__declspec(dllimport)
WIN_DWORD __stdcall GetModuleFileNameA(WIN_HMODULE hModule,
                                       WIN_LPSTR lpFilename, WIN_DWORD nSize);
__declspec(dllimport)
WIN_DWORD __stdcall FormatMessageA(WIN_DWORD dwFlags, const void *lpSource,
                                   WIN_DWORD dwMessageId,
                                   WIN_DWORD dwLanguageId, WIN_LPSTR lpBuffer,
                                   WIN_DWORD nSize, void *Arguments);
__declspec(dllimport)
size_t __stdcall VirtualQuery(const void *lpAddress,
                              WIN_MEMORY_BASIC_INFORMATION *lpBuffer,
                              size_t dwLength);

#ifdef __cplusplus
}
#endif

/* TLS storage for thread-local error message and module name buffer */
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#define THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
#define THREAD_LOCAL __thread
#else
#define THREAD_LOCAL
#endif

static THREAD_LOCAL char thread_dlerror_msg[512] = {0};
static THREAD_LOCAL int thread_dlerror_set = 0;
static THREAD_LOCAL char thread_dladdr_fname[MAX_PATH] = {0};

/**
 * @brief Helper to set the thread-local error message from a Windows error
 * code.
 * @param err_code Windows system error code.
 * @return POSIX_DLFCN_SUCCESS on completion.
 */
static enum posix_dlfcn_error_code set_dlerror(WIN_DWORD err_code) {
  size_t len;

  if (err_code == 0) {
    thread_dlerror_set = 0;
    return POSIX_DLFCN_SUCCESS;
  }

  if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                     NULL, err_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                     thread_dlerror_msg,
                     (WIN_DWORD)(sizeof(thread_dlerror_msg) - 1), NULL) == 0) {
#if defined(_MSC_VER)
    sprintf_s(thread_dlerror_msg, sizeof(thread_dlerror_msg),
              "Unknown error code: " NUM_FORMAT, (NUM_FORMAT_CAST)err_code);
#else
    sprintf(thread_dlerror_msg, "Unknown error code: " NUM_FORMAT,
            (NUM_FORMAT_CAST)err_code);
#endif
  } else {
    len = strlen(thread_dlerror_msg);
    while (len > 0 && (thread_dlerror_msg[len - 1] == 13 ||
                       thread_dlerror_msg[len - 1] == 10)) {
      thread_dlerror_msg[len - 1] = 0;
      len--;
    }
  }
  thread_dlerror_set = 1;
  return POSIX_DLFCN_SUCCESS;
}

/**
 * @brief Opens a dynamic library and returns a handle.
 */
void *dlopen(const char *file, int mode) {
  WIN_HMODULE handle;
  enum posix_dlfcn_error_code rc;
  (void)mode;

  if (file == NULL) {
    handle = GetModuleHandleA(NULL);
    if (handle == NULL) {
      rc = set_dlerror(GetLastError());
      if (rc != POSIX_DLFCN_SUCCESS) {
        return NULL;
      }
    }
    return (void *)handle;
  }

  {
    char fixed_file[MAX_PATH];
    char *lib_prefix;
    char *dot;
    char *p;

#if defined(_MSC_VER)
    strncpy_s(fixed_file, sizeof(fixed_file), file, _TRUNCATE);
#else
    strncpy(fixed_file, file, sizeof(fixed_file) - 1);
#endif
    fixed_file[sizeof(fixed_file) - 1] = '\0';

    lib_prefix = strstr(fixed_file, "libvalkeylua");
    if (lib_prefix) {
      memmove(lib_prefix, lib_prefix + 3, strlen(lib_prefix + 3) + 1);
    }

    dot = strrchr(fixed_file, '.');
    if (dot && (strcmp(dot, ".dylib") == 0 || strcmp(dot, ".so") == 0)) {
#if defined(_MSC_VER)
      strcpy_s(dot, sizeof(fixed_file) - (size_t)(dot - fixed_file), ".dll");
#else
      strcpy(dot, ".dll");
#endif
    }

    p = fixed_file;
    while (*p) {
      if (*p == '/') {
        *p = '\\';
      }
      p++;
    }

    handle = LoadLibraryA(fixed_file);
  }

  if (handle == NULL) {
    rc = set_dlerror(GetLastError());
    if (rc != POSIX_DLFCN_SUCCESS) {
      return NULL;
    }
    return NULL;
  }

  return (void *)handle;
}

/**
 * @brief Closes a dynamic library handle.
 */
int dlclose(void *handle) {
  enum posix_dlfcn_error_code rc;

  if (handle == NULL) {
    return 0;
  }

  if (handle == (void *)GetModuleHandleA(NULL)) {
    return 0;
  }

  if (!FreeLibrary((WIN_HMODULE)handle)) {
    rc = set_dlerror(GetLastError());
    if (rc != POSIX_DLFCN_SUCCESS) {
      return -1;
    }
    return -1;
  }

  return 0;
}

/**
 * @brief Obtains the address of a symbol within a dynamic library.
 */
void *dlsym(void *handle, const char *name) {
  WIN_FARPROC proc;
  void *ret_ptr;
  WIN_HMODULE hModule;
  enum posix_dlfcn_error_code rc;

  hModule = (WIN_HMODULE)handle;

  if (name == NULL) {
    rc = set_dlerror(ERROR_INVALID_PARAMETER);
    if (rc != POSIX_DLFCN_SUCCESS) {
      return NULL;
    }
    return NULL;
  }

  if (handle == RTLD_DEFAULT) {
    hModule = GetModuleHandleA(NULL);
  } else if (handle == RTLD_NEXT) {
#if defined(_MSC_VER)
    strcpy_s(thread_dlerror_msg, sizeof(thread_dlerror_msg),
             "RTLD_NEXT is not supported");
#else
    strncpy(thread_dlerror_msg, "RTLD_NEXT is not supported",
            sizeof(thread_dlerror_msg) - 1);
    thread_dlerror_msg[sizeof(thread_dlerror_msg) - 1] = '\0';
#endif
    thread_dlerror_set = 1;
    return NULL;
  }

  proc = GetProcAddress(hModule, name);
  if (proc == NULL) {
    rc = set_dlerror(GetLastError());
    if (rc != POSIX_DLFCN_SUCCESS) {
      return NULL;
    }
    return NULL;
  }

  ret_ptr = NULL;
#if defined(_MSC_VER)
  memcpy_s(&ret_ptr, sizeof(ret_ptr), &proc, sizeof(proc));
#else
  memcpy(&ret_ptr, &proc, sizeof(proc));
#endif
  return ret_ptr;
}

/**
 * @brief Returns a human-readable string describing the most recent error.
 */
char *dlerror(void) {
  if (thread_dlerror_set) {
    thread_dlerror_set = 0;
    return thread_dlerror_msg;
  }
  return NULL;
}

/**
 * @brief Translates an address to symbol information.
 */
int dladdr(const void *addr, Dl_info *info) {
  WIN_MEMORY_BASIC_INFORMATION mbi;
  WIN_HMODULE handle;
  WIN_DWORD len;

  if (info == NULL || addr == NULL) {
    return 0;
  }

  if (VirtualQuery(addr, &mbi, sizeof(mbi)) == 0) {
    return 0;
  }

  handle = (WIN_HMODULE)mbi.AllocationBase;
  info->dli_fbase = (void *)handle;

  len = GetModuleFileNameA(handle, thread_dladdr_fname,
                           (WIN_DWORD)sizeof(thread_dladdr_fname));
  if (len == 0 || len == sizeof(thread_dladdr_fname)) {
    info->dli_fname = NULL;
  } else {
    info->dli_fname = thread_dladdr_fname;
  }

  info->dli_sname = NULL;
  info->dli_saddr = NULL;

  return 1;
}

#elif !defined(__linux__) && !defined(__CYGWIN__) && !defined(__APPLE__) &&    \
    !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__) &&  \
    !defined(__sun) && !defined(__QNX__)

/**
 * @brief Fallback dlopen when dynamic linking is unsupported.
 */
void *dlopen(const char *file, int mode) {
  (void)file;
  (void)mode;
  return NULL;
}

/**
 * @brief Fallback dlclose when dynamic linking is unsupported.
 */
int dlclose(void *handle) {
  (void)handle;
  return -1;
}

/**
 * @brief Fallback dlsym when dynamic linking is unsupported.
 */
void *dlsym(void *handle, const char *name) {
  (void)handle;
  (void)name;
  return NULL;
}

/**
 * @brief Fallback dlerror when dynamic linking is unsupported.
 */
char *dlerror(void) { return "Dynamic linking not supported on this platform"; }

/**
 * @brief Fallback dladdr when dynamic linking is unsupported.
 */
int dladdr(const void *addr, Dl_info *info) {
  (void)addr;
  (void)info;
  return 0;
}

#endif

/**
 * @brief Retrieves information on posix-dlfcn availability.
 */
enum posix_dlfcn_error_code posix_dlfcn_get_info(int *out_available) {
  if (out_available == NULL) {
    return POSIX_DLFCN_ERROR_NULL_POINTER;
  }
  *out_available = 1;
  return POSIX_DLFCN_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_dlfcn;
