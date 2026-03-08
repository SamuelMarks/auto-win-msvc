/* posix-dirent.c - Strict C89 Implementation */
#include "posix-dirent.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <io.h>
#include <sys/types.h>

#if defined(__MINGW32__) || defined(__MINGW64__)
#include <stdint.h>
#elif defined(_MSC_VER) && _MSC_VER >= 1400
/* MSVC 2005+ */
#include <stddef.h>
#else
/* Older MSVC */
#ifndef intptr_t
typedef long intptr_t;
#endif
#endif

/* Provide NUM_FORMAT for C89 / MSVC variations, though not strictly needed here for standard strings */
#if defined(_MSC_VER)
#define NUM_FORMAT "%I64d"
#else
#define NUM_FORMAT "%lld"
#endif

struct DIR {
    intptr_t handle;
    struct _finddata_t find_data;
    struct dirent entry;
    int first_read;
    long offset;
    char *name; /* Store original search name for rewinddir */
};

/* Helper function to determine d_type. Returns int (exit code). */
static int get_d_type(unsigned attrib, unsigned char *out_type) {
    if (!out_type) return -1;
    if (attrib & _A_SUBDIR) {
        *out_type = DT_DIR;
    } else {
        *out_type = DT_REG;
    }
    return 0;
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

    search_path = (char *)malloc(name_len + 3);
    if (!search_path) {
        free(dirp);
        errno = ENOMEM;
        return NULL;
    }

#if defined(_MSC_VER) && _MSC_VER >= 1400
    strcpy_s(search_path, name_len + 3, name);
#else
    strcpy(search_path, name);
#endif

    if (name_len > 0 && search_path[name_len - 1] != '/' && search_path[name_len - 1] != '\\') {
#if defined(_MSC_VER) && _MSC_VER >= 1400
        strcat_s(search_path, name_len + 3, "\\*");
#else
        strcat(search_path, "\\*");
#endif
    } else {
#if defined(_MSC_VER) && _MSC_VER >= 1400
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

#if defined(_MSC_VER) && _MSC_VER >= 1400
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
    /* Handle _TRUNCATE safely for C89 or older if needed, _TRUNCATE is available with secure CRT */
    strncpy_s(dirp->entry.d_name, sizeof(dirp->entry.d_name), dirp->find_data.name, _TRUNCATE);
#else
    strncpy(dirp->entry.d_name, dirp->find_data.name, sizeof(dirp->entry.d_name) - 1);
    dirp->entry.d_name[sizeof(dirp->entry.d_name) - 1] = '\0';
#endif

    return &dirp->entry;
}

int closedir(DIR *dirp) {
    if (!dirp) {
        errno = EBADF;
        return -1;
    }

    if (dirp->handle != -1) {
        _findclose(dirp->handle);
    }
    
    if (dirp->name) {
        free(dirp->name);
    }

    free(dirp);
    return 0;
}

void rewinddir(DIR *dirp) {
    char *search_path;
    size_t name_len;

    if (!dirp || !dirp->name) return;

    if (dirp->handle != -1) {
        _findclose(dirp->handle);
    }

    name_len = strlen(dirp->name);
    search_path = (char *)malloc(name_len + 3);
    if (!search_path) return;

#if defined(_MSC_VER) && _MSC_VER >= 1400
    strcpy_s(search_path, name_len + 3, dirp->name);
#else
    strcpy(search_path, dirp->name);
#endif

    if (name_len > 0 && search_path[name_len - 1] != '/' && search_path[name_len - 1] != '\\') {
#if defined(_MSC_VER) && _MSC_VER >= 1400
        strcat_s(search_path, name_len + 3, "\\*");
#else
        strcat(search_path, "\\*");
#endif
    } else {
#if defined(_MSC_VER) && _MSC_VER >= 1400
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
    if (!dirp) return -1;
    return dirp->offset;
}

void seekdir(DIR *dirp, long loc) {
    if (!dirp) return;

    rewinddir(dirp);
    while (dirp->offset < loc) {
        if (readdir(dirp) == NULL) break;
    }
}

int scandir(const char *dirp, struct dirent ***namelist,
            int (*filter)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **)) {
    DIR *d;
    struct dirent *entry;
    struct dirent **names = NULL;
    size_t count = 0;
    size_t capacity = 10;

    if (!dirp || !namelist) return -1;

    d = opendir(dirp);
    if (!d) return -1;

    names = (struct dirent **)malloc(capacity * sizeof(struct dirent *));
    if (!names) {
        closedir(d);
        return -1;
    }

    while ((entry = readdir(d)) != NULL) {
        if (!filter || filter(entry)) {
            struct dirent *copy = (struct dirent *)malloc(sizeof(struct dirent));
            if (!copy) {
                while (count > 0) free(names[--count]);
                free(names);
                closedir(d);
                return -1;
            }
            memcpy(copy, entry, sizeof(struct dirent));

            if (count >= capacity) {
                size_t new_cap = capacity * 2;
                struct dirent **new_names = (struct dirent **)realloc(names, new_cap * sizeof(struct dirent *));
                if (!new_names) {
                    free(copy);
                    while (count > 0) free(names[--count]);
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
        qsort(names, count, sizeof(struct dirent *), (int (*)(const void *, const void *))compar);
    }

    *namelist = names;
    return (int)count;
}

int alphasort(const struct dirent **a, const struct dirent **b) {
    return strcmp((*a)->d_name, (*b)->d_name);
}
#else
typedef int dummy_posix_dirent;
#endif /* _WIN32 */

/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

typedef int make_iso_compilers_happy_tu_posix_dirent;

