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

static void *default_fts_calloc(size_t nmemb, size_t size) {
  return calloc(nmemb, size);
}

static void *default_fts_malloc(size_t size) { return malloc(size); }

static DIR *default_fts_opendir(const char *name) {
#if defined(__WATCOMC__) && (defined(__DOS__) || defined(__MSDOS__))
  if (_osmajor < 2) {
    errno = ENOSYS;
    return NULL;
  }
#endif
  return opendir(name);
}

/** @brief Overrideable calloc hook for OOM testing in bsd-fts. */
void *(*g_bsd_fts_calloc)(size_t, size_t) = default_fts_calloc;
/** @brief Overrideable malloc hook for OOM testing in bsd-fts. */
void *(*g_bsd_fts_malloc)(size_t) = default_fts_malloc;
/** @brief Overrideable opendir hook for FTS_DNR testing in bsd-fts. */
DIR *(*g_bsd_fts_opendir)(const char *) = default_fts_opendir;

/** @brief Resets all bsd-fts allocator and directory hooks to defaults. */
void bsd_fts_reset_hooks(void);
void bsd_fts_reset_hooks(void) {
  g_bsd_fts_calloc = default_fts_calloc;
  g_bsd_fts_malloc = default_fts_malloc;
  g_bsd_fts_opendir = default_fts_opendir;
}

static void fts_strip_trailing_sep(char *buf, size_t cap, const char *path) {
  size_t len = strlen(path);
  if (len >= cap) {
    len = cap - 1;
  }
  memcpy(buf, path, len);
  buf[len] = '\0';
  while (len > 1 && (buf[len - 1] == '/' || buf[len - 1] == '\\')) {
    if (len == 3 && buf[1] == ':') {
      break;
    }
    buf[--len] = '\0';
  }
}

/**
 * @brief Allocate and populate a single FTSENT traversal node.
 * @param[in] sp Owning FTS stream.
 * @param[in] path Full traversal path string (non-NULL).
 * @param[in] name Entry basename string (non-NULL).
 * @param[in] level Traversal depth level.
 * @param[in] parent Parent FTSENT node, or NULL for root.
 * @return Allocated FTSENT pointer, or NULL on allocation failure.
 */
static FTSENT *fts_alloc_ent(FTS *sp, const char *path, const char *name,
                             long level, FTSENT *parent) {
  FTSENT *e;
  size_t plen;
  char clean_path[512];

  e = (FTSENT *)g_bsd_fts_calloc(1, sizeof(*e));
  if (e == NULL) {
    return NULL;
  }
  plen = strlen(path);
  e->fts_path = (char *)g_bsd_fts_malloc(plen + 1u);
  if (e->fts_path == NULL) {
    free(e);
    return NULL;
  }
  memcpy(e->fts_path, path, plen + 1u);
  e->fts_fts = sp;
  e->fts_level = level;
  e->fts_parent = parent;
  e->fts_instr = FTS_NOINSTR;
  e->fts_statp = &e->fts_statbuf;
  e->fts_name = e->fts_namebuf;
#if defined(_MSC_VER)
  strncpy_s(e->fts_namebuf, sizeof(e->fts_namebuf), name, _TRUNCATE);
#else
  strncpy(e->fts_namebuf, name, sizeof(e->fts_namebuf) - 1);
  e->fts_namebuf[sizeof(e->fts_namebuf) - 1] = '\0';
#endif
  e->fts_namelen = strlen(e->fts_namebuf);
  e->fts_accpath = e->fts_path;
  e->fts_pathlen = plen;

  fts_strip_trailing_sep(clean_path, sizeof(clean_path), e->fts_path);
  if ((sp->fts_options & FTS_NOSTAT) != 0) {
    e->fts_info = FTS_NSOK;
  } else if (stat(clean_path, &e->fts_statbuf) == 0) {
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
  return e;
}

/**
 * @brief Free a non-NULL FTSENT node and its path buffer.
 * @param[in,out] e Non-NULL FTSENT node.
 */
static void fts_free_ent(FTSENT *e) {
  free(e->fts_path);
  free(e);
}

/**
 * @brief Expand a preorder directory node into child entries plus FTS_DP.
 * @param[in,out] sp FTS stream.
 * @param[in,out] dir_ent Preorder directory entry (non-empty path).
 * @param[in,out] tail_next Pointer to head of remaining sibling queue.
 */
static void fts_expand_dir(FTS *sp, FTSENT *dir_ent, FTSENT **tail_next) {
  DIR *dp;
  struct dirent *de;
  FTSENT *dp_ent;
  FTSENT *first = NULL;
  FTSENT *last = NULL;
  size_t count = 0;
  char clean_dir[512];

  fts_strip_trailing_sep(clean_dir, sizeof(clean_dir), dir_ent->fts_path);
  dp = g_bsd_fts_opendir(clean_dir);
  if (dp == NULL) {
    dir_ent->fts_errno = errno;
    dir_ent->fts_info = FTS_DNR;
    return;
  }
  while ((de = readdir(dp)) != NULL) {
    char *childpath;
    FTSENT *ce;
    size_t plen;
    size_t nlen;
    size_t need_sep;
    if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
      if ((sp->fts_options & FTS_SEEDOT) == 0) {
        continue;
      }
    }
    plen = dir_ent->fts_pathlen;
    nlen = strlen(de->d_name);
    need_sep = (dir_ent->fts_path[plen - 1] == '/' ||
                dir_ent->fts_path[plen - 1] == '\\')
                   ? 0u
                   : 1u;
    childpath = (char *)g_bsd_fts_malloc(plen + need_sep + nlen + 1u);
    if (childpath == NULL) {
      continue;
    }
    memcpy(childpath, dir_ent->fts_path, plen);
    if (need_sep != 0u) {
      childpath[plen] = '/';
    }
    memcpy(childpath + plen + need_sep, de->d_name, nlen + 1u);
    ce = fts_alloc_ent(sp, childpath, de->d_name, dir_ent->fts_level + 1,
                       dir_ent);
    free(childpath);
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
    FTSENT **arr = (FTSENT **)g_bsd_fts_malloc(count * sizeof(FTSENT *));
    if (arr != NULL) {
      size_t i = 0;
      FTSENT *it = first;
      while (it != NULL) {
        arr[i++] = it;
        it = it->fts_link;
      }
      for (i = 1; i < count; i++) {
        FTSENT *key_ent = arr[i];
        size_t j = i;
        while (j > 0 && sp->fts_compar((const FTSENT *const *)&arr[j - 1],
                                       (const FTSENT *const *)&key_ent) > 0) {
          arr[j] = arr[j - 1];
          j--;
        }
        arr[j] = key_ent;
      }
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
      first = dp_ent;
      last = dp_ent;
    }
  }

  if (first != NULL) {
    last->fts_link = *tail_next;
    *tail_next = first;
  }
}

/**
 * @brief Open a file hierarchy for traversal.
 * @return Allocated FTS stream, or NULL on error.
 */
FTS *fts_open(char *const *path_argv, int options,
              int (*compar)(const FTSENT *const *, const FTSENT *const *)) {
  FTS *sp;
  FTSENT *first = NULL;
  FTSENT *last = NULL;
  char *const *p;

  if (path_argv == NULL || *path_argv == NULL) {
    errno = EINVAL;
    return NULL;
  }
  for (p = path_argv; *p != NULL; p++) {
    if (**p == '\0') {
      errno = ENOENT;
      return NULL;
    }
  }
  sp = (FTS *)g_bsd_fts_calloc(1, sizeof(*sp));
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
    if (s1 != NULL && s2 != NULL) {
      base = (s1 > s2) ? (s1 + 1) : (s2 + 1);
    } else if (s1 != NULL) {
      base = s1 + 1;
    } else if (s2 != NULL) {
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

/**
 * @brief Read the next entry in the file hierarchy.
 * @param[in,out] sp FTS stream.
 * @return Pointer to next FTSENT, or NULL on completion/error.
 */
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

/**
 * @brief Return linked list of queued children of the current stream.
 * @return Linked list of FTSENT nodes, or NULL.
 */
FTSENT *fts_children(FTS *sp, int instr) {
  (void)instr;
  if (sp == NULL) {
    errno = EINVAL;
    return NULL;
  }
  return sp->fts_child;
}

/**
 * @brief Set traversal instruction on an FTSENT node.
 * @return 0 on success, or -1 on error.
 */
int fts_set(FTS *sp, FTSENT *p, int instr) {
  (void)sp;
  if (p == NULL) {
    errno = EINVAL;
    return -1;
  }
  p->fts_instr = (unsigned short)instr;
  return 0;
}

/**
 * @brief Close an FTS stream and release all associated memory.
 * @return 0 on success, or -1 on error.
 */
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

/**
 * @brief Get client pointer associated with an FTS stream.
 * @return Client pointer, or NULL if sp is NULL.
 */
void *fts_get_clientptr(FTS *sp) {
  return (sp != NULL) ? sp->fts_clientptr : NULL;
}

/**
 * @brief Get parent FTS stream associated with an FTSENT node.
 * @return Owning FTS pointer, or NULL if p is NULL.
 */
FTS *fts_get_stream(FTSENT *p) { return (p != NULL) ? p->fts_fts : NULL; }

/**
 * @brief Associate a client pointer with an FTS stream.
 */
void fts_set_clientptr(FTS *sp, void *clientptr) {
  if (sp != NULL) {
    sp->fts_clientptr = clientptr;
  }
}

/** @brief Dummy typedef preventing empty translation unit warnings. */
typedef int make_iso_compilers_happy_tu_bsd_fts;

#endif
/** @brief Dummy typedef preventing empty mega translation unit warnings. */
typedef int make_iso_compilers_happy_mega_bsd_fts;
