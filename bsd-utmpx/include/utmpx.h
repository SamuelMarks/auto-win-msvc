#ifndef BSD_UTMPX_H
#define BSD_UTMPX_H

/**
 * @file utmpx.h
 * @brief FreeBSD/POSIX user accounting database (<utmpx.h>) polyfill.
 */

/* clang-format off */
#include <sys/types.h>
#include <sys/time.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EMPTY
/** @brief No valid user accounting information. */
#define EMPTY 0
#endif
#ifndef BOOT_TIME
/** @brief Time of system boot. */
#define BOOT_TIME 1
#endif
#ifndef OLD_TIME
/** @brief Time before system clock change. */
#define OLD_TIME 2
#endif
#ifndef NEW_TIME
/** @brief Time after system clock change. */
#define NEW_TIME 3
#endif
#ifndef USER_PROCESS
/** @brief Normal user process. */
#define USER_PROCESS 4
#endif
#ifndef INIT_PROCESS
/** @brief Process spawned by init. */
#define INIT_PROCESS 5
#endif
#ifndef LOGIN_PROCESS
/** @brief Session leader for user login. */
#define LOGIN_PROCESS 6
#endif
#ifndef DEAD_PROCESS
/** @brief Terminated session leader. */
#define DEAD_PROCESS 7
#endif
#ifndef SHUTDOWN_TIME
/** @brief Time of system shutdown. */
#define SHUTDOWN_TIME 8
#endif

/** @brief Active login sessions database selector. */
#define UTXDB_ACTIVE 0
/** @brief Last login database selector. */
#define UTXDB_LASTLOGIN 1
/** @brief Login history log database selector. */
#define UTXDB_LOG 2

/** @brief User accounting record. */
struct utmpx {
  /** @brief Type of entry. */
  short ut_type;
  /** @brief Timestamp of entry modification. */
  struct timeval ut_tv;
  /** @brief Terminal identifier. */
  char ut_id[8];
  /** @brief Process ID. */
  pid_t ut_pid;
  /** @brief User login name. */
  char ut_user[32];
  /** @brief Device name. */
  char ut_line[16];
  /** @brief Remote hostname. */
  char ut_host[128];
};

/** @brief Rewind user accounting database. */
void setutxent(void);
/** @brief Close user accounting database. */
void endutxent(void);
/** @brief Read next entry from user accounting database. */
struct utmpx *getutxent(void);
/** @brief Search user accounting database by ID/type. */
struct utmpx *getutxid(const struct utmpx *id);
/** @brief Search user accounting database by line. */
struct utmpx *getutxline(const struct utmpx *line);
/** @brief Write entry to user accounting database. */
struct utmpx *pututxline(const struct utmpx *utmpx);
/** @brief Select active/lastlogin/log user accounting database. */
int setutxdb(int db, const char *file);
/** @brief Find user accounting entry by username. */
struct utmpx *getutxuser(const char *user);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_UTMPX_H */
