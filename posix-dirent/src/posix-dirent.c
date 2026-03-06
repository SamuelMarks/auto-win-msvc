/* posix-dirent.c - Strict C89 Implementation */
#include "posix-dirent.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct DIR {
    HANDLE handle;
    WIN32_FIND_DATAA find_data;
    struct dirent entry;
    int first_read;
    long offset;
    char *name; /* Store the original directory name for rewinddir */
};

/* Helper function to determine d_type from WIN32_FIND_DATAA */
static unsigned char get_d_type(DWORD dwFileAttributes) {
    if (dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        return DT_LNK;
    }
    if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        return DT_DIR;
    }
    return DT_REG;
}

/**
 * @brief Opens a directory stream.
 */
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

    dirp->handle = FindFirstFileA(search_path, &dirp->find_data);
    free(search_path);

    if (dirp->handle == INVALID_HANDLE_VALUE) {
        free(dirp);
        errno = ENOENT;
        return NULL;
    }

    dirp->first_read = 1;
    dirp->offset = 0;
    
    dirp->name = (char *)malloc(name_len + 1);
    if (!dirp->name) {
        if (dirp->handle != INVALID_HANDLE_VALUE) {
            FindClose(dirp->handle);
        }
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

/**
 * @brief Reads a directory entry.
 */
struct dirent *readdir(DIR *dirp) {
    if (!dirp) {
        errno = EBADF;
        return NULL;
    }

    if (!dirp->first_read) {
        if (!FindNextFileA(dirp->handle, &dirp->find_data)) {
            return NULL;
        }
    } else {
        dirp->first_read = 0;
    }

    dirp->entry.d_ino = 0;
    dirp->entry.d_off = dirp->offset++;
    dirp->entry.d_reclen = 0;
    dirp->entry.d_type = get_d_type(dirp->find_data.dwFileAttributes);

#if defined(_MSC_VER) && _MSC_VER >= 1400
    strncpy_s(dirp->entry.d_name, sizeof(dirp->entry.d_name), dirp->find_data.cFileName, _TRUNCATE);
#else
    strncpy(dirp->entry.d_name, dirp->find_data.cFileName, sizeof(dirp->entry.d_name) - 1);
    dirp->entry.d_name[sizeof(dirp->entry.d_name) - 1] = '\0';
#endif

    return &dirp->entry;
}

/**
 * @brief Closes a directory stream.
 */
int closedir(DIR *dirp) {
    if (!dirp) {
        errno = EBADF;
        return -1;
    }

    if (dirp->handle != INVALID_HANDLE_VALUE) {
        FindClose(dirp->handle);
    }
    
    if (dirp->name) {
        free(dirp->name);
    }

    free(dirp);
    return 0;
}

/**
 * @brief Rewinds a directory stream.
 */
void rewinddir(DIR *dirp) {
    char *search_path;
    size_t name_len;

    if (!dirp || !dirp->name) return;

    if (dirp->handle != INVALID_HANDLE_VALUE) {
        FindClose(dirp->handle);
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

    dirp->handle = FindFirstFileA(search_path, &dirp->find_data);
    free(search_path);

    dirp->first_read = 1;
    dirp->offset = 0;
}

/**
 * @brief Returns the current location in the directory stream.
 */
long telldir(DIR *dirp) {
    if (!dirp) return -1;
    return dirp->offset;
}

/**
 * @brief Seeks to a specific location in the directory stream.
 */
void seekdir(DIR *dirp, long loc) {
    if (!dirp) return;

    rewinddir(dirp);
    while (dirp->offset < loc) {
        if (readdir(dirp) == NULL) break;
    }
}

/**
 * @brief Scans a directory for entries.
 */
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

/**
 * @brief Compares two directory entries for sorting alphabetically.
 */
int alphasort(const struct dirent **a, const struct dirent **b) {
    return strcmp((*a)->d_name, (*b)->d_name);
}
#else
/* When building on a non-Windows platform where this file might be compiled, 
   we can just leave it empty or provide dummy implementations to avoid empty translation unit warnings. */
typedef int dummy_posix_dirent;
#endif /* _WIN32 */
