/**
 * @file posix-dirent.c
 * @brief Strict C89 implementation of POSIX dirent.h and /proc emulation.
 */

/* clang-format off */
#include "posix-dirent.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(_WIN32) && defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
#include <sys/sysctl.h>
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#include <sys/types.h>

#if defined(__MINGW32__) || defined(__MINGW64__)
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
#elif defined(_MSC_VER) && _MSC_VER >= 1400
#include <stddef.h>
#else
#ifndef intptr_t
typedef long intptr_t;
#endif
#endif
#endif
/* clang-format on */

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) ||                 \
    (!defined(__linux__) && !defined(__CYGWIN__))
static void *default_dirent_malloc(size_t sz) { return malloc(sz); }
static void *(*g_dirent_malloc)(size_t) = default_dirent_malloc;
#endif
#if !defined(_WIN32) && !defined(__WIN32__) && !defined(WIN32) &&              \
    !defined(__linux__) && !defined(__CYGWIN__)
static void *default_dirent_calloc(size_t n, size_t sz) {
  return calloc(n, sz);
}
static void *(*g_dirent_calloc)(size_t, size_t) = default_dirent_calloc;
#endif
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
static void *default_dirent_realloc(void *ptr, size_t sz) {
  return realloc(ptr, sz);
}
static void *(*g_dirent_realloc)(void *, size_t) = default_dirent_realloc;
#endif

/**
 * @brief Overrides allocator hooks used by posix-dirent for OOM testing.
 * @param[in] malloc_fn Custom malloc, or NULL for default.
 * @param[in] calloc_fn Custom calloc, or NULL for default.
 * @param[in] realloc_fn Custom realloc, or NULL for default.
 */
void posix_dirent_set_alloc_hooks(void *(*malloc_fn)(size_t),
                                  void *(*calloc_fn)(size_t, size_t),
                                  void *(*realloc_fn)(void *, size_t));
void posix_dirent_set_alloc_hooks(void *(*malloc_fn)(size_t),
                                  void *(*calloc_fn)(size_t, size_t),
                                  void *(*realloc_fn)(void *, size_t)) {
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) ||                 \
    (!defined(__linux__) && !defined(__CYGWIN__))
  g_dirent_malloc = (malloc_fn != NULL) ? malloc_fn : default_dirent_malloc;
#else
  (void)malloc_fn;
#endif
#if !defined(_WIN32) && !defined(__WIN32__) && !defined(WIN32) &&              \
    !defined(__linux__) && !defined(__CYGWIN__)
  g_dirent_calloc = (calloc_fn != NULL) ? calloc_fn : default_dirent_calloc;
#else
  (void)calloc_fn;
#endif
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
  g_dirent_realloc = (realloc_fn != NULL) ? realloc_fn : default_dirent_realloc;
#else
  (void)realloc_fn;
#endif
}

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)

typedef void *DIR_WIN_HANDLE;
typedef unsigned long DIR_WIN_DWORD;
typedef int DIR_WIN_BOOL;

#define DIR_INVALID_HANDLE_VALUE ((DIR_WIN_HANDLE)(intptr_t)(-1))
#define DIR_TH32CS_SNAPPROCESS 0x00000002UL

typedef struct tagDIR_PROCESSENTRY32 {
  DIR_WIN_DWORD dwSize;
  DIR_WIN_DWORD cntUsage;
  DIR_WIN_DWORD th32ProcessID;
  uintptr_t th32DefaultHeapID;
  DIR_WIN_DWORD th32ModuleID;
  DIR_WIN_DWORD cntThreads;
  DIR_WIN_DWORD th32ParentProcessID;
  long pcPriClassBase;
  DIR_WIN_DWORD dwFlags;
  char szExeFile[260];
} DIR_PROCESSENTRY32;

#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) DIR_WIN_HANDLE __stdcall
CreateToolhelp32Snapshot(DIR_WIN_DWORD dwFlags, DIR_WIN_DWORD th32ProcessID);
__declspec(dllimport) DIR_WIN_BOOL __stdcall
Process32First(DIR_WIN_HANDLE hSnapshot, DIR_PROCESSENTRY32 *lppe);
__declspec(dllimport) DIR_WIN_BOOL __stdcall
Process32Next(DIR_WIN_HANDLE hSnapshot, DIR_PROCESSENTRY32 *lppe);
__declspec(dllimport) DIR_WIN_BOOL __stdcall
CloseHandle(DIR_WIN_HANDLE hObject);
#if !defined(__WATCOMC__) && !defined(__DOS__) && !defined(__MSDOS__)
__declspec(dllimport) void *__stdcall
GetModuleHandleA(const char *lpModuleName);
__declspec(dllimport) void *__stdcall GetProcAddress(void *hModule,
                                                     const char *lpProcName);
__declspec(dllimport) void *__stdcall
CreateFileA(const char *lpFileName, unsigned long dwDesiredAccess,
            unsigned long dwShareMode, void *lpSecurityAttributes,
            unsigned long dwCreationDisposition,
            unsigned long dwFlagsAndAttributes, void *hTemplateFile);
#endif
#ifdef __cplusplus
}
#endif

typedef DIR_WIN_HANDLE(__stdcall *dir_snap_fn)(DIR_WIN_DWORD, DIR_WIN_DWORD);
typedef DIR_WIN_BOOL(__stdcall *dir_p32_fn)(DIR_WIN_HANDLE,
                                            DIR_PROCESSENTRY32 *);
typedef DIR_WIN_BOOL(__stdcall *dir_close_h_fn)(DIR_WIN_HANDLE);
typedef void *(__stdcall *dir_get_mod_fn)(const char *);
typedef void *(__stdcall *dir_get_proc_fn)(void *, const char *);
typedef void *(__stdcall *dir_create_file_fn)(const char *, unsigned long,
                                              unsigned long, void *,
                                              unsigned long, unsigned long,
                                              void *);
typedef intptr_t (*dir_get_osfh_fn)(int);
typedef int (*dir_open_osfh_fn)(intptr_t, int);
typedef int (*dir_crt_close_fn)(int);

static DIR_WIN_HANDLE __stdcall default_dir_snap(DIR_WIN_DWORD fl,
                                                 DIR_WIN_DWORD pid) {
  return CreateToolhelp32Snapshot(fl, pid);
}
static DIR_WIN_BOOL __stdcall default_dir_p32first(DIR_WIN_HANDLE h,
                                                   DIR_PROCESSENTRY32 *pe) {
  return Process32First(h, pe);
}
static DIR_WIN_BOOL __stdcall default_dir_p32next(DIR_WIN_HANDLE h,
                                                  DIR_PROCESSENTRY32 *pe) {
  return Process32Next(h, pe);
}
static DIR_WIN_BOOL __stdcall default_dir_close_h(DIR_WIN_HANDLE h) {
  return CloseHandle(h);
}
#if !defined(__WATCOMC__) && !defined(__DOS__) && !defined(__MSDOS__)
static void *__stdcall default_dir_get_mod(const char *name) {
  return GetModuleHandleA(name);
}
static void *__stdcall default_dir_get_proc(void *mod, const char *name) {
  return GetProcAddress(mod, name);
}
static void *__stdcall default_dir_create_file(const char *fn,
                                               unsigned long acc,
                                               unsigned long sh, void *sa,
                                               unsigned long disp,
                                               unsigned long fl, void *tm) {
  return CreateFileA(fn, acc, sh, sa, disp, fl, tm);
}
static intptr_t default_dir_get_osfh(int fd) { return _get_osfhandle(fd); }
static int default_dir_open_osfh(intptr_t h, int fl) {
  return _open_osfhandle(h, fl);
}
static int default_dir_crt_close(int fd) { return _close(fd); }
#endif

static dir_snap_fn g_dir_snap = default_dir_snap;
static dir_p32_fn g_dir_p32first = default_dir_p32first;
static dir_p32_fn g_dir_p32next = default_dir_p32next;
static dir_close_h_fn g_dir_close_h = default_dir_close_h;
#if !defined(__WATCOMC__) && !defined(__DOS__) && !defined(__MSDOS__)
static dir_get_mod_fn g_dir_get_mod = default_dir_get_mod;
static dir_get_proc_fn g_dir_get_proc = default_dir_get_proc;
static dir_create_file_fn g_dir_create_file = default_dir_create_file;
static dir_get_osfh_fn g_dir_get_osfh = default_dir_get_osfh;
static dir_open_osfh_fn g_dir_open_osfh = default_dir_open_osfh;
static dir_crt_close_fn g_dir_crt_close = default_dir_crt_close;
#endif

/**
 * @brief Overrides Win32 and CRT handle hooks for deterministic testing.
 */
void posix_dirent_set_win32_hooks(
    dir_snap_fn snap_fn, dir_p32_fn first_fn, dir_p32_fn next_fn,
    dir_close_h_fn close_h_fn, dir_get_mod_fn mod_fn, dir_get_proc_fn proc_fn,
    dir_create_file_fn cf_fn, dir_get_osfh_fn get_osfh_fn,
    dir_open_osfh_fn open_osfh_fn, dir_crt_close_fn crt_close_fn);
void posix_dirent_set_win32_hooks(
    dir_snap_fn snap_fn, dir_p32_fn first_fn, dir_p32_fn next_fn,
    dir_close_h_fn close_h_fn, dir_get_mod_fn mod_fn, dir_get_proc_fn proc_fn,
    dir_create_file_fn cf_fn, dir_get_osfh_fn get_osfh_fn,
    dir_open_osfh_fn open_osfh_fn, dir_crt_close_fn crt_close_fn) {
  g_dir_snap = (snap_fn != NULL) ? snap_fn : default_dir_snap;
  g_dir_p32first = (first_fn != NULL) ? first_fn : default_dir_p32first;
  g_dir_p32next = (next_fn != NULL) ? next_fn : default_dir_p32next;
  g_dir_close_h = (close_h_fn != NULL) ? close_h_fn : default_dir_close_h;
#if !defined(__WATCOMC__) && !defined(__DOS__) && !defined(__MSDOS__)
  g_dir_get_mod = (mod_fn != NULL) ? mod_fn : default_dir_get_mod;
  g_dir_get_proc = (proc_fn != NULL) ? proc_fn : default_dir_get_proc;
  g_dir_create_file = (cf_fn != NULL) ? cf_fn : default_dir_create_file;
  g_dir_get_osfh = (get_osfh_fn != NULL) ? get_osfh_fn : default_dir_get_osfh;
  g_dir_open_osfh =
      (open_osfh_fn != NULL) ? open_osfh_fn : default_dir_open_osfh;
  g_dir_crt_close =
      (crt_close_fn != NULL) ? crt_close_fn : default_dir_crt_close;
#else
  (void)mod_fn;
  (void)proc_fn;
  (void)cf_fn;
  (void)get_osfh_fn;
  (void)open_osfh_fn;
  (void)crt_close_fn;
#endif
}

/**
 * @brief Resets all posix-dirent allocator and Win32 hooks to defaults.
 */
void posix_dirent_reset_hooks(void);
void posix_dirent_reset_hooks(void) {
  g_dirent_malloc = default_dirent_malloc;
  g_dirent_realloc = default_dirent_realloc;
  g_dir_snap = default_dir_snap;
  g_dir_p32first = default_dir_p32first;
  g_dir_p32next = default_dir_p32next;
  g_dir_close_h = default_dir_close_h;
#if !defined(__WATCOMC__) && !defined(__DOS__) && !defined(__MSDOS__)
  g_dir_get_mod = default_dir_get_mod;
  g_dir_get_proc = default_dir_get_proc;
  g_dir_create_file = default_dir_create_file;
  g_dir_get_osfh = default_dir_get_osfh;
  g_dir_open_osfh = default_dir_open_osfh;
  g_dir_crt_close = default_dir_crt_close;
#endif
}

struct DIR {
  intptr_t handle;
  struct _finddata_t find_data;
  struct dirent entry;
  int first_read;
  long offset;
  char *name;
  int is_proc;
  DIR_WIN_HANDLE snap_handle;
  DIR_PROCESSENTRY32 pe32;
};

static unsigned char get_d_type(unsigned attrib) {
  return (attrib & _A_SUBDIR) ? DT_DIR : DT_REG;
}

static int is_proc_path(const char *name) {
  static const char bs_proc[6] = {92, 'p', 'r', 'o', 'c', 0};
  static const char bs_procs[7] = {92, 'p', 'r', 'o', 'c', 92, 0};
  return (strcmp(name, "/proc") == 0 || strcmp(name, "/proc/") == 0 ||
          strcmp(name, bs_proc) == 0 || strcmp(name, bs_procs) == 0);
}

DIR *opendir(const char *name) {
  DIR *dirp;
  char *search_path;
  size_t name_len;

  if (!name || name[0] == '\0') {
    errno = ENOENT;
    return NULL;
  }

  name_len = strlen(name);
  dirp = (DIR *)g_dirent_malloc(sizeof(DIR));
  if (!dirp) {
    errno = ENOMEM;
    return NULL;
  }
  memset(dirp, 0, sizeof(DIR));
  dirp->handle = -1;
  dirp->snap_handle = DIR_INVALID_HANDLE_VALUE;

  if (is_proc_path(name)) {
    dirp->is_proc = 1;
    dirp->snap_handle = g_dir_snap(DIR_TH32CS_SNAPPROCESS, 0);
    if (dirp->snap_handle == DIR_INVALID_HANDLE_VALUE) {
      free(dirp);
      errno = ENOENT;
      return NULL;
    }
    dirp->pe32.dwSize = sizeof(DIR_PROCESSENTRY32);
    if (!g_dir_p32first(dirp->snap_handle, &dirp->pe32)) {
      g_dir_close_h(dirp->snap_handle);
      free(dirp);
      errno = ENOENT;
      return NULL;
    }
    dirp->first_read = 1;
    dirp->offset = 0;
    dirp->name = (char *)g_dirent_malloc(name_len + 1);
    if (!dirp->name) {
      g_dir_close_h(dirp->snap_handle);
      free(dirp);
      errno = ENOMEM;
      return NULL;
    }
#if defined(_MSC_VER)
    strcpy_s(dirp->name, name_len + 1, name);
#else
    strcpy(dirp->name, name);
#endif
    return dirp;
  }

  search_path = (char *)g_dirent_malloc(name_len + 3);
  if (!search_path) {
    free(dirp);
    errno = ENOMEM;
    return NULL;
  }

#if defined(_MSC_VER)
  strcpy_s(search_path, name_len + 3, name);
#else
  strcpy(search_path, name);
#endif

  if (search_path[name_len - 1] != '/' &&
      search_path[name_len - 1] != (char)92) {
    static const char bs_star[3] = {92, '*', 0};
#if defined(_MSC_VER)
    strcat_s(search_path, name_len + 3, bs_star);
#else
    strcat(search_path, bs_star);
#endif
  } else {
#if defined(_MSC_VER)
    strcat_s(search_path, name_len + 3, "*");
#else
    strcat(search_path, "*");
#endif
  }

  dirp->handle = _findfirst(search_path, &dirp->find_data);
  free(search_path);

  if (dirp->handle == -1) {
    free(dirp);
    errno = ENOENT;
    return NULL;
  }

  dirp->first_read = 1;
  dirp->offset = 0;

  dirp->name = (char *)g_dirent_malloc(name_len + 1);
  if (!dirp->name) {
    _findclose(dirp->handle);
    free(dirp);
    errno = ENOMEM;
    return NULL;
  }

#if defined(_MSC_VER)
  strcpy_s(dirp->name, name_len + 1, name);
#else
  strcpy(dirp->name, name);
#endif

  return dirp;
}

struct dirent *readdir(DIR *dirp) {
  if (!dirp) {
    errno = EBADF;
    return NULL;
  }

  if (dirp->is_proc) {
    if (!dirp->first_read) {
      if (!g_dir_p32next(dirp->snap_handle, &dirp->pe32)) {
        return NULL;
      }
    } else {
      dirp->first_read = 0;
    }
    while (dirp->pe32.th32ProcessID == 0) {
      if (!g_dir_p32next(dirp->snap_handle, &dirp->pe32)) {
        return NULL;
      }
    }
    dirp->entry.d_ino = 1;
    dirp->entry.d_off = dirp->offset++;
    dirp->entry.d_reclen = sizeof(struct dirent);
    dirp->entry.d_type = DT_DIR;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    sprintf_s(dirp->entry.d_name, sizeof(dirp->entry.d_name), "%lu",
              (unsigned long)dirp->pe32.th32ProcessID);
#else
    sprintf(dirp->entry.d_name, "%lu", (unsigned long)dirp->pe32.th32ProcessID);
#endif
    dirp->entry.d_namlen = (unsigned short)strlen(dirp->entry.d_name);
    return &dirp->entry;
  }

  if (!dirp->first_read) {
    if (_findnext(dirp->handle, &dirp->find_data) != 0) {
      return NULL;
    }
  } else {
    dirp->first_read = 0;
  }

  dirp->entry.d_ino = 1;
  dirp->entry.d_off = dirp->offset++;
  dirp->entry.d_reclen = sizeof(struct dirent);
  dirp->entry.d_type = get_d_type(dirp->find_data.attrib);

#if defined(_MSC_VER) && _MSC_VER >= 1400
  strncpy_s(dirp->entry.d_name, sizeof(dirp->entry.d_name),
            dirp->find_data.name, _TRUNCATE);
#else
  strncpy(dirp->entry.d_name, dirp->find_data.name,
          sizeof(dirp->entry.d_name) - 1);
  dirp->entry.d_name[sizeof(dirp->entry.d_name) - 1] = '\0';
#endif

  dirp->entry.d_namlen = (unsigned short)strlen(dirp->entry.d_name);
  return &dirp->entry;
}

int closedir(DIR *dirp) {
  if (!dirp) {
    errno = EBADF;
    return -1;
  }

  if (dirp->is_proc) {
    if (dirp->snap_handle != DIR_INVALID_HANDLE_VALUE) {
      g_dir_close_h(dirp->snap_handle);
    }
  } else if (dirp->handle != -1) {
    _findclose(dirp->handle);
  }

  free(dirp->name);
  free(dirp);
  return 0;
}

void rewinddir(DIR *dirp) {
  char *search_path;
  size_t name_len;

  if (!dirp) {
    return;
  }

  if (dirp->is_proc) {
    if (dirp->snap_handle != DIR_INVALID_HANDLE_VALUE) {
      g_dir_close_h(dirp->snap_handle);
    }
    dirp->snap_handle = g_dir_snap(DIR_TH32CS_SNAPPROCESS, 0);
    if (dirp->snap_handle != DIR_INVALID_HANDLE_VALUE) {
      dirp->pe32.dwSize = sizeof(DIR_PROCESSENTRY32);
      g_dir_p32first(dirp->snap_handle, &dirp->pe32);
    }
    dirp->first_read = 1;
    dirp->offset = 0;
    return;
  }

  if (dirp->handle != -1) {
    _findclose(dirp->handle);
    dirp->handle = -1;
  }

  name_len = strlen(dirp->name);
  search_path = (char *)g_dirent_malloc(name_len + 3);
  if (!search_path) {
    return;
  }

#if defined(_MSC_VER)
  strcpy_s(search_path, name_len + 3, dirp->name);
#else
  strcpy(search_path, dirp->name);
#endif

  if (search_path[name_len - 1] != '/' &&
      search_path[name_len - 1] != (char)92) {
    static const char bs_star[3] = {92, '*', 0};
#if defined(_MSC_VER)
    strcat_s(search_path, name_len + 3, bs_star);
#else
    strcat(search_path, bs_star);
#endif
  } else {
#if defined(_MSC_VER)
    strcat_s(search_path, name_len + 3, "*");
#else
    strcat(search_path, "*");
#endif
  }

  dirp->handle = _findfirst(search_path, &dirp->find_data);
  free(search_path);

  dirp->first_read = 1;
  dirp->offset = 0;
}

long telldir(DIR *dirp) {
  if (!dirp) {
    return -1;
  }
  return dirp->offset;
}

void seekdir(DIR *dirp, long loc) {
  if (!dirp) {
    return;
  }

  rewinddir(dirp);
  while (dirp->offset < loc) {
    if (readdir(dirp) == NULL) {
      break;
    }
  }
}

static int scandir_stream(DIR *d, struct dirent ***namelist,
                          int (*filter)(const struct dirent *),
                          int (*compar)(const struct dirent **,
                                        const struct dirent **)) {
  struct dirent *entry;
  struct dirent **names;
  size_t count = 0;
  size_t capacity = 4;

  names = (struct dirent **)g_dirent_malloc(capacity * sizeof(struct dirent *));
  if (!names) {
    closedir(d);
    return -1;
  }

  while ((entry = readdir(d)) != NULL) {
    if (!filter || filter(entry)) {
      struct dirent *copy =
          (struct dirent *)g_dirent_malloc(sizeof(struct dirent));
      if (!copy) {
        while (count > 0) {
          free(names[--count]);
        }
        free(names);
        closedir(d);
        return -1;
      }
      memcpy(copy, entry, sizeof(struct dirent));

      if (count >= capacity) {
        size_t new_cap = capacity * 2;
        struct dirent **new_names = (struct dirent **)g_dirent_realloc(
            names, new_cap * sizeof(struct dirent *));
        if (!new_names) {
          free(copy);
          while (count > 0) {
            free(names[--count]);
          }
          free(names);
          closedir(d);
          return -1;
        }
        names = new_names;
        capacity = new_cap;
      }
      names[count++] = copy;
    }
  }

  closedir(d);

  if (compar) {
    qsort(names, count, sizeof(struct dirent *),
          (int (*)(const void *, const void *))compar);
  }

  *namelist = names;
  return (int)count;
}

int scandir(const char *dirp, struct dirent ***namelist,
            int (*filter)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **)) {
  DIR *d;
  if (!dirp || !namelist) {
    return -1;
  }
  d = opendir(dirp);
  if (!d) {
    return -1;
  }
  return scandir_stream(d, namelist, filter, compar);
}

int fdscandir(int dirfd, struct dirent ***namelist,
              int (*filter)(const struct dirent *),
              int (*compar)(const struct dirent **, const struct dirent **)) {
  DIR *d;
  if (!namelist) {
    errno = EINVAL;
    return -1;
  }
  d = fdopendir(dirfd);
  if (!d) {
    return -1;
  }
  return scandir_stream(d, namelist, filter, compar);
}

int alphasort(const struct dirent **a, const struct dirent **b) {
  return strcmp((*a)->d_name, (*b)->d_name);
}

DIR *fdopendir(int fd) {
#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)
  (void)fd;
  errno = ENOSYS;
  return NULL;
#else
  char path_buf[4096];
  intptr_t hFile;
  void *kernel32;
  unsigned long(__stdcall * pGetFinalPathNameByHandleA)(
      void *, char *, unsigned long, unsigned long) = NULL;
  unsigned long len;
  DIR *d;
  if (fd < 0) {
    errno = EBADF;
    return NULL;
  }
  hFile = g_dir_get_osfh(fd);
  if (hFile == (intptr_t)-1 || hFile == (intptr_t)-2) {
    errno = EBADF;
    return NULL;
  }
  kernel32 = g_dir_get_mod("kernel32.dll");
  if (!kernel32) {
    errno = ENOSYS;
    return NULL;
  }
  *(void **)(&pGetFinalPathNameByHandleA) =
      g_dir_get_proc(kernel32, "GetFinalPathNameByHandleA");
  if (!pGetFinalPathNameByHandleA) {
    errno = ENOSYS;
    return NULL;
  }
  len = pGetFinalPathNameByHandleA((void *)hFile, path_buf,
                                   (unsigned long)sizeof(path_buf), 0);
  if (len == 0 || len >= sizeof(path_buf)) {
    errno = EBADF;
    return NULL;
  }
  if (len >= 4 && path_buf[0] == (char)92 && path_buf[1] == (char)92 &&
      path_buf[2] == '?' && path_buf[3] == (char)92) {
    memmove(path_buf, path_buf + 4, len - 3);
  }
  d = opendir(path_buf);
  if (d != NULL) {
    g_dir_crt_close(fd);
  }
  return d;
#endif
}

int dirfd(DIR *dirp) {
#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)
  (void)dirp;
  errno = ENOSYS;
  return -1;
#else
  void *hDir;
  if (!dirp) {
    errno = EINVAL;
    return -1;
  }
  hDir = g_dir_create_file(dirp->name, 0x80000000UL, 7UL, NULL, 3UL,
                           0x02000000UL, NULL);
  if (hDir == (void *)(intptr_t)-1) {
    errno = ENOENT;
    return -1;
  }
  return g_dir_open_osfh((intptr_t)hDir, 0);
#endif
}

#else /* !_WIN32 */

#undef opendir
#undef readdir
#undef closedir
#undef rewinddir

#if defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
typedef int (*dirent_sysctl_fn)(int *, u_int, void *, size_t *, void *, size_t);
static dirent_sysctl_fn g_dirent_sysctl = sysctl;

/**
 * @brief Overrides the sysctl hook on macOS for deterministic /proc testing.
 * @param[in] sysctl_fn Replacement sysctl function, or NULL for default.
 */
void posix_dirent_set_sysctl_hook(dirent_sysctl_fn sysctl_fn);
void posix_dirent_set_sysctl_hook(dirent_sysctl_fn sysctl_fn) {
  g_dirent_sysctl = (sysctl_fn != NULL) ? sysctl_fn : sysctl;
}
#endif

/**
 * @brief Resets all posix-dirent allocator and platform hooks to defaults.
 */
void posix_dirent_reset_hooks(void);
void posix_dirent_reset_hooks(void) {
#if !defined(__linux__) && !defined(__CYGWIN__)
  g_dirent_malloc = default_dirent_malloc;
  g_dirent_calloc = default_dirent_calloc;
#endif
#if defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
  g_dirent_sysctl = sysctl;
#endif
}

#if !defined(__linux__) && !defined(__CYGWIN__)
#define POSIX_PROC_MAGIC 0x50524F43UL

struct posix_proc_dir {
  unsigned long magic;
  int index;
  int count;
  int *pids;
  struct dirent entry;
};

static struct posix_proc_dir *g_active_proc_dirs[16] = {NULL};

static int find_proc_dir_slot(DIR *dirp) {
#if defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
  return ((DIR *)g_active_proc_dirs[0] == dirp) ? 0 : -1;
#else
  int i;
  for (i = 0; i < 16; i++) {
    if ((DIR *)g_active_proc_dirs[i] == dirp) {
      return i;
    }
  }
  return -1;
#endif
}
#endif /* !__linux__ */

DIR *posix_opendir(const char *name) {
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
  if (!name) {
    errno = EINVAL;
    return NULL;
  }
#endif
#if !defined(__linux__) && !defined(__CYGWIN__)
#if defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
  if (strcmp(name, "/proc") == 0) {
    struct posix_proc_dir *pd = (struct posix_proc_dir *)g_dirent_calloc(
        1, sizeof(struct posix_proc_dir));
    pd->magic = POSIX_PROC_MAGIC;
    pd->index = 0;
    pd->pids = (int *)g_dirent_malloc(sizeof(int));
    pd->pids[0] = 1;
    pd->count = 1;
    g_active_proc_dirs[0] = pd;
    return (DIR *)pd;
  }
#else
  if (strcmp(name, "/proc") == 0 || strcmp(name, "/proc/") == 0) {
    struct posix_proc_dir *pd;
    int slot = -1;
    int i;
    for (i = 0; i < 16; i++) {
      if (g_active_proc_dirs[i] == NULL) {
        slot = i;
        break;
      }
    }
    if (slot >= 0) {
      pd = (struct posix_proc_dir *)g_dirent_calloc(
          1, sizeof(struct posix_proc_dir));
      if (pd != NULL) {
        pd->magic = POSIX_PROC_MAGIC;
        pd->index = 0;
        pd->count = 0;
        pd->pids = NULL;
#if defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
        {
          int mib[4];
          size_t len = 0;
          mib[0] = CTL_KERN;
          mib[1] = KERN_PROC;
          mib[2] = KERN_PROC_ALL;
          mib[3] = 0;
          if (g_dirent_sysctl(mib, 4, NULL, &len, NULL, 0) == 0 && len > 0) {
            struct kinfo_proc *kp = (struct kinfo_proc *)g_dirent_malloc(len);
            if (kp != NULL) {
              if (g_dirent_sysctl(mib, 4, kp, &len, NULL, 0) == 0) {
                size_t n = len / sizeof(struct kinfo_proc);
                pd->pids = (int *)g_dirent_malloc((n + 1) * sizeof(int));
                if (pd->pids != NULL) {
                  size_t j;
                  for (j = 0; j < n; j++) {
                    if (kp[j].kp_proc.p_pid > 0) {
                      pd->pids[pd->count++] = (int)kp[j].kp_proc.p_pid;
                    }
                  }
                }
              }
              free(kp);
            }
          }
        }
#endif
        if (pd->pids == NULL) {
          pd->pids = (int *)g_dirent_malloc(sizeof(int));
          if (pd->pids != NULL) {
            pd->pids[0] = 1;
            pd->count = 1;
          }
        }
        g_active_proc_dirs[slot] = pd;
        return (DIR *)pd;
      }
    }
    errno = ENOMEM;
    return NULL;
  }
#endif
#endif /* !__linux__ */
#if defined(__WATCOMC__) && (defined(__DOS__) || defined(__MSDOS__))
  if (_osmajor < 2) {
    errno = ENOSYS;
    return NULL;
  }
  {
    DIR *d_res = opendir(name);
    if (d_res == NULL && strcmp(name, ".") == 0) {
      int slot = -1;
      int i;
      for (i = 0; i < 16; i++) {
        if (g_active_proc_dirs[i] == NULL) {
          slot = i;
          break;
        }
      }
      if (slot >= 0) {
        struct posix_proc_dir *pd = (struct posix_proc_dir *)g_dirent_calloc(
            1, sizeof(struct posix_proc_dir));
        if (pd != NULL) {
          pd->magic = POSIX_PROC_MAGIC;
          pd->index = 0;
          pd->count = 0;
          pd->pids = NULL;
          g_active_proc_dirs[slot] = pd;
          return (DIR *)pd;
        }
      }
    }
    return d_res;
  }
#else
  return opendir(name);
#endif
}

struct dirent *posix_readdir(DIR *dirp) {
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
  if (!dirp) {
    errno = EBADF;
    return NULL;
  }
#endif
#if !defined(__linux__) && !defined(__CYGWIN__)
  if (find_proc_dir_slot(dirp) >= 0) {
    struct posix_proc_dir *pd = (struct posix_proc_dir *)dirp;
    if (pd->index == 0) {
      pd->index++;
      memset(&pd->entry, 0, sizeof(pd->entry));
      pd->entry.d_name[0] = '.';
      pd->entry.d_name[1] = '\0';
      return &pd->entry;
    }
    if (pd->index == 1) {
      pd->index++;
      memset(&pd->entry, 0, sizeof(pd->entry));
      pd->entry.d_name[0] = '.';
      pd->entry.d_name[1] = '.';
      pd->entry.d_name[2] = '\0';
      return &pd->entry;
    }
    if (pd->index - 2 < pd->count) {
      int pid = pd->pids[pd->index - 2];
      pd->index++;
      memset(&pd->entry, 0, sizeof(pd->entry));
      (sprintf)(pd->entry.d_name, "%d", pid);
      return &pd->entry;
    }
    return NULL;
  }
#endif
  return readdir(dirp);
}

int posix_closedir(DIR *dirp) {
#if !defined(__linux__) && !defined(__CYGWIN__)
  int slot;
#endif
#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
  if (!dirp) {
    errno = EBADF;
    return -1;
  }
#endif
#if !defined(__linux__) && !defined(__CYGWIN__)
  slot = find_proc_dir_slot(dirp);
  if (slot >= 0) {
    struct posix_proc_dir *pd = (struct posix_proc_dir *)dirp;
    g_active_proc_dirs[slot] = NULL;
    if (pd->pids != NULL) {
      free(pd->pids);
    }
    free(pd);
    return 0;
  }
#endif
  return closedir(dirp);
}

#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
void posix_rewinddir(DIR *dirp) {
  if (!dirp) {
    return;
  }
#if !defined(__linux__) && !defined(__CYGWIN__)
  if (find_proc_dir_slot(dirp) >= 0) {
    struct posix_proc_dir *pd = (struct posix_proc_dir *)dirp;
    pd->index = 0;
    return;
  }
#endif
  rewinddir(dirp);
}
#endif

#if defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__)
int scandir(const char *dirp, struct dirent ***namelist,
            int (*filter)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **)) {
  DIR *d;
  struct dirent *entry;
  struct dirent **names = NULL;
  size_t count = 0;
  size_t capacity = 10;

  if (!dirp || !namelist) {
    return -1;
  }

  d = opendir(dirp);
  if (!d) {
    return -1;
  }

  names = (struct dirent **)malloc(capacity * sizeof(struct dirent *));
  if (!names) {
    closedir(d);
    return -1;
  }

  while ((entry = readdir(d)) != NULL) {
    if (!filter || filter(entry)) {
      struct dirent *copy = (struct dirent *)malloc(sizeof(struct dirent));
      if (!copy) {
        while (count > 0) {
          free(names[--count]);
        }
        free(names);
        closedir(d);
        return -1;
      }
      memcpy(copy, entry, sizeof(struct dirent));

      if (count >= capacity) {
        size_t new_cap = capacity * 2;
        struct dirent **new_names =
            (struct dirent **)realloc(names, new_cap * sizeof(struct dirent *));
        if (!new_names) {
          free(copy);
          while (count > 0) {
            free(names[--count]);
          }
          free(names);
          closedir(d);
          return -1;
        }
        names = new_names;
        capacity = new_cap;
      }
      names[count++] = copy;
    }
  }

  closedir(d);

  if (compar) {
    qsort(names, count, sizeof(struct dirent *),
          (int (*)(const void *, const void *))compar);
  }

  *namelist = names;
  return (int)count;
}

int alphasort(const struct dirent **a, const struct dirent **b) {
  return strcmp((*a)->d_name, (*b)->d_name);
}

DIR *fdopendir(int fd) {
  if (fd < 0) {
    errno = EBADF;
    return NULL;
  }
  return opendir(".");
}

int dirfd(DIR *dirp) {
  if (!dirp) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}
#endif
#endif /* _WIN32 */

#ifndef AUTO_WIN_MSVC_MEGA_LIBRARY
/**
 * @brief Retrieves information on posix-dirent module availability.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_dirent_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}
#endif

typedef int make_iso_compilers_happy_tu_posix_dirent;
