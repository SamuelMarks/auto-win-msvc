/* clang-format off */
#include "greatest.h"
#include <utmpx.h>
#include <timeconv.h>
#include <ttyent.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

static int g_mock_utmpx_env_mode = 0;

#if defined(_MSC_VER) && _MSC_VER >= 1400
static int mock_utmpx_getenv_s(size_t *pReturnValue, char *buffer,
                               size_t numberOfElements, const char *varname) {
  (void)numberOfElements;
  if (strcmp(varname, "USERNAME") == 0) {
    if (g_mock_utmpx_env_mode == 1) {
      *pReturnValue = 4;
      strcpy_s(buffer, numberOfElements, "bob");
      return 0;
    }
    if (g_mock_utmpx_env_mode == 4) {
      *pReturnValue = 0;
      return 0;
    }
    *pReturnValue = 0;
    return 22;
  }
  if (strcmp(varname, "USER") == 0) {
    if (g_mock_utmpx_env_mode == 2) {
      *pReturnValue = 4;
      strcpy_s(buffer, numberOfElements, "eve");
      return 0;
    }
    if (g_mock_utmpx_env_mode == 4) {
      *pReturnValue = 0;
      return 0;
    }
    *pReturnValue = 0;
    return 22;
  }
  *pReturnValue = 0;
  return 22;
}
#define BSD_UTMPX_MOCK_getenv_s mock_utmpx_getenv_s
#else
static char *mock_utmpx_getenv(const char *name) {
  static char s_bob[] = "bob";
  static char s_eve[] = "eve";
  if (strcmp(name, "USERNAME") == 0) {
    return (g_mock_utmpx_env_mode == 1) ? s_bob : NULL;
  }
  if (strcmp(name, "USER") == 0) {
    return (g_mock_utmpx_env_mode == 2) ? s_eve : NULL;
  }
  return NULL;
}
#define BSD_UTMPX_MOCK_getenv mock_utmpx_getenv
#endif

void mocked_setutxent(void);
void mocked_endutxent(void);
struct utmpx *mocked_getutxent(void);
struct utmpx *mocked_getutxid(const struct utmpx *id);
struct utmpx *mocked_getutxline(const struct utmpx *line);
struct utmpx *mocked_pututxline(const struct utmpx *utmpx);
int mocked_setutxdb(int db, const char *file);
struct utmpx *mocked_getutxuser(const char *user);
struct ttyent *mocked_getttyent(void);
struct ttyent *mocked_getttynam(const char *tty);
int mocked_setttyent(void);
int mocked_endttyent(void);
int mocked_isnettty(const char *tty);
#define setutxent mocked_setutxent
#define endutxent mocked_endutxent
#define getutxent mocked_getutxent
#define getutxid mocked_getutxid
#define getutxline mocked_getutxline
#define pututxline mocked_pututxline
#define setutxdb mocked_setutxdb
#define getutxuser mocked_getutxuser
#define getttyent mocked_getttyent
#define getttynam mocked_getttynam
#define setttyent mocked_setttyent
#define endttyent mocked_endttyent
#define isnettty mocked_isnettty

#include "../src/bsd_utmpx.c"

#undef setutxent
#undef endutxent
#undef getutxent
#undef getutxid
#undef getutxline
#undef pututxline
#undef setutxdb
#undef getutxuser
#undef getttyent
#undef getttynam
#undef setttyent
#undef endttyent
#undef isnettty

TEST test_timeconv_all_inlines(void) {
  ASSERT_EQ(100, (int)_time_to_time32(_time32_to_time(100)));
  ASSERT_EQ(200, (int)_time_to_time64(_time64_to_time(200)));
  ASSERT_EQ(300L, _time_to_long(_long_to_time(300L)));
  ASSERT_EQ(400, _time_to_int(_int_to_time(400)));
  PASS();
}

TEST test_bsd_utmpx_and_ttyent_linked(void) {
  struct utmpx *u;
  struct utmpx query;
  struct ttyent *t;

  memset(&query, 0, sizeof(query));
  query.ut_type = USER_PROCESS;

  setutxent();
  u = getutxent();
  ASSERT(u != NULL);
  ASSERT_EQ(USER_PROCESS, u->ut_type);
  /* Second call exhausts single-entry cursor */
  ASSERT(getutxent() == NULL);
  endutxent();

  ASSERT(getutxid(NULL) == NULL);
  ASSERT(getutxid(&query) != NULL);

  ASSERT(getutxline(NULL) == NULL);
  ASSERT(getutxline(&query) != NULL);

  ASSERT(pututxline(NULL) == NULL);
  ASSERT(pututxline(&query) != NULL);

  ASSERT_EQ(0, setutxdb(UTXDB_ACTIVE, NULL));
  ASSERT(getutxuser(NULL) == NULL);
  u = getutxuser("alice");
  ASSERT(u != NULL);
  ASSERT_STR_EQ("alice", u->ut_user);
  /* Second getutxuser call when g_utx_idx > 0 returns NULL */
  ASSERT(getutxuser("alice") == NULL);

  ASSERT_EQ(1, setttyent());
  t = getttyent();
  ASSERT(t != NULL);
  ASSERT(getttyent() == NULL);
  ASSERT(getttynam(NULL) == NULL);
  ASSERT(getttynam("console") != NULL);
  ASSERT_EQ(1, endttyent());
  ASSERT_EQ(0, isnettty("console"));

  PASS();
}

TEST test_bsd_utmpx_mocked_env_branches(void) {
  struct utmpx *u;
  struct utmpx query;

  memset(&query, 0, sizeof(query));

  /* Mode 1: USERNAME present */
  g_mock_utmpx_env_mode = 1;
  mocked_setutxent();
  u = mocked_getutxent();
  ASSERT(u != NULL);
  ASSERT_STR_EQ("bob", u->ut_user);
  ASSERT(mocked_getutxent() == NULL);

  /* Mode 2: USERNAME absent, USER present */
  g_mock_utmpx_env_mode = 2;
  mocked_setutxent();
  u = mocked_getutxent();
  ASSERT(u != NULL);
  ASSERT_STR_EQ("eve", u->ut_user);

  /* Mode 3: Both USERNAME and USER absent -> "root" fallback */
  g_mock_utmpx_env_mode = 3;
  mocked_setutxent();
  u = mocked_getutxent();
  ASSERT(u != NULL);
  ASSERT_STR_EQ("root", u->ut_user);

  /* Mode 4 (MSVC req == 0 when rc == 0) */
  g_mock_utmpx_env_mode = 4;
  mocked_setutxent();
  u = mocked_getutxent();
  ASSERT(u != NULL);
  ASSERT_STR_EQ("root", u->ut_user);

  /* Exercise remaining mocked functions and branches */
  mocked_endutxent();
  ASSERT(mocked_getutxid(NULL) == NULL);
  ASSERT(mocked_getutxid(&query) != NULL);
  ASSERT(mocked_getutxline(NULL) == NULL);
  ASSERT(mocked_getutxline(&query) != NULL);
  ASSERT(mocked_pututxline(NULL) == NULL);
  ASSERT(mocked_pututxline(&query) != NULL);
  ASSERT_EQ(0, mocked_setutxdb(UTXDB_ACTIVE, NULL));
  ASSERT(mocked_getutxuser(NULL) == NULL);
  ASSERT(mocked_getutxuser("carol") != NULL);
  ASSERT(mocked_getutxuser("carol") == NULL);
  ASSERT_EQ(1, mocked_setttyent());
  ASSERT(mocked_getttyent() != NULL);
  ASSERT(mocked_getttyent() == NULL);
  ASSERT(mocked_getttynam(NULL) == NULL);
  ASSERT(mocked_getttynam("console") != NULL);
  ASSERT_EQ(1, mocked_endttyent());
  ASSERT_EQ(0, mocked_isnettty("console"));

  PASS();
}

SUITE(suite_bsd_utmpx_core) {
  RUN_TEST(test_timeconv_all_inlines);
  RUN_TEST(test_bsd_utmpx_and_ttyent_linked);
  RUN_TEST(test_bsd_utmpx_mocked_env_branches);
}
