/* posix-glob.c - Strict C89 Implementation */
#if !defined(_MSC_VER)
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "posix-glob.h"

#if defined(_WIN32) || defined(_MSC_VER)
#include <io.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

/* helper to append to glob_t */
static int glob_append(glob_t *pglob, const char *path) {
    size_t new_count = pglob->gl_pathc + 1;
    size_t alloc_count = pglob->gl_offs + new_count + 1;
    char **new_pathv = (char **)realloc(pglob->gl_pathv, alloc_count * sizeof(char *));
    char *new_str;
    size_t len;
    
    if (!new_pathv) return GLOB_NOSPACE;
    pglob->gl_pathv = new_pathv;
    
    len = strlen(path) + 1;
    new_str = (char *)malloc(len);
    if (!new_str) return GLOB_NOSPACE;
    
#if defined(_MSC_VER)
    strcpy_s(new_str, len, path);
#else
    strcpy(new_str, path);
#endif
    
    pglob->gl_pathv[pglob->gl_offs + pglob->gl_pathc] = new_str;
    pglob->gl_pathc++;
    pglob->gl_pathv[pglob->gl_offs + pglob->gl_pathc] = NULL;
    
    return 0;
}

static int glob_compare(const void *a, const void *b) {
    const char *sa = *(const char * const *)a;
    const char *sb = *(const char * const *)b;
    if (!sa && !sb) return 0;
    if (!sa) return 1;
    if (!sb) return -1;
    return strcmp(sa, sb);
}

int fnmatch(const char *pattern, const char *string, int flags) {
    const char *p = pattern;
    const char *s = string;
    int c;

    if (!pattern || !string) return FNM_NOMATCH;

    for (;;) {
        c = *p++;
        switch (c) {
            case '\0':
                return (*s == '\0' ? 0 : FNM_NOMATCH);
            case '?':
                if (*s == '\0') return FNM_NOMATCH;
                if ((flags & FNM_PATHNAME) && *s == '/') return FNM_NOMATCH;
                if ((flags & FNM_PERIOD) && s == string && *s == '.') return FNM_NOMATCH;
                s++;
                break;
            case '*':
                c = *p;
                while (c == '*') { c = *++p; } /* collapse multiple stars */
                if ((flags & FNM_PERIOD) && s == string && *s == '.') return FNM_NOMATCH;
                if (c == '\0') {
                    if (flags & FNM_PATHNAME) {
                        while (*s != '\0') {
                            if (*s == '/') return FNM_NOMATCH;
                            s++;
                        }
                        return 0;
                    }
                    return 0;
                }
                while (*s != '\0') {
                    if ((flags & FNM_PATHNAME) && *s == '/') break;
                    if (fnmatch(p, s, flags) == 0) return 0;
                    s++;
                }
                return FNM_NOMATCH;
            case '\\':
                if (!(flags & FNM_NOESCAPE)) {
                    if (*p != '\0') c = *p++;
                }
                /* fall through */
            default:
                if (c != *s) return FNM_NOMATCH;
                s++;
                break;
        }
    }
}

#if defined(_WIN32) || defined(_MSC_VER)
int glob(const char *pattern, int flags, int (*errfunc)(const char *epath, int eerrno), glob_t *pglob) {
    struct _finddata_t fileinfo;
    intptr_t handle;
    char dir_prefix[1024];
    const char *last_slash;
    size_t prefix_len = 0;
    int initial_pathc;
    int ret = 0;
    int match_found = 0;

    (void)errfunc;

    if (!pattern || !pglob) return GLOB_ABORTED;

    if (!(flags & GLOB_APPEND)) {
        pglob->gl_pathc = 0;
        pglob->gl_pathv = NULL;
        if (flags & GLOB_DOOFFS) {
            pglob->gl_pathv = (char **)malloc((pglob->gl_offs + 1) * sizeof(char *));
            if (!pglob->gl_pathv) return GLOB_NOSPACE;
            memset(pglob->gl_pathv, 0, (pglob->gl_offs + 1) * sizeof(char *));
        } else {
            pglob->gl_offs = 0;
        }
    }

    initial_pathc = (int)pglob->gl_pathc;

    last_slash = strrchr(pattern, '/');
    if (!last_slash) {
        const char *bslash = strrchr(pattern, '\\');
        if (bslash) last_slash = bslash;
    }

    if (last_slash) {
        prefix_len = (size_t)(last_slash - pattern) + 1;
        if (prefix_len > sizeof(dir_prefix) - 1) prefix_len = sizeof(dir_prefix) - 1;
#if defined(_MSC_VER)
        strncpy_s(dir_prefix, sizeof(dir_prefix), pattern, prefix_len);
#else
        strncpy(dir_prefix, pattern, prefix_len);
        dir_prefix[prefix_len] = '\0';
#endif
    } else {
        dir_prefix[0] = '\0';
    }

    handle = _findfirst(pattern, &fileinfo);
    if (handle != -1) {
        do {
            char full_path[2048];
            if (strcmp(fileinfo.name, ".") == 0 || strcmp(fileinfo.name, "..") == 0) continue;

            if (prefix_len > 0) {
#if defined(_MSC_VER)
                sprintf_s(full_path, sizeof(full_path), "%s%s", dir_prefix, fileinfo.name);
#else
                sprintf(full_path, "%s%s", dir_prefix, fileinfo.name);
#endif
            } else {
#if defined(_MSC_VER)
                strcpy_s(full_path, sizeof(full_path), fileinfo.name);
#else
                strcpy(full_path, fileinfo.name);
#endif
            }

            ret = glob_append(pglob, full_path);
            if (ret != 0) break;
            match_found = 1;
        } while (_findnext(handle, &fileinfo) == 0);
        _findclose(handle);
    }

    if (ret != 0) return ret;

    if (!match_found) {
        if (flags & GLOB_NOCHECK) return glob_append(pglob, pattern);
        return GLOB_NOMATCH;
    }

    if (!(flags & GLOB_NOSORT) && pglob->gl_pathc > (size_t)initial_pathc) {
        qsort(pglob->gl_pathv + pglob->gl_offs + initial_pathc,
              pglob->gl_pathc - initial_pathc,
              sizeof(char *),
              glob_compare);
    }

    return 0;
}
#else
int glob(const char *pattern, int flags, int (*errfunc)(const char *epath, int eerrno), glob_t *pglob) {
    DIR *dir;
    struct dirent *ent;
    char dir_prefix[1024];
    const char *last_slash;
    size_t prefix_len = 0;
    int initial_pathc;
    int ret = 0;
    int match_found = 0;
    char search_dir[1024];
    const char *file_pattern;

    (void)errfunc;

    if (!pattern || !pglob) return GLOB_ABORTED;

    if (!(flags & GLOB_APPEND)) {
        pglob->gl_pathc = 0;
        pglob->gl_pathv = NULL;
        if (flags & GLOB_DOOFFS) {
            pglob->gl_pathv = (char **)malloc((pglob->gl_offs + 1) * sizeof(char *));
            if (!pglob->gl_pathv) return GLOB_NOSPACE;
            memset(pglob->gl_pathv, 0, (pglob->gl_offs + 1) * sizeof(char *));
        } else {
            pglob->gl_offs = 0;
        }
    }

    initial_pathc = (int)pglob->gl_pathc;

    last_slash = strrchr(pattern, '/');
    if (last_slash) {
        prefix_len = (size_t)(last_slash - pattern) + 1;
        if (prefix_len > sizeof(dir_prefix) - 1) prefix_len = sizeof(dir_prefix) - 1;
        strncpy(dir_prefix, pattern, prefix_len);
        dir_prefix[prefix_len] = '\0';
        
        if (prefix_len > 1) {
            strncpy(search_dir, pattern, prefix_len - 1);
            search_dir[prefix_len - 1] = '\0';
        } else {
            strcpy(search_dir, "/");
        }
        file_pattern = last_slash + 1;
    } else {
        dir_prefix[0] = '\0';
        strcpy(search_dir, ".");
        file_pattern = pattern;
    }

    dir = opendir(search_dir);
    if (dir) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;

            if (fnmatch(file_pattern, ent->d_name, 0) == 0) {
                char full_path[2048];
                if (prefix_len > 0) {
                    sprintf(full_path, "%s%s", dir_prefix, ent->d_name);
                } else {
                    strcpy(full_path, ent->d_name);
                }

                ret = glob_append(pglob, full_path);
                if (ret != 0) break;
                match_found = 1;
            }
        }
        closedir(dir);
    }

    if (ret != 0) return ret;

    if (!match_found) {
        if (flags & GLOB_NOCHECK) return glob_append(pglob, pattern);
        return GLOB_NOMATCH;
    }

    if (!(flags & GLOB_NOSORT) && pglob->gl_pathc > (size_t)initial_pathc) {
        qsort(pglob->gl_pathv + pglob->gl_offs + initial_pathc,
              pglob->gl_pathc - initial_pathc,
              sizeof(char *),
              glob_compare);
    }

    return 0;
}
#endif

void globfree(glob_t *pglob) {
    size_t i;
    if (!pglob || !pglob->gl_pathv) return;
    for (i = 0; i < pglob->gl_pathc; i++) {
        if (pglob->gl_pathv[pglob->gl_offs + i]) {
            free(pglob->gl_pathv[pglob->gl_offs + i]);
        }
    }
    free(pglob->gl_pathv);
    pglob->gl_pathv = NULL;
    pglob->gl_pathc = 0;
}

int wordexp(const char *words, wordexp_t *pwordexp, int flags) {
    char *copy;
    char *token;
#if defined(_MSC_VER) || !defined(_WIN32)
    char *context = NULL;
#endif
    char **new_we_wordv;

    if (!words || !pwordexp) return WRDE_SYNTAX;

    if (!(flags & WRDE_APPEND)) {
        pwordexp->we_wordc = 0;
        pwordexp->we_wordv = NULL;
        if (flags & WRDE_DOOFFS) {
            pwordexp->we_wordv = (char **)malloc((pwordexp->we_offs + 1) * sizeof(char *));
            if (!pwordexp->we_wordv) return WRDE_NOSPACE;
            memset(pwordexp->we_wordv, 0, (pwordexp->we_offs + 1) * sizeof(char *));
        } else {
            pwordexp->we_offs = 0;
        }
    }

    copy = (char *)malloc(strlen(words) + 1);
    if (!copy) return WRDE_NOSPACE;
    
#if defined(_MSC_VER)
    strcpy_s(copy, strlen(words) + 1, words);
    token = strtok_s(copy, " \t\n", &context);
#else
    strcpy(copy, words);
#if defined(_WIN32)
    token = strtok(copy, " \t\n");
#else
    token = strtok_r(copy, " \t\n", &context);
#endif
#endif

    while (token) {
        size_t len = strlen(token) + 1;
        char *word = (char *)malloc(len);
        if (!word) {
            free(copy);
            return WRDE_NOSPACE;
        }
#if defined(_MSC_VER)
        strcpy_s(word, len, token);
#else
        strcpy(word, token);
#endif

        new_we_wordv = (char **)realloc(pwordexp->we_wordv, (pwordexp->we_offs + pwordexp->we_wordc + 2) * sizeof(char *));
        if (!new_we_wordv) {
            free(word);
            free(copy);
            return WRDE_NOSPACE;
        }
        pwordexp->we_wordv = new_we_wordv;
        pwordexp->we_wordv[pwordexp->we_offs + pwordexp->we_wordc] = word;
        pwordexp->we_wordc++;
        pwordexp->we_wordv[pwordexp->we_offs + pwordexp->we_wordc] = NULL;

#if defined(_MSC_VER)
        token = strtok_s(NULL, " \t\n", &context);
#else
#if defined(_WIN32)
        token = strtok(NULL, " \t\n");
#else
        token = strtok_r(NULL, " \t\n", &context);
#endif
#endif
    }
    free(copy);

    return 0;
}

void wordfree(wordexp_t *pwordexp) {
    size_t i;
    if (!pwordexp || !pwordexp->we_wordv) return;
    for (i = 0; i < pwordexp->we_wordc; i++) {
        if (pwordexp->we_wordv[pwordexp->we_offs + i]) {
            free(pwordexp->we_wordv[pwordexp->we_offs + i]);
        }
    }
    free(pwordexp->we_wordv);
    pwordexp->we_wordv = NULL;
    pwordexp->we_wordc = 0;
}
