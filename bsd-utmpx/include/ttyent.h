#ifndef BSD_TTYENT_H
#define BSD_TTYENT_H

/**
 * @file ttyent.h
 * @brief BSD terminal configuration database (<ttyent.h>) polyfill.
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Path to system ttys configuration file. */
#define _PATH_TTYS "/etc/ttys"

/** @brief Terminal login enabled flag. */
#define TTY_ON 0x01
/** @brief Root login permitted flag. */
#define TTY_SECURE 0x02
/** @brief Dialup line flag. */
#define TTY_DIALUP 0x04
/** @brief Network terminal flag. */
#define TTY_NETWORK 0x08

/** @brief BSD terminal table entry. */
struct ttyent {
  /** @brief Character device name. */
  char *ty_name;
  /** @brief Command to execute (usually getty). */
  char *ty_getty;
  /** @brief Terminal type for termcap/terminfo. */
  char *ty_type;
  /** @brief Status flags bitmask. */
  int ty_status;
  /** @brief Window manager command. */
  char *ty_window;
  /** @brief Comment field. */
  char *ty_comment;
  /** @brief Terminal group name. */
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
