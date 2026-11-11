#ifndef BSD_DB_H
#define BSD_DB_H

/**
 * @file db.h
 * @brief BSD 1.85 database interface (<db.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#include <stddef.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define RET_ERROR -1
#define RET_SUCCESS 0
#define RET_SPECIAL 1

#define R_CURSOR 1
#define __R_UNUSED 2
#define R_FIRST 3
#define R_IAFTER 4
#define R_IBEFORE 5
#define R_LAST 6
#define R_NEXT 7
#define R_NOOVERWRITE 8
#define R_PREV 9
#define R_SETCURSOR 10
#define R_RECNOSYNC 11

/** @brief Database type enumeration. */
typedef enum { DB_BTREE, DB_HASH, DB_RECNO } DBTYPE;

/** @brief Key/data datum structure for BSD db. */
typedef struct {
  void *data;
  size_t size;
} DBT;

#define R_DUP 0x01

/** @brief B-tree configuration info structure. */
typedef struct {
  unsigned long flags;
  unsigned int cachesize;
  int maxkeypage;
  int minkeypage;
  unsigned int psize;
  int (*compare)(const DBT *, const DBT *);
  size_t (*prefix)(const DBT *, const DBT *);
  int lorder;
} BTREEINFO;

/** @brief Hash table configuration info structure. */
typedef struct {
  unsigned int bsize;
  unsigned int ffactor;
  unsigned int nelem;
  unsigned int cachesize;
  uint32_t (*hash)(const void *, size_t);
  int lorder;
} HASHINFO;

#define R_FIXEDLEN 0x01
#define R_NOKEY 0x02
#define R_SNAPSHOT 0x04

/** @brief Record-number database configuration info structure. */
typedef struct {
  unsigned long flags;
  unsigned int cachesize;
  unsigned int psize;
  int lorder;
  size_t reclen;
  unsigned char bval;
  char *bfname;
} RECNOINFO;

/** @brief BSD 1.85 database handle with method dispatch pointers. */
typedef struct __db {
  DBTYPE type;
  int (*close)(struct __db *);
  int (*del)(const struct __db *, const DBT *, unsigned int);
  int (*get)(const struct __db *, const DBT *, DBT *, unsigned int);
  int (*put)(const struct __db *, DBT *, const DBT *, unsigned int);
  int (*seq)(const struct __db *, DBT *, DBT *, unsigned int);
  int (*sync)(const struct __db *, unsigned int);
  void *internal;
  int (*fd)(const struct __db *);
} DB;

/**
 * @brief Open or create a BSD 1.85 database (in-memory or file-backed).
 * @param[in] file Database file path, or NULL for in-memory database.
 * @param[in] flags Open flags.
 * @param[in] mode File permissions mode.
 * @param[in] type DB_BTREE, DB_HASH, or DB_RECNO.
 * @param[in] openinfo Optional BTREEINFO, HASHINFO, or RECNOINFO pointer.
 * @return Pointer to DB handle, or NULL on error.
 */
DB *dbopen(const char *file, int flags, int mode, DBTYPE type,
           const void *openinfo);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_DB_H */
