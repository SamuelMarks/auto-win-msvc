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

#define EMPTY 0
#define BOOT_TIME 1
#define OLD_TIME 2
#define NEW_TIME 3
#define USER_PROCESS 4
#define INIT_PROCESS 5
#define LOGIN_PROCESS 6
#define DEAD_PROCESS 7
#define SHUTDOWN_TIME 8

#define UTXDB_ACTIVE 0
#define UTXDB_LASTLOGIN 1
#define UTXDB_LOG 2

/** @brief User accounting record structure. */
struct utmpx {
  short ut_type;
  struct timeval ut_tv;
  char ut_id[8];
  pid_t ut_pid;
  char ut_user[32];
  char ut_line[16];
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
