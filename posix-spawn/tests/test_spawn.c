/* clang-format off */
#include "greatest.h"
#include "posix-spawn.h"
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#if defined(_MSC_VER) || defined(__MINGW32__)
#if defined(_WIN32) && defined(POSIX_SPAWN_SHARED)
__declspec(dllimport)
#endif
extern void *(*posix_spawn_mock_malloc_ptr)(size_t);

struct mock_proc_info {
  void *hProcess;
  void *hThread;
  unsigned long dwProcessId;
  unsigned long dwThreadId;
};

static int g_spawn_oom = 0;
static ptrdiff_t g_mock_osfh_val = 0x10;
static unsigned long g_mock_mod_len = 0;
static const char *g_mock_mod_str = "busybox.exe";
static int g_mock_cp_mode =
    0; /* 0=fail, 1=succeed, 2=fail first then succeed */
static int g_mock_cp_calls = 0;

static void *spawn_test_malloc(size_t sz) {
  if (g_spawn_oom > 0) {
    g_spawn_oom--;
    if (g_spawn_oom == 0) {
      return NULL;
    }
  }
  return malloc(sz);
}

static ptrdiff_t mock_spawn_get_osfh(int fd) {
  (void)fd;
  return g_mock_osfh_val;
}

static unsigned long mock_spawn_mod_fname(char *buf, unsigned long sz) {
  if (g_mock_mod_len > 0 && g_mock_mod_len < sz && g_mock_mod_str) {
    size_t l = strlen(g_mock_mod_str);
    memcpy(buf, g_mock_mod_str, l + 1);
  }
  return g_mock_mod_len;
}

static int mock_spawn_create_proc(const char *app, char *cmdline,
                                  unsigned long flags, void *env, void *si,
                                  void *pi) {
  struct mock_proc_info *p = (struct mock_proc_info *)pi;
  (void)app;
  (void)cmdline;
  (void)flags;
  (void)env;
  (void)si;
  g_mock_cp_calls++;
  if (g_mock_cp_mode == 0) {
    return 0;
  }
  if (g_mock_cp_mode == 2 && g_mock_cp_calls == 1) {
    return 0;
  }
  p->hProcess = (void *)(size_t)0x2222;
  p->hThread = (void *)(size_t)0x3333;
  p->dwProcessId = 4321UL;
  p->dwThreadId = 4322UL;
  return 1;
}

static int mock_spawn_close_handle(void *h) {
  /* If h is a real kernel file handle (> 0x4000 or not our sentinel), don't
     leak; for tests opening real temp files, we pass NULL for close_handle_fn
     when using real files! */
  (void)h;
  return 1;
}

TEST test_posix_spawn_win32_comprehensive(void) {
  pid_t pid = 0;
  char *argv_complex[9];
  char *argv_empty[2];
  char *argv_many[66];
  char *envp[3];
  posix_spawnattr_t attr;
  posix_spawn_file_actions_t fa;
  int saved_count;
  int i;

  /* Invalid arguments */
  argv_complex[0] = "prog";
  argv_complex[1] = NULL;
  ASSERT_EQ(EINVAL, posix_spawn(&pid, NULL, NULL, NULL, argv_complex, NULL));
  ASSERT_EQ(EINVAL, posix_spawn(&pid, "prog", NULL, NULL, NULL, NULL));
  argv_empty[0] = NULL;
  ASSERT_EQ(EINVAL, posix_spawn(&pid, "prog", NULL, NULL, argv_empty, NULL));

  /* Real CreateProcessA success via cmd.exe /c exit 0 */
  {
    char *cmd_argv[5];
    cmd_argv[0] = "cmd.exe";
    cmd_argv[1] = "/c";
    cmd_argv[2] = "exit";
    cmd_argv[3] = "0";
    cmd_argv[4] = NULL;
    ASSERT_EQ(0, posix_spawnp(&pid, "cmd.exe", NULL, NULL, cmd_argv, NULL));
  }

  /* Safe_get_osfhandle bounds via dup2 from invalid/out-of-range fds using mock
   * CreateProcess */
  posix_spawn_set_win32_hooks(NULL, NULL, mock_spawn_create_proc,
                              mock_spawn_close_handle);
  g_mock_cp_mode = 1;
  posix_spawn_file_actions_init(&fa);
  /* Manually test safe_get_osfhandle(fd >= 2048) and invalid fd 999 via dup2 */
  ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&fa, 2048, 0));
  ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&fa, 999, 1));
  ASSERT_EQ(0, posix_spawn(&pid, "prog", &fa, NULL, argv_complex, NULL));
  posix_spawn_file_actions_destroy(&fa);

  /* Also exercise safe_get_osfhandle(fd < 0) via internal action struct */
  {
    typedef struct {
      int type;
      int fd;
      int new_fd;
      char *path;
      int oflag;
      mode_t mode;
      void *next;
    } raw_act_t;
    raw_act_t raw;
    memset(&raw, 0, sizeof(raw));
    raw.type = 1; /* DUP2 */
    raw.fd = -5;
    raw.new_fd = 0;
    fa.actions = &raw;
    ASSERT_EQ(0, posix_spawn(&pid, "prog", &fa, NULL, argv_complex, NULL));
    fa.actions = NULL;
  }

  /* Command-line quoting: empty, simple, space, tab, newline, vtab, quote,
   * backslashes */
  argv_complex[0] = "prog";
  argv_complex[1] = "";
  argv_complex[2] = "has space\\";
  argv_complex[3] = "tab\there\\\"q";
  argv_complex[4] = "nl\nhere\\x";
  argv_complex[5] = "vt\vhere";
  argv_complex[6] = "plain\"quote";
  argv_complex[7] = "plain_arg";
  argv_complex[8] = NULL;

  envp[0] = "FOO=bar";
  envp[1] = "BAZ=1";
  envp[2] = NULL;

  /* OOM in create_cmdline and create_envblock */
  posix_spawn_mock_malloc_ptr = spawn_test_malloc;
  g_spawn_oom = 1;
  ASSERT_EQ(ENOMEM, posix_spawn(&pid, "prog", NULL, NULL, argv_complex, envp));
  g_spawn_oom = 2;
  ASSERT_EQ(ENOMEM, posix_spawn(&pid, "prog", NULL, NULL, argv_complex, envp));
  posix_spawn_mock_malloc_ptr = NULL;

  /* Attributes: without POSIX_SPAWN_SETPGROUP and with POSIX_SPAWN_SETPGROUP */
  posix_spawnattr_init(&attr);
  ASSERT_EQ(0, posix_spawn(NULL, "prog", NULL, &attr, argv_complex, envp));
  posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETPGROUP);
  ASSERT_EQ(0, posix_spawn(&pid, "prog", NULL, &attr, argv_complex, envp));
  ASSERT_EQ(4321, (int)pid);
  posix_spawnattr_destroy(&attr);

  /* Standard handle fallback when get_osfh returns -1 and -2 */
  posix_spawn_set_win32_hooks(mock_spawn_get_osfh, NULL, mock_spawn_create_proc,
                              mock_spawn_close_handle);
  g_mock_osfh_val = -1;
  ASSERT_EQ(0, posix_spawn(&pid, "prog", NULL, NULL, argv_complex, NULL));
  g_mock_osfh_val = -2;
  ASSERT_EQ(0, posix_spawn(&pid, "prog", NULL, NULL, argv_complex, NULL));

  /* Child table full branch (g_posix_child_count >= 1024) */
  saved_count = posix_spawn_set_child_count(1024);
  ASSERT_EQ(0, posix_spawn(&pid, "prog", NULL, NULL, argv_complex, NULL));
  (void)posix_spawn_set_child_count(saved_count);

  /* File actions: empty initialized file_actions (actions == NULL) */
  posix_spawn_file_actions_init(&fa);
  ASSERT_EQ(0, posix_spawn(&pid, "prog", &fa, NULL, argv_complex, NULL));
  posix_spawn_file_actions_destroy(&fa);

  /* File actions: OPEN (all oflag & fd combos, plus re-open same fd), CLOSE,
     DUP2 Use real CloseHandle so real temp file handles are properly closed! */
  posix_spawn_set_win32_hooks(NULL, NULL, mock_spawn_create_proc, NULL);
  remove("spawn_tmp_0.txt");
  remove("spawn_tmp_1.txt");
  remove("spawn_tmp_2.txt");
  remove("spawn_tmp_3.txt");

  posix_spawn_file_actions_init(&fa);
  /* OPEN fd 1 (O_WRONLY | O_CREAT | O_EXCL -> CREATE_NEW), then re-open fd 1
   * (O_WRONLY | O_CREAT | O_TRUNC -> CREATE_ALWAYS) */
  ASSERT_EQ(0, posix_spawn_file_actions_addopen(&fa, 1, "spawn_tmp_1.txt",
                                                _O_WRONLY | _O_CREAT | _O_EXCL,
                                                0666));
  ASSERT_EQ(0, posix_spawn_file_actions_addopen(&fa, 1, "spawn_tmp_1.txt",
                                                _O_WRONLY | _O_CREAT | _O_TRUNC,
                                                0666));
  /* OPEN fd 0 (O_RDONLY | O_CREAT -> OPEN_ALWAYS) and re-open fd 0 (O_RDONLY ->
   * OPEN_EXISTING) */
  ASSERT_EQ(0, posix_spawn_file_actions_addopen(&fa, 0, "spawn_tmp_0.txt",
                                                _O_RDONLY | _O_CREAT, 0666));
  ASSERT_EQ(0, posix_spawn_file_actions_addopen(&fa, 0, "spawn_tmp_0.txt",
                                                _O_RDONLY, 0666));
  /* OPEN fd 2 (O_RDWR | O_CREAT) and re-open fd 2 (O_RDWR | O_TRUNC ->
   * TRUNCATE_EXISTING) */
  ASSERT_EQ(0, posix_spawn_file_actions_addopen(&fa, 2, "spawn_tmp_2.txt",
                                                _O_RDWR | _O_CREAT, 0666));
  ASSERT_EQ(0, posix_spawn_file_actions_addopen(&fa, 2, "spawn_tmp_2.txt",
                                                _O_RDWR | _O_TRUNC, 0666));
  /* OPEN fd 3 (>2 -> immediately closed) with unusual access mask 3 */
  ASSERT_EQ(0, posix_spawn_file_actions_addopen(
                   &fa, 3, "spawn_tmp_3.txt", (_O_WRONLY | _O_RDWR) | _O_CREAT,
                   0666));
  /* OPEN missing file (h == INVALID_HANDLE_VALUE) */
  ASSERT_EQ(0, posix_spawn_file_actions_addopen(
                   &fa, 0, "Z:\\missing_spawn_file_999.txt", _O_RDONLY, 0666));
  /* DUP2 all src (0, 1, 2, 3) and dst (0, 1, 2, 3) */
  ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&fa, 0, 3));
  ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&fa, 1, 0));
  ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&fa, 2, 1));
  ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&fa, 3, 2));
  /* CLOSE all fds (0, 1, 2, 3) */
  ASSERT_EQ(0, posix_spawn_file_actions_addclose(&fa, 0));
  ASSERT_EQ(0, posix_spawn_file_actions_addclose(&fa, 1));
  ASSERT_EQ(0, posix_spawn_file_actions_addclose(&fa, 2));
  ASSERT_EQ(0, posix_spawn_file_actions_addclose(&fa, 3));

  /* Need mock_close_handle only for pi.hThread/pi.hProcess? Wait: CloseHandle
   * on fake handle returns FALSE harmlessly! */
  ASSERT_EQ(0, posix_spawn(&pid, "prog", &fa, NULL, argv_complex, NULL));
  posix_spawn_file_actions_destroy(&fa);

  remove("spawn_tmp_0.txt");
  remove("spawn_tmp_1.txt");
  remove("spawn_tmp_2.txt");
  remove("spawn_tmp_3.txt");

  /* posix_spawnp fallback branches */
  posix_spawn_set_win32_hooks(NULL, mock_spawn_mod_fname,
                              mock_spawn_create_proc, mock_spawn_close_handle);
  g_mock_cp_mode = 0;

  /* 1. file == NULL */
  ASSERT_EQ(EIO, posix_spawnp(&pid, NULL, NULL, NULL, argv_complex, NULL));
  /* 2. argv == NULL / argv[0] == NULL */
  ASSERT_EQ(EINVAL, posix_spawnp(&pid, "echo", NULL, NULL, NULL, NULL));
  ASSERT_EQ(EINVAL, posix_spawnp(&pid, "echo", NULL, NULL, argv_empty, NULL));
  /* 3. Excluded names & slashes */
  ASSERT_EQ(EIO, posix_spawnp(&pid, "nonexistent_cmd_xyz_987", NULL, NULL,
                              argv_complex, NULL));
  ASSERT_EQ(EIO,
            posix_spawnp(&pid, "no_such_cmd", NULL, NULL, argv_complex, NULL));
  ASSERT_EQ(EIO,
            posix_spawnp(&pid, "missing_cmd", NULL, NULL, argv_complex, NULL));
  ASSERT_EQ(EIO, posix_spawnp(&pid, "sub/cmd", NULL, NULL, argv_complex, NULL));
  ASSERT_EQ(EIO,
            posix_spawnp(&pid, "sub\\cmd", NULL, NULL, argv_complex, NULL));

  /* 4. mod_len == 0 */
  g_mock_mod_len = 0;
  ASSERT_EQ(EIO, posix_spawnp(&pid, "echo", NULL, NULL, argv_complex, NULL));

  /* 5. mod_len > 0, non-busybox exe */
  g_mock_mod_str = "C:\\normal_runner.exe";
  g_mock_mod_len = (unsigned long)strlen(g_mock_mod_str);
  ASSERT_EQ(EIO, posix_spawnp(&pid, "echo", NULL, NULL, argv_complex, NULL));

  /* 6. busybox (lowercase) with > 62 args -> hits k < 62 cap! */
  for (i = 0; i < 65; i++) {
    argv_many[i] = "arg";
  }
  argv_many[65] = NULL;
  g_mock_mod_str = "C:\\tools\\busybox.exe";
  g_mock_mod_len = (unsigned long)strlen(g_mock_mod_str);
  g_mock_cp_mode = 2;
  g_mock_cp_calls = 0;
  ASSERT_EQ(0, posix_spawnp(&pid, "echo", NULL, NULL, argv_many, NULL));

  /* 7. BUSYBOX (uppercase) with short argv -> hits argv[k] == NULL exit */
  g_mock_mod_str = "C:\\tools\\BUSYBOX.EXE";
  g_mock_mod_len = (unsigned long)strlen(g_mock_mod_str);
  g_mock_cp_mode = 2;
  g_mock_cp_calls = 0;
  ASSERT_EQ(0, posix_spawnp(&pid, "echo", NULL, NULL, argv_complex, NULL));

  posix_spawn_set_win32_hooks(NULL, NULL, NULL, NULL);
  PASS();
}
#endif

TEST test_posix_spawn_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;

  rc = posix_spawn_init(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_spawn_init(&status);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, status);
  PASS();
}

TEST test_spawn(void) {
  pid_t pid;
  char *argv[2];
  int rc;

  argv[0] = "";
  argv[1] = NULL;
  rc = posix_spawn(&pid, "", NULL, NULL, argv, NULL);
  ASSERT(rc != 0);
  PASS();
}

TEST test_posix_spawn(void) {
  pid_t pid;
  char *argv[2];
  int rc;

  argv[0] = "nonexistent_executable_12345.exe";
  argv[1] = NULL;

  rc = posix_spawn(&pid, argv[0], NULL, NULL, argv, NULL);
  ASSERT(rc != 0);
  PASS();
}

TEST test_posix_spawn_file_actions_init(void) {
  posix_spawn_file_actions_t fa;
  ASSERT(posix_spawn_file_actions_init(NULL) != 0);
  ASSERT_EQ(0, posix_spawn_file_actions_init(&fa));
  posix_spawn_file_actions_destroy(&fa);
  PASS();
}

TEST test_posix_spawn_file_actions_destroy(void) {
  posix_spawn_file_actions_t fa;
  ASSERT(posix_spawn_file_actions_destroy(NULL) != 0);
  ASSERT_EQ(0, posix_spawn_file_actions_init(&fa));
  ASSERT_EQ(0, posix_spawn_file_actions_destroy(&fa));
  PASS();
}

TEST test_posix_spawn_file_actions_addclose(void) {
  posix_spawn_file_actions_t fa;
  posix_spawn_file_actions_init(&fa);
  ASSERT(posix_spawn_file_actions_addclose(NULL, 3) != 0);
  ASSERT(posix_spawn_file_actions_addclose(&fa, -1) != 0);
  ASSERT_EQ(0, posix_spawn_file_actions_addclose(&fa, 3));
  posix_spawn_file_actions_destroy(&fa);
  PASS();
}

TEST test_posix_spawn_file_actions_adddup2(void) {
  posix_spawn_file_actions_t fa;
  posix_spawn_file_actions_init(&fa);
  ASSERT(posix_spawn_file_actions_adddup2(NULL, 1, 2) != 0);
  ASSERT(posix_spawn_file_actions_adddup2(&fa, -1, 2) != 0);
  ASSERT(posix_spawn_file_actions_adddup2(&fa, 1, -1) != 0);
  ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&fa, 1, 2));
  posix_spawn_file_actions_destroy(&fa);
  PASS();
}

TEST test_posix_spawn_file_actions_addopen(void) {
  posix_spawn_file_actions_t fa;
  posix_spawn_file_actions_init(&fa);
  ASSERT(posix_spawn_file_actions_addopen(NULL, 0, "dummy.txt", 0, 0666) != 0);
  ASSERT(posix_spawn_file_actions_addopen(&fa, -1, "dummy.txt", 0, 0666) != 0);
  ASSERT(posix_spawn_file_actions_addopen(&fa, 0, NULL, 0, 0666) != 0);
  ASSERT_EQ(0, posix_spawn_file_actions_addopen(&fa, 0, "dummy.txt", 0, 0666));
  posix_spawn_file_actions_destroy(&fa);
  PASS();
}

TEST test_posix_spawn_file_actions_multiple(void) {
  posix_spawn_file_actions_t fa;
  posix_spawn_file_actions_init(&fa);
  ASSERT_EQ(0, posix_spawn_file_actions_addopen(&fa, 0, "dummy.txt", 0, 0666));
  ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&fa, 0, 1));
  ASSERT_EQ(0, posix_spawn_file_actions_addclose(&fa, 2));
  posix_spawn_file_actions_destroy(&fa);
  PASS();
}

SUITE(suite_posix_spawn_spawn) {
  RUN_TEST(test_posix_spawn_init);
  RUN_TEST(test_spawn);
  RUN_TEST(test_posix_spawn);
  RUN_TEST(test_posix_spawn_file_actions_init);
  RUN_TEST(test_posix_spawn_file_actions_destroy);
  RUN_TEST(test_posix_spawn_file_actions_addclose);
  RUN_TEST(test_posix_spawn_file_actions_adddup2);
  RUN_TEST(test_posix_spawn_file_actions_addopen);
  RUN_TEST(test_posix_spawn_file_actions_multiple);
#if defined(_MSC_VER) || defined(__MINGW32__)
  RUN_TEST(test_posix_spawn_win32_comprehensive);
#endif
}
