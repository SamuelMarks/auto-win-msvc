#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)
/**
 * @file bsd_utmpx.c
 * @brief Implementation of <utmpx.h> and <ttyent.h> polyfills.
 */

/* clang-format off */
#include <utmpx.h>
#include <ttyent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/* clang-format on */

#ifndef BSD_UTMPX_MOCK_getenv
#define BSD_UTMPX_MOCK_getenv getenv
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400
#ifndef BSD_UTMPX_MOCK_getenv_s
#define BSD_UTMPX_MOCK_getenv_s getenv_s
#endif
#endif

static int g_utx_idx = 0;
static struct utmpx g_utx_entry;

static void init_utx_entry(void) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
  char u_buf[64];
  size_t req = 0;
  const char *u = "root";
  if (BSD_UTMPX_MOCK_getenv_s(&req, u_buf, sizeof(u_buf), "USERNAME") == 0 &&
      req > 0) {
    u = u_buf;
  } else if (BSD_UTMPX_MOCK_getenv_s(&req, u_buf, sizeof(u_buf), "USER") == 0 &&
             req > 0) {
    u = u_buf;
  }
#else
  const char *u = BSD_UTMPX_MOCK_getenv("USERNAME");
  if (u == NULL) {
    u = BSD_UTMPX_MOCK_getenv("USER");
  }
  if (u == NULL) {
    u = "root";
  }
#endif
  memset(&g_utx_entry, 0, sizeof(g_utx_entry));
  g_utx_entry.ut_type = USER_PROCESS;
  g_utx_entry.ut_pid = 1;
  g_utx_entry.ut_tv.tv_sec = (long)time(NULL);
  g_utx_entry.ut_tv.tv_usec = 0;
#if defined(_MSC_VER) && _MSC_VER >= 1400
  strncpy_s(g_utx_entry.ut_id, sizeof(g_utx_entry.ut_id), "tty1", _TRUNCATE);
  strncpy_s(g_utx_entry.ut_user, sizeof(g_utx_entry.ut_user), u, _TRUNCATE);
  strncpy_s(g_utx_entry.ut_line, sizeof(g_utx_entry.ut_line), "console",
            _TRUNCATE);
  strncpy_s(g_utx_entry.ut_host, sizeof(g_utx_entry.ut_host), "localhost",
            _TRUNCATE);
#else
  strncpy(g_utx_entry.ut_id, "tty1", sizeof(g_utx_entry.ut_id) - 1);
  strncpy(g_utx_entry.ut_user, u, sizeof(g_utx_entry.ut_user) - 1);
  strncpy(g_utx_entry.ut_line, "console", sizeof(g_utx_entry.ut_line) - 1);
  strncpy(g_utx_entry.ut_host, "localhost", sizeof(g_utx_entry.ut_host) - 1);
#endif
}

/** @brief Resets the user accounting database cursor. */
void setutxent(void) { g_utx_idx = 0; }

/** @brief Closes the user accounting database. */
void endutxent(void) { g_utx_idx = 0; }

/** @brief Reads the next user accounting entry. */
struct utmpx *getutxent(void) {
  if (g_utx_idx++ == 0) {
    init_utx_entry();
    return &g_utx_entry;
  }
  return NULL;
}

/** @brief Searches for a user accounting entry matching id. */
struct utmpx *getutxid(const struct utmpx *id) {
  if (id == NULL) {
    return NULL;
  }
  init_utx_entry();
  return &g_utx_entry;
}

/** @brief Searches for a user accounting entry matching ut_line. */
struct utmpx *getutxline(const struct utmpx *line) {
  if (line == NULL) {
    return NULL;
  }
  init_utx_entry();
  return &g_utx_entry;
}

/** @brief Writes a user accounting entry. */
struct utmpx *pututxline(const struct utmpx *utmpx) {
  if (utmpx == NULL) {
    return NULL;
  }
  g_utx_entry = *utmpx;
  return &g_utx_entry;
}

/** @brief Selects an alternate user accounting database file. */
int setutxdb(int db, const char *file) {
  (void)db;
  (void)file;
  g_utx_idx = 0;
  return 0;
}

/** @brief Searches for a user accounting entry by username. */
struct utmpx *getutxuser(const char *user) {
  if (user == NULL) {
    return NULL;
  }
  if (g_utx_idx++ == 0) {
    init_utx_entry();
#if defined(_MSC_VER) && _MSC_VER >= 1400
    strncpy_s(g_utx_entry.ut_user, sizeof(g_utx_entry.ut_user), user,
              _TRUNCATE);
#else
    strncpy(g_utx_entry.ut_user, user, sizeof(g_utx_entry.ut_user) - 1);
#endif
    return &g_utx_entry;
  }
  return NULL;
}

static int g_tty_idx = 0;
static char g_tty_name[] = "console";
static char g_tty_getty[] = "none";
static char g_tty_type[] = "vt100";
static char g_tty_comment[] = "System Console";
static struct ttyent g_tty_entry = {
    g_tty_name, g_tty_getty,   g_tty_type, TTY_ON | TTY_SECURE,
    NULL,       g_tty_comment, g_tty_getty};

/** @brief Reads the next entry from the ttys database. */
struct ttyent *getttyent(void) {
  if (g_tty_idx++ == 0) {
    return &g_tty_entry;
  }
  return NULL;
}

/** @brief Searches for a ttys entry by terminal name. */
struct ttyent *getttynam(const char *tty) {
  if (tty == NULL) {
    return NULL;
  }
  return &g_tty_entry;
}

/** @brief Rewinds the ttys database cursor. */
int setttyent(void) {
  g_tty_idx = 0;
  return 1;
}

/** @brief Closes the ttys database. */
int endttyent(void) {
  g_tty_idx = 0;
  return 1;
}

/** @brief Checks whether a terminal is a network pseudo-terminal. */
int isnettty(const char *tty) {
  (void)tty;
  return 0;
}

typedef int make_iso_compilers_happy_tu_bsd_utmpx;

#endif
typedef int make_iso_compilers_happy_mega_bsd_utmpx;
