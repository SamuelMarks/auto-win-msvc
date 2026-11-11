#ifndef BSD_TTYENT_H
#define BSD_TTYENT_H

/**
 * @file ttyent.h
 * @brief BSD terminal configuration database (<ttyent.h>) polyfill.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define _PATH_TTYS "/etc/ttys"

#define TTY_ON 0x01
#define TTY_SECURE 0x02
#define TTY_DIALUP 0x04
#define TTY_NETWORK 0x08

/** @brief Terminal database entry structure. */
struct ttyent {
  char *ty_name;
  char *ty_getty;
  char *ty_type;
  int ty_status;
  char *ty_window;
  char *ty_comment;
  char *ty_group;
};

/** @brief Read next entry from /etc/ttys. */
struct ttyent *getttyent(void);
/** @brief Look up /etc/ttys entry by terminal name. */
struct ttyent *getttynam(const char *tty);
/** @brief Rewind /etc/ttys database. */
int setttyent(void);
/** @brief Close /etc/ttys database. */
int endttyent(void);
/** @brief Check whether a terminal status bit is set. */
int isnettty(const char *tty);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_TTYENT_H */
