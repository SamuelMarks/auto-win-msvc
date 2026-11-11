#ifndef BSD_NDBM_H
#define BSD_NDBM_H

/**
 * @file ndbm.h
 * @brief POSIX/BSD <ndbm.h> database interface built on <db.h>.
 */

/* clang-format off */
#include <db.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define DBM_INSERT 0
#define DBM_REPLACE 1

/** @brief Datum structure for ndbm. */
typedef struct {
  char *dptr;
  int dsize;
} datum;

typedef DB DBM;

/** @brief Open an ndbm database. */
DBM *dbm_open(const char *file, int open_flags, mode_t file_mode);
/** @brief Close an ndbm database. */
void dbm_close(DBM *db);
/** @brief Fetch a record from an ndbm database. */
datum dbm_fetch(DBM *db, datum key);
/** @brief Return first key in an ndbm database. */
datum dbm_firstkey(DBM *db);
/** @brief Return next key in an ndbm database. */
datum dbm_nextkey(DBM *db);
/** @brief Delete a key from an ndbm database. */
int dbm_delete(DBM *db, datum key);
/** @brief Store a key/value pair in an ndbm database. */
int dbm_store(DBM *db, datum key, datum content, int store_mode);
/** @brief Return error indicator for an ndbm database. */
int dbm_error(DBM *db);
/** @brief Clear error indicator for an ndbm database. */
int dbm_clearerr(DBM *db);
/** @brief Return underlying file descriptor for an ndbm database. */
int dbm_dirfno(DBM *db);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_NDBM_H */
