/* posix-libgen.c - Strict C89 Implementation */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "posix-libgen.h"

#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif
#ifndef _MAX_DRIVE
#define _MAX_DRIVE 3
#endif
#ifndef _MAX_DIR
#define _MAX_DIR 256
#endif
#ifndef _MAX_FNAME
#define _MAX_FNAME 256
#endif
#ifndef _MAX_EXT
#define _MAX_EXT 256
#endif

/* Fallback for non-MSVC compilers if tested on Linux/macOS */
#if !defined(_MSC_VER) && !defined(__MINGW32__)
static int _splitpath_s(const char *path, char *drive, size_t drive_sz,
                        char *dir, size_t dir_sz, char *fname, size_t fname_sz,
                        char *ext, size_t ext_sz) {
    const char *last_slash = NULL;
    const char *p;
    size_t dir_len = 0;

    if (drive_sz > 0) drive[0] = '\0';
    if (dir_sz > 0) dir[0] = '\0';
    if (fname_sz > 0) fname[0] = '\0';
    if (ext_sz > 0) ext[0] = '\0';

    if (path == NULL) return 0;

    if (path[0] != '\0' && path[1] == ':') {
        if (drive_sz > 2) {
            drive[0] = path[0];
            drive[1] = path[1];
            drive[2] = '\0';
        }
        path += 2;
    }

    for (p = path; *p != '\0'; ++p) {
        if (*p == '/' || *p == '\\') {
            last_slash = p;
        }
    }

    if (last_slash != NULL) {
        dir_len = (size_t)(last_slash - path + 1);
        if (dir_len < dir_sz) {
            strncpy(dir, path, dir_len);
            dir[dir_len] = '\0';
        }
        if (fname_sz > strlen(last_slash + 1)) {
            strcpy(fname, last_slash + 1);
        }
    } else {
        if (fname_sz > strlen(path)) {
            strcpy(fname, path);
        }
    }
    return 0;
}

static int _makepath_s(char *path, size_t sizeInBytes, const char *drive,
                       const char *dir, const char *fname, const char *ext) {
    if (sizeInBytes == 0) return 0;
    path[0] = '\0';
    if (drive != NULL) strcat(path, drive);
    if (dir != NULL) strcat(path, dir);
    if (fname != NULL) strcat(path, fname);
    if (ext != NULL) strcat(path, ext);
    return 0;
}
#endif

char *basename(char *path) {
    static char result[_MAX_PATH];
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    size_t len;
    int is_all_slashes;
    size_t i;
    
    if (path == NULL || path[0] == '\0') {
        return (char *)".";
    }
    
    len = strlen(path);
    is_all_slashes = 1;
    for (i = 0; i < len; ++i) {
        if (path[i] != '/' && path[i] != '\\') {
            is_all_slashes = 0;
            break;
        }
    }
    
    if (is_all_slashes) {
        return (char *)"/";
    }
    
    /* Strip trailing slashes */
    while (len > 1 && (path[len - 1] == '/' || path[len - 1] == '\\')) {
        path[len - 1] = '\0';
        len--;
    }
    
    if (_splitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT) != 0) {
        return (char *)".";
    }

    if (fname[0] == '\0' && ext[0] == '\0') {
        return (char *)".";
    }

    if (_makepath_s(result, _MAX_PATH, NULL, NULL, fname, ext) != 0) {
        return (char *)".";
    }

    return result;
}

char *dirname(char *path) {
    static char result[_MAX_PATH];
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    size_t len;
    size_t dir_len;
    int is_all_slashes;
    size_t i;
    
    if (path == NULL || path[0] == '\0') {
        return (char *)".";
    }
    
    len = strlen(path);
    is_all_slashes = 1;
    for (i = 0; i < len; ++i) {
        if (path[i] != '/' && path[i] != '\\') {
            is_all_slashes = 0;
            break;
        }
    }
    
    if (is_all_slashes) {
        return (char *)"/";
    }
    
    /* Strip trailing slashes */
    while (len > 1 && (path[len - 1] == '/' || path[len - 1] == '\\')) {
        path[len - 1] = '\0';
        len--;
    }
    
    if (_splitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT) != 0) {
        return (char *)".";
    }

    dir_len = strlen(dir);
    /* Remove trailing slashes from dir, except if it makes dir empty but dir was originally not empty */
    while (dir_len > 1 && (dir[dir_len - 1] == '/' || dir[dir_len - 1] == '\\')) {
        dir[dir_len - 1] = '\0';
        dir_len--;
    }
    
    /* If dir is just a slash, we might have dir_len == 1. That is correct. */

    if (dir[0] == '\0' && drive[0] == '\0') {
        return (char *)".";
    }

    if (_makepath_s(result, _MAX_PATH, drive, dir, NULL, NULL) != 0) {
        return (char *)".";
    }

    len = strlen(result);
    if (len == 0) {
        return (char *)".";
    }
    
    /* Strip trailing slashes from result, except if it is the root directory */
    while (len > 1 && (result[len - 1] == '/' || result[len - 1] == '\\')) {
        /* Do not strip if it is just "C:\" */
        if (len == 3 && result[1] == ':') {
            break;
        }
        result[len - 1] = '\0';
        len--;
    }
    
    if (result[0] == '\0') {
        return (char *)".";
    }
    
    return result;
}

