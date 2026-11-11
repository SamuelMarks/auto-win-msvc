#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)
/**
 * @file bsd_fts.c
 * @brief Implementation of BSD <fts.h> file hierarchy traversal.
 */

/* clang-format off */
#include <fts.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

static FTSENT *fts_alloc_ent(FTS *sp, const char *path, const char *name,
                             long level, FTSENT *parent) {
  FTSENT *e = (FTSENT *)calloc(1, sizeof(*e));
  if (e == NULL) {
    return NULL;
  }
  e->fts_fts = sp;
  e->fts_level = level;
  e->fts_parent = parent;
  e->fts_instr = FTS_NOINSTR;
  e->fts_statp = &e->fts_statbuf;
  e->fts_name = e->fts_namebuf;
#if defined(_MSC_VER)
  strncpy_s(e->fts_namebuf, sizeof(e->fts_namebuf), name ? name : "",
            _TRUNCATE);
  e->fts_namelen = strlen(e->fts_namebuf);
  e->fts_path = _strdup(path ? path : "");
#else
  strncpy(e->fts_namebuf, name ? name : "", sizeof(e->fts_namebuf) - 1);
  e->fts_namebuf[sizeof(e->fts_namebuf) - 1] = '\0';
  e->fts_namelen = strlen(e->fts_namebuf);
  e->fts_path = strdup(path ? path : "");
#endif
  e->fts_accpath = e->fts_path;
  e->fts_pathlen = e->fts_path ? strlen(e->fts_path) : 0;

  if ((sp->fts_options & FTS_NOSTAT) != 0) {
    e->fts_info = FTS_NSOK;
  } else {
    if (stat(e->fts_path, &e->fts_statbuf) == 0) {
      e->fts_ino = e->fts_statbuf.st_ino;
      e->fts_dev = e->fts_statbuf.st_dev;
      e->fts_nlink = e->fts_statbuf.st_nlink;
      if ((e->fts_statbuf.st_mode & S_IFMT) == S_IFDIR) {
        e->fts_info = FTS_D;
      } else {
        e->fts_info = FTS_F;
      }
    } else {
      e->fts_errno = errno;
      e->fts_info = FTS_NS;
    }
  }
  return e;
}

static void fts_free_ent(FTSENT *e) {
  if (e != NULL) {
    if (e->fts_path != NULL) {
      free(e->fts_path);
    }
    free(e);
  }
}

static void fts_expand_dir(FTS *sp, FTSENT *dir_ent, FTSENT **tail_next) {
  DIR *dp;
  struct dirent *de;
  FTSENT *dp_ent;
  FTSENT *first = NULL, *last = NULL;
  size_t count = 0;

  dp = opendir(dir_ent->fts_path);
  if (dp == NULL) {
    dir_ent->fts_errno = errno;
    dir_ent->fts_info = FTS_DNR;
    return;
  }
  while ((de = readdir(dp)) != NULL) {
    char childpath[1024];
    FTSENT *ce;
    size_t plen;
    if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
      if ((sp->fts_options & FTS_SEEDOT) == 0) {
        continue;
      }
    }
    plen = strlen(dir_ent->fts_path);
    if (plen > 0 && (dir_ent->fts_path[plen - 1] == '/' ||
                     dir_ent->fts_path[plen - 1] == '\\')) {
#if defined(_MSC_VER)
      _snprintf_s(childpath, sizeof(childpath), _TRUNCATE, "%s%s",
                  dir_ent->fts_path, de->d_name);
#else
      snprintf(childpath, sizeof(childpath), "%s%s", dir_ent->fts_path,
               de->d_name);
#endif
    } else {
#if defined(_MSC_VER)
      _snprintf_s(childpath, sizeof(childpath), _TRUNCATE, "%s/%s",
                  dir_ent->fts_path, de->d_name);
#else
      snprintf(childpath, sizeof(childpath), "%s/%s", dir_ent->fts_path,
               de->d_name);
#endif
    }
    ce = fts_alloc_ent(sp, childpath, de->d_name, dir_ent->fts_level + 1,
                       dir_ent);
    if (ce != NULL) {
      if (first == NULL) {
        first = ce;
      } else {
        last->fts_link = ce;
      }
      last = ce;
      count++;
    }
  }
  closedir(dp);

  if (count > 1 && sp->fts_compar != NULL) {
    FTSENT **arr = (FTSENT **)malloc(count * sizeof(FTSENT *));
    if (arr != NULL) {
      size_t i = 0;
      FTSENT *it = first;
      while (it != NULL) {
        arr[i++] = it;
        it = it->fts_link;
      }
      qsort(arr, count, sizeof(FTSENT *),
            (int (*)(const void *, const void *))sp->fts_compar);
      first = arr[0];
      for (i = 0; i + 1 < count; i++) {
        arr[i]->fts_link = arr[i + 1];
      }
      arr[count - 1]->fts_link = NULL;
      last = arr[count - 1];
      free(arr);
    }
  }

  dp_ent = fts_alloc_ent(sp, dir_ent->fts_path, dir_ent->fts_name,
                         dir_ent->fts_level, dir_ent->fts_parent);
  if (dp_ent != NULL) {
    dp_ent->fts_info = FTS_DP;
    if (last != NULL) {
      last->fts_link = dp_ent;
      last = dp_ent;
    } else {
      first = last = dp_ent;
    }
  }

  if (first != NULL) {
    last->fts_link = *tail_next;
    *tail_next = first;
  }
}

FTS *fts_open(char *const *path_argv, int options,
              int (*compar)(const FTSENT *const *, const FTSENT *const *)) {
  FTS *sp;
  FTSENT *first = NULL, *last = NULL;
  char *const *p;

  if (path_argv == NULL || *path_argv == NULL) {
    errno = EINVAL;
    return NULL;
  }
  sp = (FTS *)calloc(1, sizeof(*sp));
  if (sp == NULL) {
    return NULL;
  }
  sp->fts_options = options;
  sp->fts_compar = compar;

  for (p = path_argv; *p != NULL; p++) {
    const char *base;
    const char *s1 = strrchr(*p, '/');
    const char *s2 = strrchr(*p, '\\');
    FTSENT *e;
    if (s1 && s2) {
      base = (s1 > s2) ? (s1 + 1) : (s2 + 1);
    } else if (s1) {
      base = s1 + 1;
    } else if (s2) {
      base = s2 + 1;
    } else {
      base = *p;
    }
    e = fts_alloc_ent(sp, *p, base, FTS_ROOTLEVEL, NULL);
    if (e != NULL) {
      if (first == NULL) {
        first = e;
      } else {
        last->fts_link = e;
      }
      last = e;
    }
  }
  sp->fts_child = first;
  return sp;
}

FTSENT *fts_read(FTS *sp) {
  FTSENT *cur;
  if (sp == NULL) {
    errno = EINVAL;
    return NULL;
  }
  if (sp->fts_cur != NULL) {
    cur = sp->fts_cur;
    if (cur->fts_info == FTS_D && cur->fts_instr != FTS_SKIP) {
      fts_expand_dir(sp, cur, &sp->fts_child);
    }
    fts_free_ent(cur);
    sp->fts_cur = NULL;
  }
  while (sp->fts_child != NULL) {
    cur = sp->fts_child;
    sp->fts_child = cur->fts_link;
    cur->fts_link = NULL;
    if (cur->fts_instr == FTS_SKIP) {
      fts_free_ent(cur);
      continue;
    }
    sp->fts_cur = cur;
    return cur;
  }
  errno = 0;
  return NULL;
}

FTSENT *fts_children(FTS *sp, int instr) {
  (void)instr;
  if (sp == NULL) {
    errno = EINVAL;
    return NULL;
  }
  return sp->fts_child;
}

int fts_set(FTS *sp, FTSENT *p, int instr) {
  (void)sp;
  if (p == NULL) {
    errno = EINVAL;
    return -1;
  }
  p->fts_instr = (unsigned short)instr;
  return 0;
}

int fts_close(FTS *sp) {
  FTSENT *it;
  if (sp == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (sp->fts_cur != NULL) {
    fts_free_ent(sp->fts_cur);
  }
  it = sp->fts_child;
  while (it != NULL) {
    FTSENT *next = it->fts_link;
    fts_free_ent(it);
    it = next;
  }
  free(sp);
  return 0;
}

void *fts_get_clientptr(FTS *sp) {
  return (sp != NULL) ? sp->fts_clientptr : NULL;
}

FTS *fts_get_stream(FTSENT *p) { return (p != NULL) ? p->fts_fts : NULL; }

void fts_set_clientptr(FTS *sp, void *clientptr) {
  if (sp != NULL) {
    sp->fts_clientptr = clientptr;
  }
}

typedef int make_iso_compilers_happy_tu_bsd_fts;

#endif
typedef int make_iso_compilers_happy_mega_bsd_fts;
