#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
/**
 * @file posix-dlfcn.c
 * @brief Strict C89 POSIX dlfcn.h implementation for Windows/MSVC.
 */

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

__declspec(dllimport) WIN_HMODULE __stdcall
LoadLibraryA(WIN_LPCSTR lpLibFileName);
__declspec(dllimport) WIN_HMODULE __stdcall
GetModuleHandleA(WIN_LPCSTR lpModuleName);
__declspec(dllimport) int __stdcall FreeLibrary(WIN_HMODULE hLibModule);
__declspec(dllimport) WIN_FARPROC __stdcall
GetProcAddress(WIN_HMODULE hModule, WIN_LPCSTR lpProcName);
__declspec(dllimport) WIN_DWORD __stdcall GetLastError(void);
__declspec(dllimport) WIN_DWORD __stdcall
GetModuleFileNameA(WIN_HMODULE hModule, WIN_LPSTR lpFilename, WIN_DWORD nSize);
__declspec(dllimport) WIN_DWORD __stdcall
FormatMessageA(WIN_DWORD dwFlags, const void *lpSource, WIN_DWORD dwMessageId,
               WIN_DWORD dwLanguageId, WIN_LPSTR lpBuffer, WIN_DWORD nSize,
               void *Arguments);
__declspec(dllimport) size_t __stdcall
VirtualQuery(const void *lpAddress, WIN_MEMORY_BASIC_INFORMATION *lpBuffer,
             size_t dwLength);

static WIN_HMODULE default_dlfcn_load_lib(WIN_LPCSTR name) {
  return LoadLibraryA(name);
}
static WIN_HMODULE default_dlfcn_get_mod(WIN_LPCSTR name) {
  return GetModuleHandleA(name);
}
static int default_dlfcn_free_lib(WIN_HMODULE mod) { return FreeLibrary(mod); }
static WIN_FARPROC default_dlfcn_get_proc(WIN_HMODULE mod, WIN_LPCSTR name) {
  return GetProcAddress(mod, name);
}
static WIN_DWORD default_dlfcn_last_err(void) { return GetLastError(); }
static WIN_DWORD default_dlfcn_mod_fname(WIN_HMODULE mod, WIN_LPSTR buf,
                                         WIN_DWORD sz) {
  return GetModuleFileNameA(mod, buf, sz);
}
static WIN_DWORD default_dlfcn_fmt_msg(WIN_DWORD flags, const void *src,
                                       WIN_DWORD msg_id, WIN_DWORD lang_id,
                                       WIN_LPSTR buf, WIN_DWORD sz,
                                       void *args) {
  return FormatMessageA(flags, src, msg_id, lang_id, buf, sz, args);
}
static size_t default_dlfcn_vquery(const void *addr,
                                   WIN_MEMORY_BASIC_INFORMATION *mbi,
                                   size_t len) {
  return VirtualQuery(addr, mbi, len);
}

static WIN_HMODULE (*g_dlfcn_load_lib)(WIN_LPCSTR) = default_dlfcn_load_lib;
static WIN_HMODULE (*g_dlfcn_get_mod)(WIN_LPCSTR) = default_dlfcn_get_mod;
static int (*g_dlfcn_free_lib)(WIN_HMODULE) = default_dlfcn_free_lib;
static WIN_FARPROC (*g_dlfcn_get_proc)(WIN_HMODULE,
                                       WIN_LPCSTR) = default_dlfcn_get_proc;
static WIN_DWORD (*g_dlfcn_last_err)(void) = default_dlfcn_last_err;
static WIN_DWORD (*g_dlfcn_mod_fname)(WIN_HMODULE, WIN_LPSTR,
                                      WIN_DWORD) = default_dlfcn_mod_fname;
static WIN_DWORD (*g_dlfcn_fmt_msg)(WIN_DWORD, const void *, WIN_DWORD,
                                    WIN_DWORD, WIN_LPSTR, WIN_DWORD,
                                    void *) = default_dlfcn_fmt_msg;
static size_t (*g_dlfcn_vquery)(const void *, WIN_MEMORY_BASIC_INFORMATION *,
                                size_t) = default_dlfcn_vquery;

/**
 * @brief Overrides Win32 API hooks for posix-dlfcn unit testing.
 */
void posix_dlfcn_set_win32_hooks(
    WIN_HMODULE (*load_lib_fn)(WIN_LPCSTR),
    WIN_HMODULE (*get_mod_fn)(WIN_LPCSTR), int (*free_lib_fn)(WIN_HMODULE),
    WIN_FARPROC (*get_proc_fn)(WIN_HMODULE, WIN_LPCSTR),
    WIN_DWORD (*last_err_fn)(void),
    WIN_DWORD (*mod_fname_fn)(WIN_HMODULE, WIN_LPSTR, WIN_DWORD),
    WIN_DWORD (*fmt_msg_fn)(WIN_DWORD, const void *, WIN_DWORD, WIN_DWORD,
                            WIN_LPSTR, WIN_DWORD, void *),
    size_t (*vquery_fn)(const void *, WIN_MEMORY_BASIC_INFORMATION *, size_t)) {
  g_dlfcn_load_lib = load_lib_fn;
  g_dlfcn_get_mod = get_mod_fn;
  g_dlfcn_free_lib = free_lib_fn;
  g_dlfcn_get_proc = get_proc_fn;
  g_dlfcn_last_err = last_err_fn;
  g_dlfcn_mod_fname = mod_fname_fn;
  g_dlfcn_fmt_msg = fmt_msg_fn;
  g_dlfcn_vquery = vquery_fn;
}

/**
 * @brief Resets Win32 API hooks for posix-dlfcn to defaults.
 */
void posix_dlfcn_reset_win32_hooks(void) {
  g_dlfcn_load_lib = default_dlfcn_load_lib;
  g_dlfcn_get_mod = default_dlfcn_get_mod;
  g_dlfcn_free_lib = default_dlfcn_free_lib;
  g_dlfcn_get_proc = default_dlfcn_get_proc;
  g_dlfcn_last_err = default_dlfcn_last_err;
  g_dlfcn_mod_fname = default_dlfcn_mod_fname;
  g_dlfcn_fmt_msg = default_dlfcn_fmt_msg;
  g_dlfcn_vquery = default_dlfcn_vquery;
}

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
 * @brief Sets the thread-local error message from a Windows error code.
 */
static void set_dlerror(WIN_DWORD err_code) {
  size_t len;

  if (err_code == 0) {
    thread_dlerror_set = 0;
    return;
  }

  if (g_dlfcn_fmt_msg(FORMAT_MESSAGE_FROM_SYSTEM |
                          FORMAT_MESSAGE_IGNORE_INSERTS,
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
}

/**
 * @brief Opens a dynamic library and returns a handle.
 */
void *dlopen(const char *file, int mode) {
  WIN_HMODULE handle;
  if (mode < 0) {
    return NULL;
  }

  if (file == NULL) {
    handle = g_dlfcn_get_mod(NULL);
    if (handle == NULL) {
      set_dlerror(g_dlfcn_last_err());
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
        *p = (char)92;
      }
      p++;
    }

    handle = g_dlfcn_load_lib(fixed_file);
  }

  if (handle == NULL) {
    set_dlerror(g_dlfcn_last_err());
    return NULL;
  }

  return (void *)handle;
}

/**
 * @brief Closes a dynamic library handle.
 */
int dlclose(void *handle) {
  if (handle == NULL) {
    return 0;
  }

  if (handle == (void *)g_dlfcn_get_mod(NULL)) {
    return 0;
  }

  if (!g_dlfcn_free_lib((WIN_HMODULE)handle)) {
    set_dlerror(g_dlfcn_last_err());
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

  hModule = (WIN_HMODULE)handle;

  if (name == NULL) {
    set_dlerror(ERROR_INVALID_PARAMETER);
    return NULL;
  }

  if (handle == RTLD_DEFAULT) {
    hModule = g_dlfcn_get_mod(NULL);
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

  proc = g_dlfcn_get_proc(hModule, name);
  if (proc == NULL) {
    set_dlerror(g_dlfcn_last_err());
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

  if (g_dlfcn_vquery(addr, &mbi, sizeof(mbi)) == 0) {
    return 0;
  }

  handle = (WIN_HMODULE)mbi.AllocationBase;
  info->dli_fbase = (void *)handle;

  len = g_dlfcn_mod_fname(handle, thread_dladdr_fname,
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
  if (!file) {
    return NULL;
  }
  if (mode < 0) {
    return NULL;
  }
  return NULL;
}

/**
 * @brief Fallback dlclose when dynamic linking is unsupported.
 */
int dlclose(void *handle) {
  if (!handle) {
    return -1;
  }
  return -1;
}

/**
 * @brief Fallback dlsym when dynamic linking is unsupported.
 */
void *dlsym(void *handle, const char *name) {
  if (!handle) {
    return NULL;
  }
  if (!name) {
    return NULL;
  }
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
  if (!addr) {
    return 0;
  }
  if (!info) {
    return 0;
  }
  return 0;
}

#endif

/**
 * @brief Retrieves information on posix-dlfcn availability.
 */
auto_win_msvc_error_t posix_dlfcn_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_dlfcn;

#endif
typedef int make_iso_compilers_happy_mega_batch3_2;
