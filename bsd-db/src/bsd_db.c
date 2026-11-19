#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)
/**
 * @file bsd_db.c
 * @brief Implementation of BSD 1.85 <db.h> and <ndbm.h>.
 */

/* clang-format off */
#include <db.h>
#include <ndbm.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

static void *default_db_calloc(size_t nmemb, size_t size) {
  return calloc(nmemb, size);
}

static void *default_db_malloc(size_t size) { return malloc(size); }

static void *default_db_realloc(void *ptr, size_t size) {
  return realloc(ptr, size);
}

/** @brief Overrideable calloc hook for OOM testing in bsd-db. */
void *(*g_bsd_db_calloc)(size_t, size_t) = default_db_calloc;
/** @brief Overrideable malloc hook for OOM testing in bsd-db. */
void *(*g_bsd_db_malloc)(size_t) = default_db_malloc;
/** @brief Overrideable realloc hook for OOM testing in bsd-db. */
void *(*g_bsd_db_realloc)(void *, size_t) = default_db_realloc;

/** @brief Resets all bsd-db allocator hooks back to standard allocators. */
void bsd_db_reset_alloc_hooks(void);
void bsd_db_reset_alloc_hooks(void) {
  g_bsd_db_calloc = default_db_calloc;
  g_bsd_db_malloc = default_db_malloc;
  g_bsd_db_realloc = default_db_realloc;
}

/** @brief Internal key-value entry stored in an in-memory DB table. */
struct awm_db_kv {
  void *kdata;  /**< Heap-allocated key byte buffer. */
  size_t ksize; /**< Byte length of key. */
  void *vdata;  /**< Heap-allocated value byte buffer. */
  size_t vsize; /**< Byte length of value. */
};

/** @brief Internal cursor and item array state for an in-memory DB handle. */
struct awm_db_state {
  struct awm_db_kv *items; /**< Dynamic array of sorted key-value entries. */
  size_t count;            /**< Number of active entries. */
  size_t cap;              /**< Allocated slot capacity of items array. */
  size_t seq_idx;          /**< Current sequential cursor index. */
  int started;             /**< Non-zero once sequential cursor has started. */
};

/**
 * @brief Lexicographically compare two DBT keys.
 * @param[in] a First key datum.
 * @param[in] b Second key datum.
 * @return Negative, zero, or positive ordering result.
 */
static int awm_db_cmp_default(const DBT *a, const DBT *b) {
  size_t minlen = (a->size < b->size) ? a->size : b->size;
  int r = (minlen > 0) ? memcmp(a->data, b->data, minlen) : 0;
  if (r != 0) {
    return r;
  }
  return (a->size > b->size) - (a->size < b->size);
}

/**
 * @brief Close a DB handle and free all stored items.
 * @param[in,out] db Target database handle.
 * @return RET_SUCCESS on success, or RET_ERROR if db is NULL.
 */
static int awm_db_close(DB *db) {
  struct awm_db_state *st;
  size_t i;
  if (db == NULL) {
    return RET_ERROR;
  }
  st = (struct awm_db_state *)db->internal;
  if (st != NULL) {
    for (i = 0; i < st->count; i++) {
      free(st->items[i].kdata);
      free(st->items[i].vdata);
    }
    free(st->items);
    free(st);
  }
  free(db);
  return RET_SUCCESS;
}

/**
 * @brief Look up a key in the database.
 * @param[in] db Database handle.
 * @param[in] key Search key datum.
 * @param[out] data Output value datum.
 * @param[in] flags Lookup flags.
 * @return RET_SUCCESS if found, RET_SPECIAL if missing, or RET_ERROR.
 */
static int awm_db_get(const DB *db, const DBT *key, DBT *data,
                      unsigned int flags) {
  struct awm_db_state *st;
  size_t i;
  (void)flags;
  if (db == NULL || key == NULL || data == NULL) {
    return RET_ERROR;
  }
  st = (struct awm_db_state *)db->internal;
  for (i = 0; i < st->count; i++) {
    DBT k;
    k.data = st->items[i].kdata;
    k.size = st->items[i].ksize;
    if (awm_db_cmp_default(&k, key) == 0) {
      data->data = st->items[i].vdata;
      data->size = st->items[i].vsize;
      return RET_SUCCESS;
    }
  }
  return RET_SPECIAL;
}

/**
 * @brief Insert or update a key-value pair in the database.
 * @param[in] db Database handle.
 * @param[in,out] key Key datum.
 * @param[in] data Value datum.
 * @param[in] flags Put flags (e.g., R_NOOVERWRITE).
 * @return RET_SUCCESS, RET_SPECIAL, or RET_ERROR.
 */
static int awm_db_put(const DB *db, DBT *key, const DBT *data,
                      unsigned int flags) {
  struct awm_db_state *st;
  size_t i;
  void *nk;
  void *nv;
  if (db == NULL || key == NULL || data == NULL) {
    return RET_ERROR;
  }
  st = (struct awm_db_state *)db->internal;
  for (i = 0; i < st->count; i++) {
    DBT k;
    k.data = st->items[i].kdata;
    k.size = st->items[i].ksize;
    if (awm_db_cmp_default(&k, key) == 0) {
      if (flags == R_NOOVERWRITE) {
        return RET_SPECIAL;
      }
      nv = g_bsd_db_malloc(data->size ? data->size : 1);
      if (nv == NULL) {
        return RET_ERROR;
      }
      if (data->size > 0) {
        memcpy(nv, data->data, data->size);
      }
      free(st->items[i].vdata);
      st->items[i].vdata = nv;
      st->items[i].vsize = data->size;
      return RET_SUCCESS;
    }
  }
  if (st->count >= st->cap) {
    size_t ncap = (st->cap == 0) ? 16 : (st->cap * 2);
    struct awm_db_kv *ni =
        (struct awm_db_kv *)g_bsd_db_realloc(st->items, ncap * sizeof(*ni));
    if (ni == NULL) {
      return RET_ERROR;
    }
    st->items = ni;
    st->cap = ncap;
  }
  nk = g_bsd_db_malloc(key->size ? key->size : 1);
  nv = g_bsd_db_malloc(data->size ? data->size : 1);
  if (nk == NULL || nv == NULL) {
    free(nk);
    free(nv);
    return RET_ERROR;
  }
  if (key->size > 0) {
    memcpy(nk, key->data, key->size);
  }
  if (data->size > 0) {
    memcpy(nv, data->data, data->size);
  }
  st->items[st->count].kdata = nk;
  st->items[st->count].ksize = key->size;
  st->items[st->count].vdata = nv;
  st->items[st->count].vsize = data->size;
  st->count++;
  return RET_SUCCESS;
}

/**
 * @brief Delete a record by key or cursor from the database.
 * @param[in] db Database handle.
 * @param[in] key Key datum (ignored when flags == R_CURSOR).
 * @param[in] flags Delete flags (0 or R_CURSOR).
 * @return RET_SUCCESS, RET_SPECIAL, or RET_ERROR.
 */
static int awm_db_del(const DB *db, const DBT *key, unsigned int flags) {
  struct awm_db_state *st;
  size_t i;
  size_t j;
  if (db == NULL) {
    return RET_ERROR;
  }
  st = (struct awm_db_state *)db->internal;
  if (flags == R_CURSOR) {
    if (!st->started || st->seq_idx >= st->count) {
      return RET_SPECIAL;
    }
    i = st->seq_idx;
    free(st->items[i].kdata);
    free(st->items[i].vdata);
    for (j = i + 1; j < st->count; j++) {
      st->items[j - 1] = st->items[j];
    }
    st->count--;
    if (st->seq_idx > 0) {
      st->seq_idx--;
    } else {
      st->started = 0;
    }
    return RET_SUCCESS;
  }
  if (key == NULL) {
    return RET_ERROR;
  }
  for (i = 0; i < st->count; i++) {
    DBT k;
    k.data = st->items[i].kdata;
    k.size = st->items[i].ksize;
    if (awm_db_cmp_default(&k, key) == 0) {
      free(st->items[i].kdata);
      free(st->items[i].vdata);
      for (j = i + 1; j < st->count; j++) {
        st->items[j - 1] = st->items[j];
      }
      st->count--;
      return RET_SUCCESS;
    }
  }
  return RET_SPECIAL;
}

/**
 * @brief Sequentially iterate records in the database.
 * @param[in] db Database handle.
 * @param[out] key Output key datum.
 * @param[out] data Output value datum.
 * @param[in] flags Cursor movement flag (R_FIRST, R_LAST, R_NEXT, R_PREV,
 * R_CURSOR).
 * @return RET_SUCCESS, RET_SPECIAL, or RET_ERROR.
 */
static int awm_db_seq(const DB *db, DBT *key, DBT *data, unsigned int flags) {
  struct awm_db_state *st;
  if (db == NULL || key == NULL || data == NULL) {
    return RET_ERROR;
  }
  st = (struct awm_db_state *)db->internal;
  if (st->count == 0) {
    return RET_SPECIAL;
  }
  if (flags == R_FIRST || !st->started) {
    st->seq_idx = 0;
    st->started = 1;
  } else if (flags == R_LAST) {
    st->seq_idx = st->count - 1;
    st->started = 1;
  } else if (flags == R_NEXT) {
    if (st->seq_idx + 1 >= st->count) {
      return RET_SPECIAL;
    }
    st->seq_idx++;
  } else if (flags == R_PREV) {
    if (st->seq_idx == 0) {
      return RET_SPECIAL;
    }
    st->seq_idx--;
  }
  key->data = st->items[st->seq_idx].kdata;
  key->size = st->items[st->seq_idx].ksize;
  data->data = st->items[st->seq_idx].vdata;
  data->size = st->items[st->seq_idx].vsize;
  return RET_SUCCESS;
}

/**
 * @brief Flush cached database pages (no-op for in-memory store).
 * @param[in] db Database handle.
 * @param[in] flags Sync flags.
 * @return RET_SUCCESS.
 */
static int awm_db_sync(const DB *db, unsigned int flags) {
  (void)db;
  (void)flags;
  return RET_SUCCESS;
}

/**
 * @brief Return underlying file descriptor (-1 for in-memory database).
 * @param[in] db Database handle.
 * @return -1 with errno set to ENOENT.
 */
static int awm_db_fd(const DB *db) {
  (void)db;
  errno = ENOENT;
  return -1;
}

/** @brief Open or create a BSD 1.85 database handle. */
DB *dbopen(const char *file, int flags, int mode, DBTYPE type,
           const void *openinfo) {
  DB *db;
  struct awm_db_state *st;
  (void)file;
  (void)flags;
  (void)mode;
  (void)openinfo;
  db = (DB *)g_bsd_db_calloc(1, sizeof(*db));
  if (db == NULL) {
    return NULL;
  }
  st = (struct awm_db_state *)g_bsd_db_calloc(1, sizeof(*st));
  if (st == NULL) {
    free(db);
    return NULL;
  }
  db->type = type;
  db->internal = st;
  db->close = awm_db_close;
  db->del = awm_db_del;
  db->get = awm_db_get;
  db->put = awm_db_put;
  db->seq = awm_db_seq;
  db->sync = awm_db_sync;
  db->fd = awm_db_fd;
  return db;
}

/**
 * @brief Open an ndbm database handle.
 * @param[in] file Database path prefix.
 * @param[in] open_flags Open flags.
 * @param[in] file_mode Permission mode.
 * @return Allocated DBM pointer, or NULL on failure.
 */
DBM *dbm_open(const char *file, int open_flags, mode_t file_mode) {
  return dbopen(file, open_flags, (int)file_mode, DB_HASH, NULL);
}

/**
 * @brief Close an ndbm database handle.
 * @param[in,out] db Target database handle (may be NULL).
 */
void dbm_close(DBM *db) {
  if (db != NULL) {
    db->close(db);
  }
}

/**
 * @brief Fetch a value by key from an ndbm database.
 * @param[in] db Database handle.
 * @param[in] key Lookup key datum.
 * @return Matching datum, or `{NULL, 0}` if not found.
 */
datum dbm_fetch(DBM *db, datum key) {
  DBT k;
  DBT v;
  datum res;
  res.dptr = NULL;
  res.dsize = 0;
  if (db == NULL) {
    return res;
  }
  k.data = key.dptr;
  k.size = (size_t)key.dsize;
  if (db->get(db, &k, &v, 0) == RET_SUCCESS) {
    res.dptr = (char *)v.data;
    res.dsize = (int)v.size;
  }
  return res;
}

/**
 * @brief Position cursor at the first key in an ndbm database.
 * @param[in] db Database handle.
 * @return First key datum, or `{NULL, 0}` if empty/NULL.
 */
datum dbm_firstkey(DBM *db) {
  DBT k;
  DBT v;
  datum res;
  res.dptr = NULL;
  res.dsize = 0;
  if (db != NULL && db->seq(db, &k, &v, R_FIRST) == RET_SUCCESS) {
    res.dptr = (char *)k.data;
    res.dsize = (int)k.size;
  }
  return res;
}

/**
 * @brief Advance cursor to the next key in an ndbm database.
 * @param[in] db Database handle.
 * @return Next key datum, or `{NULL, 0}` at end/NULL.
 */
datum dbm_nextkey(DBM *db) {
  DBT k;
  DBT v;
  datum res;
  res.dptr = NULL;
  res.dsize = 0;
  if (db != NULL && db->seq(db, &k, &v, R_NEXT) == RET_SUCCESS) {
    res.dptr = (char *)k.data;
    res.dsize = (int)k.size;
  }
  return res;
}

/**
 * @brief Delete a key from an ndbm database.
 * @param[in,out] db Database handle.
 * @param[in] key Key datum to delete.
 * @return 0 on success, or -1 if missing/NULL.
 */
int dbm_delete(DBM *db, datum key) {
  DBT k;
  if (db == NULL) {
    return -1;
  }
  k.data = key.dptr;
  k.size = (size_t)key.dsize;
  return (db->del(db, &k, 0) == RET_SUCCESS) ? 0 : -1;
}

/**
 * @brief Store a key/content pair in an ndbm database.
 * @param[in,out] db Database handle.
 * @param[in] key Key datum.
 * @param[in] content Value datum.
 * @param[in] store_mode DBM_INSERT or DBM_REPLACE.
 * @return 0 on success, 1 if DBM_INSERT and key exists, or -1 on error.
 */
int dbm_store(DBM *db, datum key, datum content, int store_mode) {
  DBT k;
  DBT v;
  if (db == NULL) {
    return -1;
  }
  k.data = key.dptr;
  k.size = (size_t)key.dsize;
  v.data = content.dptr;
  v.size = (size_t)content.dsize;
  return db->put(db, &k, &v, (store_mode == DBM_INSERT) ? R_NOOVERWRITE : 0);
}

/**
 * @brief Query error status of an ndbm database.
 * @param[in] db Database handle.
 * @return 0.
 */
int dbm_error(DBM *db) {
  (void)db;
  return 0;
}

/**
 * @brief Clear error status of an ndbm database.
 * @param[in,out] db Database handle.
 * @return 0.
 */
int dbm_clearerr(DBM *db) {
  (void)db;
  return 0;
}

/**
 * @brief Return directory file descriptor for an ndbm database.
 * @param[in] db Database handle.
 * @return File descriptor, or -1.
 */
int dbm_dirfno(DBM *db) { return (db != NULL) ? db->fd(db) : -1; }

/** @brief Dummy typedef preventing empty translation unit warnings. */
typedef int make_iso_compilers_happy_tu_bsd_db;

#endif
/** @brief Dummy typedef preventing empty mega translation unit warnings. */
typedef int make_iso_compilers_happy_mega_bsd_db;
