/* posix-dirent.c - Strict C89 Implementation */
/* clang-format off */
#include "posix-dirent.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(_WIN32) && defined(__APPLE__) && !defined(__STRICT_ANSI__) &&     \
    defined(_DARWIN_C_SOURCE)
#include <sys/sysctl.h>
#endif


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
/* MSVC 2005+ */
#include <stddef.h>
#else
/* Older MSVC */
#ifndef intptr_t
typedef long intptr_t;
#endif
#endif
#endif
/* clang-format on */

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
#ifdef __cplusplus
}
#endif

struct DIR {
  intptr_t handle;
  struct _finddata_t find_data;
  struct dirent entry;
  int first_read;
  long offset;
  char *name; /* Store original search name for rewinddir */
  int is_proc;
  DIR_WIN_HANDLE snap_handle;
  DIR_PROCESSENTRY32 pe32;
};

/* Helper function to determine d_type. Returns int (exit code). */
static int get_d_type(unsigned attrib, unsigned char *out_type) {
  if (!out_type)
    return -1;
  if (attrib & _A_SUBDIR) {
    *out_type = DT_DIR;
  } else {
    *out_type = DT_REG;
  }
  return 0;
}

static int is_proc_path(const char *name) {
  if (!name)
    return 0;
  return (strcmp(name, "/proc") == 0 || strcmp(name, "/proc/") == 0 ||
          strcmp(name, "\\proc") == 0 || strcmp(name, "\\proc\\") == 0);
}

DIR *opendir(const char *name) {
  DIR *dirp;
  char *search_path;
  size_t name_len;

  if (!name) {
    errno = ENOENT;
    return NULL;
  }

  name_len = strlen(name);
  dirp = (DIR *)malloc(sizeof(DIR));
  if (!dirp) {
    errno = ENOMEM;
    return NULL;
  }
  memset(dirp, 0, sizeof(DIR));
  dirp->handle = -1;
  dirp->snap_handle = DIR_INVALID_HANDLE_VALUE;

  if (is_proc_path(name)) {
    dirp->is_proc = 1;
    dirp->snap_handle = CreateToolhelp32Snapshot(DIR_TH32CS_SNAPPROCESS, 0);
    if (dirp->snap_handle == DIR_INVALID_HANDLE_VALUE) {
      free(dirp);
      errno = ENOENT;
      return NULL;
    }
    dirp->pe32.dwSize = sizeof(DIR_PROCESSENTRY32);
    if (!Process32First(dirp->snap_handle, &dirp->pe32)) {
      CloseHandle(dirp->snap_handle);
      free(dirp);
      errno = ENOENT;
      return NULL;
    }
    dirp->first_read = 1;
    dirp->offset = 0;
    dirp->name = (char *)malloc(name_len + 1);
    if (!dirp->name) {
      CloseHandle(dirp->snap_handle);
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

  search_path = (char *)malloc(name_len + 3);
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

  if (name_len > 0 && search_path[name_len - 1] != '/' &&
      search_path[name_len - 1] != '\\') {
#if defined(_MSC_VER)
    strcat_s(search_path, name_len + 3, "\\*");
#else
    strcat(search_path, "\\*");
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

  dirp->name = (char *)malloc(name_len + 1);
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
      if (!Process32Next(dirp->snap_handle, &dirp->pe32)) {
        return NULL;
      }
    } else {
      dirp->first_read = 0;
    }
    while (dirp->pe32.th32ProcessID == 0) {
      if (!Process32Next(dirp->snap_handle, &dirp->pe32)) {
        return NULL;
      }
    }
    dirp->entry.d_ino = 0;
    dirp->entry.d_off = dirp->offset++;
    dirp->entry.d_reclen = 0;
    dirp->entry.d_type = DT_DIR;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    sprintf_s(dirp->entry.d_name, sizeof(dirp->entry.d_name), "%lu",
              (unsigned long)dirp->pe32.th32ProcessID);
#else
    sprintf(dirp->entry.d_name, "%lu", (unsigned long)dirp->pe32.th32ProcessID);
#endif
    return &dirp->entry;
  }

  if (!dirp->first_read) {
    if (_findnext(dirp->handle, &dirp->find_data) != 0) {
      return NULL;
    }
  } else {
    dirp->first_read = 0;
  }

  dirp->entry.d_ino = 0;
  dirp->entry.d_off = dirp->offset++;
  dirp->entry.d_reclen = 0;

  get_d_type(dirp->find_data.attrib, &dirp->entry.d_type);

#if defined(_MSC_VER) && _MSC_VER >= 1400
  strncpy_s(dirp->entry.d_name, sizeof(dirp->entry.d_name),
            dirp->find_data.name, _TRUNCATE);
#else
  strncpy(dirp->entry.d_name, dirp->find_data.name,
          sizeof(dirp->entry.d_name) - 1);
  dirp->entry.d_name[sizeof(dirp->entry.d_name) - 1] = '\0';
#endif

  return &dirp->entry;
}

/** \brief closedir function. */
int closedir(DIR *dirp) {
  if (!dirp) {
    errno = EBADF;
    return -1;
  }

  if (dirp->is_proc) {
    if (dirp->snap_handle != DIR_INVALID_HANDLE_VALUE) {
      CloseHandle(dirp->snap_handle);
    }
  } else if (dirp->handle != -1) {
    _findclose(dirp->handle);
  }

  if (dirp->name) {
    free(dirp->name);
  }

  free(dirp);
  return 0;
}

/** \brief rewinddir function. */
void rewinddir(DIR *dirp) {
  char *search_path;
  size_t name_len;

  if (!dirp || !dirp->name)
    return;

  if (dirp->is_proc) {
    if (dirp->snap_handle != DIR_INVALID_HANDLE_VALUE) {
      CloseHandle(dirp->snap_handle);
    }
    dirp->snap_handle = CreateToolhelp32Snapshot(DIR_TH32CS_SNAPPROCESS, 0);
    if (dirp->snap_handle != DIR_INVALID_HANDLE_VALUE) {
      dirp->pe32.dwSize = sizeof(DIR_PROCESSENTRY32);
      Process32First(dirp->snap_handle, &dirp->pe32);
    }
    dirp->first_read = 1;
    dirp->offset = 0;
    return;
  }

  if (dirp->handle != -1) {
    _findclose(dirp->handle);
  }

  name_len = strlen(dirp->name);
  search_path = (char *)malloc(name_len + 3);
  if (!search_path)
    return;

#if defined(_MSC_VER)
  strcpy_s(search_path, name_len + 3, dirp->name);
#else
  strcpy(search_path, dirp->name);
#endif

  if (name_len > 0 && search_path[name_len - 1] != '/' &&
      search_path[name_len - 1] != '\\') {
#if defined(_MSC_VER)
    strcat_s(search_path, name_len + 3, "\\*");
#else
    strcat(search_path, "\\*");
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

/** \brief telldir function. */
long telldir(DIR *dirp) {
  if (!dirp)
    return -1;
  return dirp->offset;
}

/** \brief seekdir function. */
void seekdir(DIR *dirp, long loc) {
  if (!dirp)
    return;

  rewinddir(dirp);
  while (dirp->offset < loc) {
    if (readdir(dirp) == NULL)
      break;
  }
}

/** \brief scandir function. */
int scandir(const char *dirp, struct dirent ***namelist,
            int (*filter)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **)) {
  DIR *d;
  struct dirent *entry;
  struct dirent **names = NULL;
  size_t count = 0;
  size_t capacity = 10;

  if (!dirp || !namelist)
    return -1;

  d = opendir(dirp);
  if (!d)
    return -1;

  names = (struct dirent **)malloc(capacity * sizeof(struct dirent *));
  if (!names) {
    closedir(d);
    return -1;
  }

  while ((entry = readdir(d)) != NULL) {
    if (!filter || filter(entry)) {
      struct dirent *copy = (struct dirent *)malloc(sizeof(struct dirent));
      if (!copy) {
        while (count > 0)
          free(names[--count]);
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
          while (count > 0)
            free(names[--count]);
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

/** \brief alphasort function. */
int alphasort(const struct dirent **a, const struct dirent **b) {
  return strcmp((*a)->d_name, (*b)->d_name);
}
#else
#undef opendir
#undef readdir
#undef closedir
#undef rewinddir

#define POSIX_PROC_MAGIC 0x50524F43UL

struct posix_proc_dir {
  unsigned long magic;
  int index;
  int count;
  int *pids;
  struct dirent entry;
};

static struct posix_proc_dir *g_active_proc_dirs[16] = {NULL};

static int is_proc_dir_handle(DIR *dirp) {
  int i;
  if (!dirp) {
    return 0;
  }
  for (i = 0; i < 16; i++) {
    if ((DIR *)g_active_proc_dirs[i] == dirp) {
      return 1;
    }
  }
  return 0;
}

/**
 * @brief Opens a directory stream, supporting virtual /proc iteration on
 * non-Linux POSIX platforms.
 * @param[in] name The name of the directory to open.
 * @return A pointer to the directory stream, or NULL if an error occurred.
 */
DIR *posix_opendir(const char *name) {
  if (!name) {
    errno = EINVAL;
    return NULL;
  }
#if !defined(__linux__)
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
      pd = (struct posix_proc_dir *)calloc(1, sizeof(struct posix_proc_dir));
      if (pd != NULL) {
        pd->magic = POSIX_PROC_MAGIC;
        pd->index = 0;
        pd->count = 0;
        pd->pids = NULL;
#if defined(__APPLE__) && !defined(__STRICT_ANSI__) && defined(_DARWIN_C_SOURCE)
        {
          int mib[4];
          size_t len = 0;
          mib[0] = CTL_KERN;
          mib[1] = KERN_PROC;
          mib[2] = KERN_PROC_ALL;
          mib[3] = 0;
          if (sysctl(mib, 4, NULL, &len, NULL, 0) == 0 && len > 0) {
            struct kinfo_proc *kp = (struct kinfo_proc *)malloc(len);
            if (kp != NULL) {
              if (sysctl(mib, 4, kp, &len, NULL, 0) == 0) {
                size_t n = len / sizeof(struct kinfo_proc);
                pd->pids = (int *)malloc((n + 1) * sizeof(int));
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
          pd->pids = (int *)malloc(sizeof(int));
          if (pd->pids != NULL) {
            pd->pids[0] = 1;
            pd->count = 1;
          }
        }
        g_active_proc_dirs[slot] = pd;
        return (DIR *)pd;
      }
    }
  }
#endif
  return opendir(name);
}

/**
 * @brief Reads a directory entry, supporting virtual /proc iteration on
 * non-Linux POSIX platforms.
 * @param[in,out] dirp The directory stream.
 * @return A pointer to the next directory entry, or NULL at EOF or error.
 */
struct dirent *posix_readdir(DIR *dirp) {
  if (!dirp) {
    errno = EBADF;
    return NULL;
  }
  if (is_proc_dir_handle(dirp)) {
    struct posix_proc_dir *pd = (struct posix_proc_dir *)dirp;
    if (pd->index == 0) {
      pd->index++;
      memset(&pd->entry, 0, sizeof(pd->entry));
      strcpy(pd->entry.d_name, ".");
      return &pd->entry;
    }
    if (pd->index == 1) {
      pd->index++;
      memset(&pd->entry, 0, sizeof(pd->entry));
      strcpy(pd->entry.d_name, "..");
      return &pd->entry;
    }
    if (pd->index - 2 < pd->count) {
      int pid = pd->pids[pd->index - 2];
      pd->index++;
      memset(&pd->entry, 0, sizeof(pd->entry));
      sprintf(pd->entry.d_name, "%d", pid);
      return &pd->entry;
    }
    return NULL;
  }
  return readdir(dirp);
}

/**
 * @brief Closes a directory stream, supporting virtual /proc iteration on
 * non-Linux POSIX platforms.
 * @param[in,out] dirp The directory stream.
 * @return 0 on success, or -1 on error.
 */
int posix_closedir(DIR *dirp) {
  if (!dirp) {
    errno = EBADF;
    return -1;
  }
  if (is_proc_dir_handle(dirp)) {
    struct posix_proc_dir *pd = (struct posix_proc_dir *)dirp;
    int i;
    for (i = 0; i < 16; i++) {
      if (g_active_proc_dirs[i] == pd) {
        g_active_proc_dirs[i] = NULL;
        break;
      }
    }
    if (pd->pids != NULL) {
      free(pd->pids);
    }
    free(pd);
    return 0;
  }
  return closedir(dirp);
}

/**
 * @brief Resets a directory stream position, supporting virtual /proc
 * iteration on non-Linux POSIX platforms.
 * @param[in,out] dirp The directory stream.
 */
void posix_rewinddir(DIR *dirp) {
  if (!dirp) {
    return;
  }
  if (is_proc_dir_handle(dirp)) {
    struct posix_proc_dir *pd = (struct posix_proc_dir *)dirp;
    pd->index = 0;
    return;
  }
  rewinddir(dirp);
}
#endif /* _WIN32 */

/**
 * @brief Retrieves information on posix-dirent module availability.
 */
auto_win_msvc_error_t posix_dirent_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

typedef int make_iso_compilers_happy_tu_posix_dirent;
