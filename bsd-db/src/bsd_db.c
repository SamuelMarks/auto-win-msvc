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

struct awm_db_kv {
  void *kdata;
  size_t ksize;
  void *vdata;
  size_t vsize;
};

struct awm_db_state {
  struct awm_db_kv *items;
  size_t count;
  size_t cap;
  size_t seq_idx;
  int started;
};

static int awm_db_cmp_default(const DBT *a, const DBT *b) {
  size_t minlen = (a->size < b->size) ? a->size : b->size;
  int r = memcmp(a->data, b->data, minlen);
  if (r != 0) {
    return r;
  }
  return (a->size > b->size) - (a->size < b->size);
}

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

static int awm_db_put(const DB *db, DBT *key, const DBT *data,
                      unsigned int flags) {
  struct awm_db_state *st;
  size_t i;
  void *nk, *nv;
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
      nv = malloc(data->size ? data->size : 1);
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
        (struct awm_db_kv *)realloc(st->items, ncap * sizeof(*ni));
    if (ni == NULL) {
      return RET_ERROR;
    }
    st->items = ni;
    st->cap = ncap;
  }
  nk = malloc(key->size ? key->size : 1);
  nv = malloc(data->size ? data->size : 1);
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

static int awm_db_del(const DB *db, const DBT *key, unsigned int flags) {
  struct awm_db_state *st;
  size_t i, j;
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

static int awm_db_sync(const DB *db, unsigned int flags) {
  (void)db;
  (void)flags;
  return RET_SUCCESS;
}

static int awm_db_fd(const DB *db) {
  (void)db;
  errno = ENOENT;
  return -1;
}

DB *dbopen(const char *file, int flags, int mode, DBTYPE type,
           const void *openinfo) {
  DB *db;
  struct awm_db_state *st;
  (void)file;
  (void)flags;
  (void)mode;
  (void)openinfo;
  db = (DB *)calloc(1, sizeof(*db));
  if (db == NULL) {
    return NULL;
  }
  st = (struct awm_db_state *)calloc(1, sizeof(*st));
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

DBM *dbm_open(const char *file, int open_flags, mode_t file_mode) {
  return dbopen(file, open_flags, (int)file_mode, DB_HASH, NULL);
}

void dbm_close(DBM *db) {
  if (db != NULL) {
    db->close(db);
  }
}

datum dbm_fetch(DBM *db, datum key) {
  DBT k, v;
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

datum dbm_firstkey(DBM *db) {
  DBT k, v;
  datum res;
  res.dptr = NULL;
  res.dsize = 0;
  if (db != NULL && db->seq(db, &k, &v, R_FIRST) == RET_SUCCESS) {
    res.dptr = (char *)k.data;
    res.dsize = (int)k.size;
  }
  return res;
}

datum dbm_nextkey(DBM *db) {
  DBT k, v;
  datum res;
  res.dptr = NULL;
  res.dsize = 0;
  if (db != NULL && db->seq(db, &k, &v, R_NEXT) == RET_SUCCESS) {
    res.dptr = (char *)k.data;
    res.dsize = (int)k.size;
  }
  return res;
}

int dbm_delete(DBM *db, datum key) {
  DBT k;
  if (db == NULL) {
    return -1;
  }
  k.data = key.dptr;
  k.size = (size_t)key.dsize;
  return (db->del(db, &k, 0) == RET_SUCCESS) ? 0 : -1;
}

int dbm_store(DBM *db, datum key, datum content, int store_mode) {
  DBT k, v;
  if (db == NULL) {
    return -1;
  }
  k.data = key.dptr;
  k.size = (size_t)key.dsize;
  v.data = content.dptr;
  v.size = (size_t)content.dsize;
  return db->put(db, &k, &v, (store_mode == DBM_INSERT) ? R_NOOVERWRITE : 0);
}

int dbm_error(DBM *db) {
  (void)db;
  return 0;
}

int dbm_clearerr(DBM *db) {
  (void)db;
  return 0;
}

int dbm_dirfno(DBM *db) { return (db != NULL) ? db->fd(db) : -1; }

typedef int make_iso_compilers_happy_tu_bsd_db;

#endif
typedef int make_iso_compilers_happy_mega_bsd_db;
