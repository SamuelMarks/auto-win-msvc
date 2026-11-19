/**
 * @file test_core_coverage.c
 * @brief Comprehensive unit and branch coverage tests for posix-core.c.
 */

/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#include <errno.h>
#include <fcntl.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#endif
/* clang-format on */

/* --- Cross-platform common hook mocks --- */

static void test_copy_str(char *dst, size_t cap, const char *src) {
#if defined(_MSC_VER)
  strcpy_s(dst, cap, src);
#else
  (void)cap;
  strcpy(dst, src);
#endif
}

static int g_cmn_malloc_fail_at = -1;
static int g_cmn_malloc_calls = 0;
static int g_cmn_open_scratch_fail = 0;
static int g_cmn_fopen_fail = 0;
static int g_cmn_system_rc = 0;
static int g_cmn_use_system_mock = 0;
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
static int g_cmn_mkstemp_fail = 0;
#endif
static struct posix_core_common_hooks g_cmn_def;

static void *mock_cmn_malloc(size_t sz) {
  int idx = g_cmn_malloc_calls++;
  if (g_cmn_malloc_fail_at >= 0 && idx == g_cmn_malloc_fail_at) {
    return NULL;
  }
  return g_cmn_def.malloc_fn(sz);
}

static void mock_cmn_free(void *ptr) { g_cmn_def.free_fn(ptr); }

static int mock_cmn_open_scratch(const char *path) {
  if (g_cmn_open_scratch_fail) {
    errno = EACCES;
    return -1;
  }
  return g_cmn_def.open_scratch_fn(path);
}

static FILE *mock_cmn_fopen(const char *path, const char *mode) {
  if (g_cmn_fopen_fail) {
    errno = EACCES;
    return NULL;
  }
  return g_cmn_def.fopen_fn(path, mode);
}

static int mock_cmn_system(const char *cmd) {
  if (g_cmn_use_system_mock) {
    (void)cmd;
    return g_cmn_system_rc;
  }
  return g_cmn_def.system_fn(cmd);
}

#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
static int mock_cmn_mkstemp(char *tmpl) {
  if (g_cmn_mkstemp_fail) {
    errno = EEXIST;
    return -1;
  }
  return g_cmn_def.mkstemp_fn(tmpl);
}
#endif

static void reset_common_mocks(void) {
  g_cmn_malloc_fail_at = -1;
  g_cmn_malloc_calls = 0;
  g_cmn_open_scratch_fail = 0;
  g_cmn_fopen_fail = 0;
  g_cmn_system_rc = 0;
  g_cmn_use_system_mock = 0;
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
  g_cmn_mkstemp_fail = 0;
#endif
  posix_core_set_common_hooks(NULL);
}

TEST test_core_common_hooks_and_scratch(void) {
  struct posix_core_common_hooks h;
  char buf[64];
  FILE *fp;
  FILE *slots[8];
  int i;

  posix_core_get_default_common_hooks(NULL);
  posix_core_get_default_common_hooks(&g_cmn_def);
  h = g_cmn_def;
  h.malloc_fn = mock_cmn_malloc;
  h.free_fn = mock_cmn_free;
  h.open_scratch_fn = mock_cmn_open_scratch;
  h.fopen_fn = mock_cmn_fopen;
  h.system_fn = mock_cmn_system;
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
  h.mkstemp_fn = mock_cmn_mkstemp;
#endif

  /* Cover default_common_open_scratch failure path */
  ASSERT_EQ(-1, g_cmn_def.open_scratch_fn(
                    "/nonexistent_dir_awm_cov_999/scratch.tmp"));

  /* Cover awm_dos_mkstemp_format_83 all branches */
  ASSERT_EQ(-1, awm_dos_mkstemp_format_83(NULL, sizeof(buf), "aXXXXXX", 0));
  ASSERT_EQ(-1, awm_dos_mkstemp_format_83(buf, sizeof(buf), NULL, 0));
  ASSERT_EQ(-1, awm_dos_mkstemp_format_83(buf, sizeof(buf), "short", 0));
  ASSERT_EQ(-1, awm_dos_mkstemp_format_83(buf, 7, "aXXXXXX", 0));
  ASSERT_EQ(-1, awm_dos_mkstemp_format_83(buf, sizeof(buf), "abcdefg", 0));

  /* Leading ./ and .\ with and without inner slashes */
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "./XXXXXX", 1));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), ".\\XXXXXX", 2));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), ".aXXXXXX", 2));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "./sub/aXXXXXX", 3));
  ASSERT_EQ(0,
            awm_dos_mkstemp_format_83(buf, sizeof(buf), "./sub\\aXXXXXX", 4));
  ASSERT_EQ(0,
            awm_dos_mkstemp_format_83(buf, sizeof(buf), ".\\sub/aXXXXXX", 5));
  ASSERT_EQ(0,
            awm_dos_mkstemp_format_83(buf, sizeof(buf), ".\\sub\\aXXXXXX", 6));

  /* s1 vs s2 ordering: s2 only, s1 > s2, s2 > s1, neither */
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "dir\\aXXXXXX", 7));
  ASSERT_EQ(0,
            awm_dos_mkstemp_format_83(buf, sizeof(buf), "d1\\d2/aXXXXXX", 8));
  ASSERT_EQ(0,
            awm_dos_mkstemp_format_83(buf, sizeof(buf), "d1/d2\\aXXXXXX", 9));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "XXXXXX", 10));

  /* prefix_len == 1 character classes: a-z, A-Z, 0-9, _, other (below/above) */
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "aXXXXXX", 11));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "ZXXXXXX", 12));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "5XXXXXX", 13));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "_XXXXXX", 14));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "-XXXXXX", 15));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "{XXXXXX", 16));

  /* prefix_len == 2 and prefix_len > 3 with all char classes */
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "aZXXXXXX", 17));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "5_-{XXXXXX", 18));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "-{5aXXXXXX", 19));

  /* Cover awm_popen_scratch and awm_pclose_scratch */
  posix_core_set_common_hooks(&h);
  g_cmn_use_system_mock = 1;
  g_cmn_system_rc = 0;

  /* Invalid arguments to awm_popen_scratch */
  ASSERT(awm_popen_scratch(NULL, "r") == NULL);
  ASSERT(awm_popen_scratch("cmd", NULL) == NULL);
  ASSERT(awm_popen_scratch("cmd", "x") == NULL);
  ASSERT(awm_popen_scratch("cmd", "rx") == NULL);
  ASSERT(awm_popen_scratch("cmd", "rbb") == NULL);
  ASSERT(awm_popen_scratch("cmd", "rtt") == NULL);
  ASSERT_EQ(-1, awm_pclose_scratch(NULL));

  /* open_scratch failure */
  g_cmn_open_scratch_fail = 1;
  ASSERT(awm_popen_scratch("cmd", "r") == NULL);
  g_cmn_open_scratch_fail = 0;

  /* Read mode: malloc fail, fopen fail, rb, rt, r */
  g_cmn_malloc_fail_at = 0;
  g_cmn_malloc_calls = 0;
  ASSERT(awm_popen_scratch("cmd", "r") == NULL);
  g_cmn_malloc_fail_at = -1;

  g_cmn_fopen_fail = 1;
  ASSERT(awm_popen_scratch("cmd", "rb") == NULL);
  g_cmn_fopen_fail = 0;

  fp = awm_popen_scratch("cmd", "rb");
  ASSERT(fp != NULL);
  ASSERT_EQ(0, awm_pclose_scratch(fp));

  fp = awm_popen_scratch("cmd", "rt");
  ASSERT(fp != NULL);
  ASSERT_EQ(0, awm_pclose_scratch(fp));

  /* Write mode: fopen fail, malloc fail, wb, wt, pclose malloc fail */
  g_cmn_fopen_fail = 1;
  ASSERT(awm_popen_scratch("cmd", "wb") == NULL);
  g_cmn_fopen_fail = 0;

  g_cmn_malloc_fail_at = 0;
  g_cmn_malloc_calls = 0;
  ASSERT(awm_popen_scratch("cmd", "w") == NULL);
  g_cmn_malloc_fail_at = -1;

  fp = awm_popen_scratch("cmd", "wb");
  ASSERT(fp != NULL);
  ASSERT_EQ(0, awm_pclose_scratch(fp));

  fp = awm_popen_scratch("cmd", "wt");
  ASSERT(fp != NULL);
  g_cmn_malloc_fail_at = 0;
  g_cmn_malloc_calls = 0;
  ASSERT_EQ(-1, awm_pclose_scratch(fp));
  g_cmn_malloc_fail_at = -1;

  /* Exhaust all 8 scratch slots -> EMFILE */
  for (i = 0; i < 8; i++) {
    slots[i] = awm_popen_scratch("cmd", "r");
    ASSERT(slots[i] != NULL);
  }
  ASSERT(awm_popen_scratch("cmd", "r") == NULL);
  ASSERT_EQ(EMFILE, errno);
  for (i = 0; i < 8; i++) {
    ASSERT_EQ(0, awm_pclose_scratch(slots[i]));
  }

  /* Unknown FILE* to awm_pclose_scratch */
  ASSERT_EQ(-1, awm_pclose_scratch(stdout));

  reset_common_mocks();
  PASS();
}

#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
TEST test_core_posix_branches(void) {
  struct posix_core_common_hooks h;
  char tmpl_ok[64];
  char tmpl_bad[64];
  int fd;
#if (defined(__APPLE__) || !defined(__linux__)) && !defined(__CYGWIN__) &&     \
    !defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
  char *argv[2];
  char *envp[1];
  argv[0] = "sh";
  argv[1] = NULL;
  envp[0] = NULL;
  ASSERT_EQ(-1, fexecve(-1, argv, envp));
  ASSERT_EQ(-1, fexecve(0, NULL, envp));
  ASSERT_EQ(-1, fexecve(0, argv, NULL));
  errno = 0;
  ASSERT_EQ(-1, fexecve(0, argv, envp));
  ASSERT_EQ(ENOSYS, errno);
#endif

  posix_core_get_default_common_hooks(&g_cmn_def);
  h = g_cmn_def;
  h.mkstemp_fn = mock_cmn_mkstemp;
  posix_core_set_common_hooks(&h);

  /* posix_mkostemp: NULL, short, non-XXXXXX, mkstemp fail, O_CLOEXEC */
  ASSERT_EQ(-1, posix_mkostemp(NULL, 0));
  test_copy_str(tmpl_bad, sizeof(tmpl_bad), "abc");
  ASSERT_EQ(-1, posix_mkostemp(tmpl_bad, 0));
  test_copy_str(tmpl_bad, sizeof(tmpl_bad), "abcdef123456");
  ASSERT_EQ(-1, posix_mkostemp(tmpl_bad, 0));

  g_cmn_mkstemp_fail = 1;
  test_copy_str(tmpl_ok, sizeof(tmpl_ok), "awm_mkostemp_fail_XXXXXX");
  ASSERT_EQ(-1, posix_mkostemp(tmpl_ok, 0));
  g_cmn_mkstemp_fail = 0;

  test_copy_str(tmpl_ok, sizeof(tmpl_ok), "awm_mkostemp_ok_XXXXXX");
#ifdef O_CLOEXEC
  fd = posix_mkostemp(tmpl_ok, O_CLOEXEC);
#else
  fd = posix_mkostemp(tmpl_ok, 0);
#endif
  ASSERT(fd >= 0);
  close(fd);
  remove(tmpl_ok);

  reset_common_mocks();
  PASS();
}
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)

/* Extern socket helpers exported by posix-sockets / posix_read_write */
extern void posix_set_dup_socket(intptr_t fd, intptr_t sock);
extern void posix_register_crt_socket(intptr_t crt_fd, intptr_t sock);
extern void set_nonblock(uintptr_t s, int nb);
extern int get_nonblock(uintptr_t s);

static struct posix_core_win32_hooks g_w32_def;

/* --- Win32 mock state --- */
static int g_w32_malloc_fail_at = -1;
static int g_w32_malloc_calls = 0;
static int g_w32_realloc_fail = 0;

static int g_w32_use_osfh = 0;
static intptr_t g_w32_osfh_default = 100;
static int g_w32_osfh_map_enabled = 0;
static intptr_t g_w32_osfh_map[160];

static int g_w32_open_osfh_ret = -2;
static int g_w32_dup_ret = -2;
static int g_w32_dup_fail_fd = -1;
static int g_w32_dup2_ret = -2;
static int g_w32_close_ret = 0;
static int g_w32_commit_ret = 0;
static int g_w32_locking_ret = 0;
static int g_w32_pipe_ret = -1;
static int g_w32_isatty_ret = 0;
static int g_w32_open_rdwr_ret = 5;
static int g_w32_chsize_ret = 0;
static int g_w32_access_ret = 0;
static int g_w32_chmod_ret = 0;
static int g_w32_unlink_ret = 0;
static int g_w32_rmdir_ret = 0;
static int g_w32_getcwd_mode = 0;
static int g_w32_chdir_ret = 0;
static int g_w32_fullpath_null = 0;
static int g_w32_mktemp_ret = 0;
static intptr_t g_w32_execve_ret = -1;
static intptr_t g_w32_spawnve_ret = 0;
static intptr_t g_w32_spawnvpe_ret = 0;
static intptr_t g_w32_spawnv_ret = 0;
static int g_w32_raise_calls = 0;

static int g_w32_mod_advapi_null = 0;
static int g_w32_mod_k32_null = 0;
static int g_w32_mod_ntdll_null = 0;
static int g_w32_mod_ucrt_null = 0;
static const char *g_w32_null_proc_name = NULL;

static unsigned long g_w32_last_error = 0;
static int g_w32_get_fsize_ok = 1;
static posix_core_i64_t g_w32_fsize_val = 100;
static int g_w32_set_fp_fail_at = -1;
static int g_w32_set_fp_calls = 0;
static posix_core_i64_t g_w32_fp_pos = 10;
static int g_w32_set_eof_ok = 1;
static int g_w32_lock_ok = 1;
static int g_w32_unlock_ok = 1;
static int g_w32_cfm_ok = 1;
static int g_w32_mvof_ok = 1;
static int g_w32_create_timer_invoke = 0;
static unsigned long g_w32_tick_count = 1000;
static unsigned long g_w32_file_attrs = 0x80UL;
static int g_w32_get_cur_dir_ok = 1;
static int g_w32_create_dir_ok = 1;
static int g_w32_create_file_ok = 1;
static int g_w32_read_file_ok = 1;
static unsigned long g_w32_read_file_bytes = 4;
static int g_w32_write_file_ok = 1;
static unsigned long g_w32_write_file_bytes = 4;
static int g_w32_dev_io_ok = 1;
static unsigned long g_w32_reparse_tag = 0xA000000CUL;
static int g_w32_wctmb_mode = 0;
static int g_w32_wctmb_calls = 0;
static int g_w32_hard_link_ok = 1;
static int g_w32_move_file_ok = 1;
static int g_w32_copy_file_fail_count = 0;
static int g_w32_get_user_ok = 1;
static unsigned long g_w32_drives_mask = 0;
static unsigned int g_w32_drive_type = 3U;
static unsigned long g_w32_page_size = 4096UL;
static unsigned long g_w32_num_procs = 4UL;
static int g_w32_memstat_ok = 1;
static const char *g_w32_mod_filename = "C:\\bin\\busybox.exe";
static int g_w32_mod_filename_fail = 0;
static int g_w32_get_exit_code_ok = 1;
static unsigned long g_w32_exit_code_val = 7;
static int g_w32_ioctl_fail_count = 0;
static int g_w32_wsa_err = 0;
static int g_w32_socketpair_ret = 0;
static int g_w32_crypt_fail_step = 0;
static int g_w32_crypt_hash_calls = 0;

/* Dynamic proc mocks */
static int g_w32_open_token_ok = 1;
static unsigned long g_w32_token_len = 16;
static int g_w32_token_info_ok = 1;
static int g_w32_token_sid_null = 0;
static int g_w32_valid_sid_ok = 1;
static int g_w32_subauth_cnt_null = 0;
static unsigned char g_w32_subauth_cnt = 2;
static int g_w32_subauth_null = 0;
static unsigned long g_w32_rid_seq[4] = {1000, 1000, 1000, 1000};
static int g_w32_rid_seq_idx = 0;

static int g_w32_exit_proc_calls = 0;
static int g_w32_term_proc_calls = 0;
static int g_w32_final_path_mode = 0;
static int g_w32_symlink_fail_count = 0;
static unsigned long g_w32_clone_status = 0;
static int g_w32_vprotect_ok = 1;
static unsigned char g_w32_fake_exit_code[2][32];
static unsigned char g_w32_fake_map_buf[64];

static int __stdcall mock_open_process_token(void *p, unsigned long acc,
                                             void **out_tok) {
  (void)p;
  (void)acc;
  if (!g_w32_open_token_ok) {
    return 0;
  }
  *out_tok = (void *)(intptr_t)0x5555;
  return 1;
}

static int __stdcall mock_get_token_info(void *tok, int cls, void *buf,
                                         unsigned long len,
                                         unsigned long *ret_len) {
  (void)tok;
  (void)cls;
  if (buf == NULL) {
    *ret_len = g_w32_token_len;
    return 0;
  }
  if (!g_w32_token_info_ok) {
    return 0;
  }
  if (len >= sizeof(void *)) {
    *(void **)buf = g_w32_token_sid_null ? NULL : (void *)(intptr_t)0x6666;
  }
  *ret_len = g_w32_token_len;
  return 1;
}

static int __stdcall mock_is_valid_sid(void *sid) {
  (void)sid;
  return g_w32_valid_sid_ok;
}

static unsigned char *__stdcall mock_get_sid_subauth_cnt(void *sid) {
  (void)sid;
  if (g_w32_subauth_cnt_null) {
    return NULL;
  }
  return &g_w32_subauth_cnt;
}

static void *__stdcall mock_get_sid_subauth(void *sid, unsigned long idx) {
  int pos;
  (void)sid;
  (void)idx;
  if (g_w32_subauth_null) {
    return NULL;
  }
  pos = g_w32_rid_seq_idx;
  if (g_w32_rid_seq_idx < 3) {
    g_w32_rid_seq_idx++;
  }
  return (void *)&g_w32_rid_seq[pos];
}

static void __stdcall mock_exit_process(unsigned int code) {
  (void)code;
  g_w32_exit_proc_calls++;
}

static int __stdcall mock_terminate_process(void *h, unsigned int code) {
  (void)h;
  (void)code;
  g_w32_term_proc_calls++;
  return 1;
}

static unsigned long __stdcall
mock_get_final_path(void *h, char *buf, unsigned long cap, unsigned long fl) {
  const char *s = NULL;
  unsigned long n;
  (void)h;
  (void)fl;
  if (g_w32_final_path_mode == 2) {
    return 0;
  }
  if (g_w32_final_path_mode == 3) {
    return cap;
  }
  if (g_w32_final_path_mode == 0) {
    s = "\\\\?\\C:\\dir";
  } else if (g_w32_final_path_mode == 1) {
    s = "C:\\dir";
  } else if (g_w32_final_path_mode == 4) {
    s = "\\\\?\\";
  } else if (g_w32_final_path_mode == 5) {
    s = "C:\\dir\\";
  } else if (g_w32_final_path_mode == 6) {
    s = "C:/dir/";
  } else if (g_w32_final_path_mode == 7) {
    memset(buf, 'A', cap - 2);
    buf[cap - 2] = '\0';
    return cap - 2;
  } else if (g_w32_final_path_mode == 8) {
    s = "\\\\x\\C:\\dir";
  } else if (g_w32_final_path_mode == 9) {
    s = "\\x?\\C:\\dir";
  } else if (g_w32_final_path_mode == 10) {
    s = "\\\\?xC:\\dir";
  } else {
    s = "ab";
  }
  n = (unsigned long)strlen(s);
  memcpy(buf, s, n + 1);
  return n;
}

static int __stdcall mock_prefetch_vm(void *h, size_t n, void *entries,
                                      unsigned long fl) {
  (void)h;
  (void)n;
  (void)entries;
  (void)fl;
  return 1;
}

static unsigned char __stdcall mock_create_symlink(const char *lpLink,
                                                   const char *lpTarget,
                                                   unsigned long fl) {
  (void)lpLink;
  (void)lpTarget;
  (void)fl;
  if (g_w32_symlink_fail_count > 0) {
    g_w32_symlink_fail_count--;
    return 0;
  }
  return 1;
}

typedef struct _MOCK_RTL_INFO {
  unsigned long Length;
  void *Process;
  void *Thread;
  void *UniqueProcess;
  void *UniqueThread;
} MOCK_RTL_INFO;

static unsigned long __stdcall mock_rtl_clone(unsigned long fl, void *p1,
                                              void *p2, void *p3, void *info) {
  MOCK_RTL_INFO *ri = (MOCK_RTL_INFO *)info;
  (void)fl;
  (void)p1;
  (void)p2;
  (void)p3;
  if (ri) {
    ri->Process = (void *)(intptr_t)0x1010;
    ri->Thread = (void *)(intptr_t)0x2020;
    ri->UniqueProcess = (void *)(intptr_t)12345;
    ri->UniqueThread = (void *)(intptr_t)12346;
  }
  return g_w32_clone_status;
}

static int __stdcall mock_virtual_protect(void *addr, size_t sz,
                                          unsigned long fl,
                                          unsigned long *oldp) {
  (void)addr;
  (void)sz;
  (void)fl;
  if (oldp) {
    *oldp = 0x20;
  }
  return g_w32_vprotect_ok;
}

/* --- Win32 hook implementations --- */

static void *mock_w32_malloc(size_t sz) {
  int idx = g_w32_malloc_calls++;
  if (g_w32_malloc_fail_at >= 0 && idx == g_w32_malloc_fail_at) {
    return NULL;
  }
  return g_w32_def.malloc_fn(sz);
}

static void *mock_w32_realloc(void *p, size_t sz) {
  if (g_w32_realloc_fail) {
    return NULL;
  }
  return g_w32_def.realloc_fn(p, sz);
}

static void mock_w32_free(void *p) { g_w32_def.free_fn(p); }

static intptr_t mock_w32_get_osfhandle(int fd) {
  if (g_w32_osfh_map_enabled && fd >= 0 && fd < 160) {
    return g_w32_osfh_map[fd];
  }
  if (g_w32_use_osfh) {
    return g_w32_osfh_default;
  }
  return g_w32_def.get_osfhandle_fn(fd);
}

static int mock_w32_open_osfhandle(intptr_t h, int fl) {
  (void)h;
  (void)fl;
  if (g_w32_open_osfh_ret != -2) {
    return g_w32_open_osfh_ret;
  }
  return 7;
}

static int mock_w32_dup(int fd) {
  if (g_w32_dup_fail_fd == fd) {
    return -1;
  }
  if (g_w32_dup_ret != -2) {
    return g_w32_dup_ret;
  }
  return fd + 50;
}

static int mock_w32_dup2(int f1, int f2) {
  (void)f1;
  if (g_w32_dup2_ret != -2) {
    return g_w32_dup2_ret;
  }
  return f2;
}

static int mock_w32_close(int fd) {
  (void)fd;
  return g_w32_close_ret;
}

static int mock_w32_commit(int fd) {
  (void)fd;
  return g_w32_commit_ret;
}

static int mock_w32_locking(int fd, int mode, long nbytes) {
  (void)fd;
  (void)mode;
  (void)nbytes;
  return g_w32_locking_ret;
}

static int mock_w32_pipe(int *pfds, unsigned int psize, int tm) {
  (void)psize;
  (void)tm;
  if (g_w32_pipe_ret == 0 && pfds) {
    pfds[0] = 11;
    pfds[1] = 12;
  }
  return g_w32_pipe_ret;
}

static int mock_w32_isatty(int fd) {
  (void)fd;
  return g_w32_isatty_ret;
}

static int mock_w32_open_rdwr(const char *p) {
  (void)p;
  return g_w32_open_rdwr_ret;
}

static int mock_w32_chsize(int fd, long sz) {
  (void)fd;
  (void)sz;
  return g_w32_chsize_ret;
}

static int mock_w32_access(const char *p, int m) {
  (void)p;
  (void)m;
  return g_w32_access_ret;
}

static int mock_w32_chmod(const char *p, int m) {
  (void)p;
  (void)m;
  return g_w32_chmod_ret;
}

static int mock_w32_unlink(const char *p) {
  (void)p;
  return g_w32_unlink_ret;
}

static int mock_w32_rmdir(const char *p) {
  (void)p;
  return g_w32_rmdir_ret;
}

static char *mock_w32_getcwd(char *buf, int maxlen) {
  if (g_w32_getcwd_mode == 1) {
    return NULL;
  }
  if (g_w32_getcwd_mode == 2) {
    buf[0] = '\0';
    return buf;
  }
  if (g_w32_getcwd_mode == 3) {
    test_copy_str(buf, (size_t)maxlen, "C:\\work\\");
    return buf;
  }
  if (g_w32_getcwd_mode == 5) {
    test_copy_str(buf, (size_t)maxlen, "C:/work/");
    return buf;
  }
  if (g_w32_getcwd_mode == 4) {
    int n = maxlen - 2;
    memset(buf, 'A', (size_t)n);
    buf[n] = '\0';
    return buf;
  }
  test_copy_str(buf, (size_t)maxlen, "C:\\work");
  return buf;
}

static int mock_w32_chdir(const char *p) {
  (void)p;
  return g_w32_chdir_ret;
}

static char *mock_w32_fullpath(char *abs_p, const char *rel_p, size_t max_len) {
  (void)rel_p;
  if (g_w32_fullpath_null) {
    return NULL;
  }
  if (abs_p) {
    test_copy_str(abs_p, max_len, "C:\\resolved");
    return abs_p;
  }
  return (char *)g_w32_def.malloc_fn(16);
}

static int mock_w32_mktemp_s(char *tmpl, size_t sz) {
  (void)tmpl;
  (void)sz;
  return g_w32_mktemp_ret;
}

static intptr_t mock_w32_execve(const char *c, const char *const *a,
                                const char *const *e) {
  (void)c;
  (void)a;
  (void)e;
  return g_w32_execve_ret;
}

static intptr_t mock_w32_spawnve(int mode, const char *c, const char *const *a,
                                 const char *const *e) {
  (void)mode;
  (void)c;
  (void)a;
  (void)e;
  return g_w32_spawnve_ret;
}

static intptr_t mock_w32_spawnvpe(int mode, const char *c, const char *const *a,
                                  const char *const *e) {
  (void)mode;
  (void)c;
  (void)a;
  (void)e;
  return g_w32_spawnvpe_ret;
}

static intptr_t mock_w32_spawnv(int mode, const char *c, const char *const *a) {
  (void)mode;
  (void)c;
  (void)a;
  return g_w32_spawnv_ret;
}

static int mock_w32_raise(int sig) {
  (void)sig;
  g_w32_raise_calls++;
  return 0;
}

static void *mock_w32_get_module_handle(const char *name) {
  if (strcmp(name, "advapi32.dll") == 0) {
    return g_w32_mod_advapi_null ? NULL : (void *)(intptr_t)0x1001;
  }
  if (strcmp(name, "kernel32.dll") == 0) {
    return g_w32_mod_k32_null ? NULL : (void *)(intptr_t)0x1002;
  }
  if (strcmp(name, "ntdll.dll") == 0) {
    return g_w32_mod_ntdll_null ? NULL : (void *)(intptr_t)0x1003;
  }
  if (strcmp(name, "ucrtbase.dll") == 0) {
    return g_w32_mod_ucrt_null ? NULL : (void *)(intptr_t)0x1004;
  }
  return NULL;
}

static void *mock_w32_get_proc_address(void *mod, const char *proc) {
  (void)mod;
  if (g_w32_null_proc_name && strcmp(proc, g_w32_null_proc_name) == 0) {
    return NULL;
  }
  if (strcmp(proc, "OpenProcessToken") == 0) {
    return (void *)(size_t)mock_open_process_token;
  }
  if (strcmp(proc, "GetTokenInformation") == 0) {
    return (void *)(size_t)mock_get_token_info;
  }
  if (strcmp(proc, "IsValidSid") == 0) {
    return (void *)(size_t)mock_is_valid_sid;
  }
  if (strcmp(proc, "GetSidSubAuthorityCount") == 0) {
    return (void *)(size_t)mock_get_sid_subauth_cnt;
  }
  if (strcmp(proc, "GetSidSubAuthority") == 0) {
    return (void *)(size_t)mock_get_sid_subauth;
  }
  if (strcmp(proc, "ExitProcess") == 0) {
    return (void *)(size_t)mock_exit_process;
  }
  if (strcmp(proc, "TerminateProcess") == 0) {
    return (void *)(size_t)mock_terminate_process;
  }
  if (strcmp(proc, "GetFinalPathNameByHandleA") == 0) {
    return (void *)(size_t)mock_get_final_path;
  }
  if (strcmp(proc, "PrefetchVirtualMemory") == 0) {
    return (void *)(size_t)mock_prefetch_vm;
  }
  if (strcmp(proc, "CreateSymbolicLinkA") == 0) {
    return (void *)(size_t)mock_create_symlink;
  }
  if (strcmp(proc, "RtlCloneUserProcess") == 0) {
    return (void *)(size_t)mock_rtl_clone;
  }
  if (strcmp(proc, "VirtualProtect") == 0) {
    return (void *)(size_t)mock_virtual_protect;
  }
  if (strcmp(proc, "_exit") == 0) {
    return (void *)g_w32_fake_exit_code[0];
  }
  if (strcmp(proc, "exit") == 0) {
    return (void *)g_w32_fake_exit_code[1];
  }
  return NULL;
}

static unsigned long mock_w32_get_last_error(void) { return g_w32_last_error; }

static int mock_w32_get_file_size_ex(void *h, posix_core_i64_t *sz) {
  (void)h;
  if (!g_w32_get_fsize_ok) {
    return 0;
  }
  *sz = g_w32_fsize_val;
  return 1;
}

static int mock_w32_set_file_pointer_ex(void *h, posix_core_i64_t dist,
                                        posix_core_i64_t *newp,
                                        unsigned long method) {
  int idx = g_w32_set_fp_calls++;
  (void)h;
  (void)dist;
  (void)method;
  if (g_w32_set_fp_fail_at >= 0 && idx == g_w32_set_fp_fail_at) {
    return 0;
  }
  if (newp) {
    *newp = g_w32_fp_pos;
  }
  return 1;
}

static int mock_w32_set_end_of_file(void *h) {
  (void)h;
  return g_w32_set_eof_ok;
}

static int mock_w32_lock_file_ex(void *h, unsigned long fl, unsigned long res,
                                 unsigned long lo, unsigned long hi, void *ov) {
  (void)h;
  (void)fl;
  (void)res;
  (void)lo;
  (void)hi;
  (void)ov;
  return g_w32_lock_ok;
}

static int mock_w32_unlock_file_ex(void *h, unsigned long res, unsigned long lo,
                                   unsigned long hi, void *ov) {
  (void)h;
  (void)res;
  (void)lo;
  (void)hi;
  (void)ov;
  return g_w32_unlock_ok;
}

static void *mock_w32_create_file_mapping(void *h, void *sa, unsigned long prot,
                                          unsigned long hi, unsigned long lo,
                                          const char *name) {
  (void)h;
  (void)sa;
  (void)prot;
  (void)hi;
  (void)lo;
  (void)name;
  return g_w32_cfm_ok ? (void *)(intptr_t)0x7777 : NULL;
}

static void *mock_w32_map_view_of_file(void *h, unsigned long acc,
                                       unsigned long hi, unsigned long lo,
                                       size_t bytes) {
  (void)h;
  (void)acc;
  (void)hi;
  (void)lo;
  (void)bytes;
  return g_w32_mvof_ok ? (void *)g_w32_fake_map_buf : NULL;
}

static int mock_w32_unmap_view_of_file(const void *p) {
  (void)p;
  return 1;
}

static int mock_w32_create_timer(void **out_t, void *q,
                                 posix_core_timer_cb_t cb, void *param,
                                 unsigned long due, unsigned long period,
                                 unsigned long fl) {
  (void)q;
  (void)due;
  (void)period;
  (void)fl;
  *out_t = (void *)(intptr_t)0x8888;
  if (g_w32_create_timer_invoke && cb) {
    cb(param, 1);
  }
  return 1;
}

static int mock_w32_delete_timer(void *q, void *t, void *ev) {
  (void)q;
  (void)t;
  (void)ev;
  return 1;
}

static unsigned long mock_w32_get_tick_count(void) { return g_w32_tick_count; }

static unsigned long mock_w32_get_file_attributes(const char *p) {
  (void)p;
  return g_w32_file_attrs;
}

static unsigned long mock_w32_get_current_dir(unsigned long len, char *buf) {
  if (!g_w32_get_cur_dir_ok) {
    return 0;
  }
  if (len > 8) {
    test_copy_str(buf, (size_t)len, "C:\\work");
    return 7;
  }
  return 0;
}

static int mock_w32_set_current_dir(const char *p) {
  (void)p;
  return 1;
}

static int mock_w32_create_directory(const char *p, void *sa) {
  (void)p;
  (void)sa;
  return g_w32_create_dir_ok;
}

static void *mock_w32_create_file(const char *n, unsigned long acc,
                                  unsigned long sh, void *sa,
                                  unsigned long disp, unsigned long fl,
                                  void *tmpl) {
  (void)n;
  (void)acc;
  (void)sh;
  (void)sa;
  (void)disp;
  (void)fl;
  (void)tmpl;
  return g_w32_create_file_ok ? (void *)(intptr_t)0x9999 : (void *)(intptr_t)-1;
}

static int mock_w32_close_handle(void *h) {
  (void)h;
  return 1;
}

static int mock_w32_read_file(void *h, void *buf, unsigned long n,
                              unsigned long *br, void *ov) {
  (void)h;
  (void)ov;
  if (!g_w32_read_file_ok) {
    return 0;
  }
  if (buf && n > 0) {
    memset(buf, 'X', n < g_w32_read_file_bytes ? n : g_w32_read_file_bytes);
  }
  if (br) {
    *br = g_w32_read_file_bytes;
  }
  return 1;
}

static int mock_w32_write_file(void *h, const void *buf, unsigned long n,
                               unsigned long *bw, void *ov) {
  (void)h;
  (void)buf;
  (void)n;
  (void)ov;
  if (!g_w32_write_file_ok) {
    return 0;
  }
  if (bw) {
    *bw = g_w32_write_file_bytes;
  }
  return 1;
}

typedef struct _TEST_REPARSE_BUF {
  unsigned long ReparseTag;
  unsigned short ReparseDataLength;
  unsigned short Reserved;
  union {
    struct {
      unsigned short SubstituteNameOffset;
      unsigned short SubstituteNameLength;
      unsigned short PrintNameOffset;
      unsigned short PrintNameLength;
      unsigned long Flags;
      wchar_t PathBuffer[16];
    } sym;
    struct {
      unsigned short SubstituteNameOffset;
      unsigned short SubstituteNameLength;
      unsigned short PrintNameOffset;
      unsigned short PrintNameLength;
      wchar_t PathBuffer[16];
    } mnt;
  } u;
} TEST_REPARSE_BUF;

static int mock_w32_device_io_control(void *h, unsigned long code, void *in_b,
                                      unsigned long in_sz, void *out_b,
                                      unsigned long out_sz,
                                      unsigned long *ret_b, void *ov) {
  TEST_REPARSE_BUF *rb = (TEST_REPARSE_BUF *)out_b;
  (void)h;
  (void)code;
  (void)in_b;
  (void)in_sz;
  (void)out_sz;
  (void)ov;
  if (!g_w32_dev_io_ok) {
    return 0;
  }
  memset(rb, 0, sizeof(*rb));
  rb->ReparseTag = g_w32_reparse_tag;
  if (g_w32_reparse_tag == 0xA000000CUL) {
    rb->u.sym.PrintNameOffset = 0;
    rb->u.sym.PrintNameLength = (unsigned short)(6 * sizeof(wchar_t));
    rb->u.sym.PathBuffer[0] = L't';
    rb->u.sym.PathBuffer[1] = L'a';
    rb->u.sym.PathBuffer[2] = L'r';
    rb->u.sym.PathBuffer[3] = L'g';
    rb->u.sym.PathBuffer[4] = L'e';
    rb->u.sym.PathBuffer[5] = L't';
  } else if (g_w32_reparse_tag == 0xA0000003UL) {
    rb->u.mnt.PrintNameOffset = 0;
    rb->u.mnt.PrintNameLength = (unsigned short)(4 * sizeof(wchar_t));
    rb->u.mnt.PathBuffer[0] = L'j';
    rb->u.mnt.PathBuffer[1] = L'u';
    rb->u.mnt.PathBuffer[2] = L'n';
    rb->u.mnt.PathBuffer[3] = L'c';
  }
  if (ret_b) {
    *ret_b = (unsigned long)sizeof(*rb);
  }
  return 1;
}

static int mock_w32_wide_char_to_multi_byte(unsigned int cp, unsigned long fl,
                                            const wchar_t *ws, int wlen,
                                            char *mb, int mblen,
                                            const char *def_c, int *used_def) {
  int call = g_w32_wctmb_calls++;
  int i;
  (void)cp;
  (void)fl;
  (void)def_c;
  (void)used_def;
  if (g_w32_wctmb_mode == 1 && call == 0) {
    return 0;
  }
  if (g_w32_wctmb_mode == 2 && call == 1) {
    return 0;
  }
  if (mb == NULL) {
    return wlen;
  }
  for (i = 0; i < wlen && i < mblen; i++) {
    mb[i] = (char)ws[i];
  }
  return wlen < mblen ? wlen : mblen;
}

static int mock_w32_create_hard_link(const char *np, const char *op, void *sa) {
  (void)np;
  (void)op;
  (void)sa;
  return g_w32_hard_link_ok;
}

static int mock_w32_move_file_ex(const char *op, const char *np,
                                 unsigned long fl) {
  (void)op;
  (void)np;
  (void)fl;
  return g_w32_move_file_ok;
}

static int mock_w32_copy_file(const char *op, const char *np, int fail_ex) {
  (void)op;
  (void)np;
  (void)fail_ex;
  if (g_w32_copy_file_fail_count > 0) {
    g_w32_copy_file_fail_count--;
    return 0;
  }
  return 1;
}

static int mock_w32_get_user_name(char *buf, unsigned long *sz) {
  if (!g_w32_get_user_ok) {
    return 0;
  }
  if (buf && sz && *sz >= 5) {
    test_copy_str(buf, (size_t)*sz, "user");
    *sz = 5;
  }
  return 1;
}

static unsigned long mock_w32_sleep_ex(unsigned long ms, int alert) {
  (void)ms;
  (void)alert;
  return 0;
}

static unsigned long mock_w32_get_logical_drives(void) {
  return g_w32_drives_mask;
}

static unsigned int mock_w32_get_drive_type(const char *root) {
  (void)root;
  return g_w32_drive_type;
}

static int mock_w32_flush_file_buffers(void *h) {
  (void)h;
  return 1;
}

typedef struct _TEST_SYS_INFO {
  unsigned long dwOemId;
  unsigned long dwPageSize;
  void *lpMin;
  void *lpMax;
  unsigned long *mask;
  unsigned long dwNumberOfProcessors;
  unsigned long dwProcessorType;
  unsigned long dwAllocGran;
  unsigned short wLevel;
  unsigned short wRev;
} TEST_SYS_INFO;

static void mock_w32_get_system_info(void *si) {
  TEST_SYS_INFO *t = (TEST_SYS_INFO *)si;
  t->dwPageSize = g_w32_page_size;
  t->dwNumberOfProcessors = g_w32_num_procs;
}

typedef struct _TEST_MEM_STAT {
  unsigned long dwLength;
  unsigned long dwMemoryLoad;
  posix_core_u64_t ullTotalPhys;
  posix_core_u64_t ullAvailPhys;
  posix_core_u64_t ullTotalPageFile;
  posix_core_u64_t ullAvailPageFile;
  posix_core_u64_t ullTotalVirtual;
  posix_core_u64_t ullAvailVirtual;
  posix_core_u64_t ullAvailExtendedVirtual;
} TEST_MEM_STAT;

static int mock_w32_global_memory_status_ex(void *ms) {
  TEST_MEM_STAT *m = (TEST_MEM_STAT *)ms;
  if (!g_w32_memstat_ok) {
    return 0;
  }
  m->ullTotalPhys = 4096UL * 100UL;
  m->ullAvailPhys = 4096UL * 50UL;
  return 1;
}

static unsigned long mock_w32_get_module_file_name(void *m, char *buf,
                                                   unsigned long sz) {
  unsigned long n;
  (void)m;
  if (g_w32_mod_filename_fail) {
    return 0;
  }
  n = (unsigned long)strlen(g_w32_mod_filename);
  if (n >= sz) {
    n = sz - 1;
  }
  memcpy(buf, g_w32_mod_filename, n);
  buf[n] = '\0';
  return n;
}

static int mock_w32_set_std_handle(unsigned long id, void *h) {
  (void)id;
  (void)h;
  return 1;
}

static int mock_w32_set_handle_information(void *h, unsigned long mask,
                                           unsigned long fl) {
  (void)h;
  (void)mask;
  (void)fl;
  return 1;
}

static unsigned long mock_w32_wait_for_single_object(void *h,
                                                     unsigned long ms) {
  (void)h;
  (void)ms;
  return 0;
}

static int mock_w32_get_exit_code_process(void *h, unsigned long *code) {
  (void)h;
  if (!g_w32_get_exit_code_ok) {
    return 0;
  }
  if (code) {
    *code = g_w32_exit_code_val;
  }
  return 1;
}

static int mock_w32_ioctlsocket(uintptr_t s, long cmd, unsigned long *argp) {
  (void)s;
  (void)cmd;
  (void)argp;
  if (g_w32_ioctl_fail_count > 0) {
    g_w32_ioctl_fail_count--;
    return -1;
  }
  return 0;
}

static int mock_w32_wsa_get_last_error(void) { return g_w32_wsa_err; }

static int mock_w32_socketpair(int d, int t, int p, intptr_t sv[2]) {
  (void)d;
  (void)t;
  (void)p;
  if (g_w32_socketpair_ret == 0 && sv) {
    sv[0] = 21;
    sv[1] = 22;
  }
  return g_w32_socketpair_ret;
}

static int mock_w32_crypt_acquire_context(size_t *prov, const char *c,
                                          const char *p, unsigned long t,
                                          unsigned long fl) {
  (void)c;
  (void)p;
  (void)t;
  (void)fl;
  if (g_w32_crypt_fail_step == 1) {
    return 0;
  }
  *prov = 1;
  return 1;
}

static int mock_w32_crypt_create_hash(size_t prov, unsigned int alg, size_t k,
                                      unsigned long fl, size_t *h) {
  (void)prov;
  (void)alg;
  (void)k;
  (void)fl;
  if (g_w32_crypt_fail_step == 2) {
    return 0;
  }
  *h = 2;
  return 1;
}

static int mock_w32_crypt_hash_data(size_t h, const unsigned char *d,
                                    unsigned long len, unsigned long fl) {
  int call = ++g_w32_crypt_hash_calls;
  (void)h;
  (void)d;
  (void)len;
  (void)fl;
  if (g_w32_crypt_fail_step == 3 && call == 1) {
    return 0;
  }
  if (g_w32_crypt_fail_step == 4 && call == 2) {
    return 0;
  }
  return 1;
}

static int mock_w32_crypt_get_hash_param(size_t h, unsigned long p,
                                         unsigned char *d, unsigned long *len,
                                         unsigned long fl) {
  unsigned long i;
  (void)h;
  (void)p;
  (void)fl;
  if (g_w32_crypt_fail_step == 5) {
    return 0;
  }
  if (d && len) {
    for (i = 0; i < *len; i++) {
      d[i] = (unsigned char)(i + 1);
    }
  }
  return 1;
}

static int mock_w32_crypt_destroy_hash(size_t h) {
  (void)h;
  return 1;
}

static int mock_w32_crypt_release_context(size_t p, unsigned long fl) {
  (void)p;
  (void)fl;
  return 1;
}

static int mock_w32_crypt_derive_key(size_t p, unsigned int alg, size_t base,
                                     unsigned long fl, size_t *k) {
  (void)p;
  (void)alg;
  (void)base;
  (void)fl;
  if (g_w32_crypt_fail_step == 6) {
    return 0;
  }
  *k = 3;
  return 1;
}

static int mock_w32_crypt_encrypt(size_t k, size_t h, int fin, unsigned long fl,
                                  unsigned char *d, unsigned long *len,
                                  unsigned long cap) {
  (void)k;
  (void)h;
  (void)fin;
  (void)fl;
  (void)cap;
  if (d && len && *len >= 8) {
    d[0] ^= 0xAA;
  }
  return 1;
}

static int mock_w32_crypt_decrypt(size_t k, size_t h, int fin, unsigned long fl,
                                  unsigned char *d, unsigned long *len) {
  (void)k;
  (void)h;
  (void)fin;
  (void)fl;
  if (d && len && *len >= 8) {
    d[0] ^= 0xAA;
  }
  return 1;
}

static int mock_w32_crypt_destroy_key(size_t k) {
  (void)k;
  return 1;
}

static void install_w32_mocks(void) {
  struct posix_core_win32_hooks h;
  int i;

  posix_core_get_default_win32_hooks(&g_w32_def);
  g_w32_malloc_fail_at = -1;
  g_w32_malloc_calls = 0;
  g_w32_realloc_fail = 0;
  g_w32_use_osfh = 1;
  g_w32_osfh_default = 100;
  g_w32_osfh_map_enabled = 0;
  for (i = 0; i < 160; i++) {
    g_w32_osfh_map[i] = -1;
  }
  g_w32_open_osfh_ret = -2;
  g_w32_dup_ret = -2;
  g_w32_dup_fail_fd = -1;
  g_w32_dup2_ret = -2;
  g_w32_close_ret = 0;
  g_w32_commit_ret = 0;
  g_w32_locking_ret = 0;
  g_w32_pipe_ret = -1;
  g_w32_isatty_ret = 0;
  g_w32_open_rdwr_ret = 5;
  g_w32_chsize_ret = 0;
  g_w32_access_ret = 0;
  g_w32_chmod_ret = 0;
  g_w32_unlink_ret = 0;
  g_w32_rmdir_ret = 0;
  g_w32_getcwd_mode = 0;
  g_w32_chdir_ret = 0;
  g_w32_fullpath_null = 0;
  g_w32_mktemp_ret = 0;
  g_w32_execve_ret = -1;
  g_w32_spawnve_ret = 0;
  g_w32_spawnvpe_ret = 0;
  g_w32_spawnv_ret = 0;
  g_w32_raise_calls = 0;
  g_w32_mod_advapi_null = 0;
  g_w32_mod_k32_null = 0;
  g_w32_mod_ntdll_null = 0;
  g_w32_mod_ucrt_null = 0;
  g_w32_null_proc_name = NULL;
  g_w32_last_error = 0;
  g_w32_get_fsize_ok = 1;
  g_w32_fsize_val = 100;
  g_w32_set_fp_fail_at = -1;
  g_w32_set_fp_calls = 0;
  g_w32_fp_pos = 10;
  g_w32_set_eof_ok = 1;
  g_w32_lock_ok = 1;
  g_w32_unlock_ok = 1;
  g_w32_cfm_ok = 1;
  g_w32_mvof_ok = 1;
  g_w32_create_timer_invoke = 0;
  g_w32_tick_count = 1000;
  g_w32_file_attrs = 0x80UL;
  g_w32_get_cur_dir_ok = 1;
  g_w32_create_dir_ok = 1;
  g_w32_create_file_ok = 1;
  g_w32_read_file_ok = 1;
  g_w32_read_file_bytes = 4;
  g_w32_write_file_ok = 1;
  g_w32_write_file_bytes = 4;
  g_w32_dev_io_ok = 1;
  g_w32_reparse_tag = 0xA000000CUL;
  g_w32_wctmb_mode = 0;
  g_w32_wctmb_calls = 0;
  g_w32_hard_link_ok = 1;
  g_w32_move_file_ok = 1;
  g_w32_copy_file_fail_count = 0;
  g_w32_get_user_ok = 1;
  g_w32_drives_mask = 0;
  g_w32_drive_type = 3U;
  g_w32_page_size = 4096UL;
  g_w32_num_procs = 4UL;
  g_w32_memstat_ok = 1;
  g_w32_mod_filename = "C:\\bin\\busybox.exe";
  g_w32_mod_filename_fail = 0;
  g_w32_get_exit_code_ok = 1;
  g_w32_exit_code_val = 7;
  g_w32_ioctl_fail_count = 0;
  g_w32_wsa_err = 0;
  g_w32_socketpair_ret = 0;
  g_w32_crypt_fail_step = 0;
  g_w32_crypt_hash_calls = 0;
  g_w32_open_token_ok = 1;
  g_w32_token_len = 16;
  g_w32_token_info_ok = 1;
  g_w32_token_sid_null = 0;
  g_w32_valid_sid_ok = 1;
  g_w32_subauth_cnt_null = 0;
  g_w32_subauth_cnt = 2;
  g_w32_subauth_null = 0;
  g_w32_rid_seq[0] = 1000;
  g_w32_rid_seq[1] = 1000;
  g_w32_rid_seq[2] = 1000;
  g_w32_rid_seq[3] = 1000;
  g_w32_rid_seq_idx = 0;
  g_w32_exit_proc_calls = 0;
  g_w32_term_proc_calls = 0;
  g_w32_final_path_mode = 0;
  g_w32_symlink_fail_count = 0;
  g_w32_clone_status = 0;
  g_w32_vprotect_ok = 1;

  h.malloc_fn = mock_w32_malloc;
  h.realloc_fn = mock_w32_realloc;
  h.free_fn = mock_w32_free;
  h.get_osfhandle_fn = mock_w32_get_osfhandle;
  h.open_osfhandle_fn = mock_w32_open_osfhandle;
  h.dup_fn = mock_w32_dup;
  h.dup2_fn = mock_w32_dup2;
  h.close_fn = mock_w32_close;
  h.commit_fn = mock_w32_commit;
  h.locking_fn = mock_w32_locking;
  h.pipe_fn = mock_w32_pipe;
  h.isatty_fn = mock_w32_isatty;
  h.open_rdwr_fn = mock_w32_open_rdwr;
  h.chsize_fn = mock_w32_chsize;
  h.access_fn = mock_w32_access;
  h.chmod_fn = mock_w32_chmod;
  h.unlink_fn = mock_w32_unlink;
  h.rmdir_fn = mock_w32_rmdir;
  h.getcwd_fn = mock_w32_getcwd;
  h.chdir_fn = mock_w32_chdir;
  h.fullpath_fn = mock_w32_fullpath;
  h.mktemp_s_fn = mock_w32_mktemp_s;
  h.execve_fn = mock_w32_execve;
  h.spawnve_fn = mock_w32_spawnve;
  h.spawnvpe_fn = mock_w32_spawnvpe;
  h.spawnv_fn = mock_w32_spawnv;
  h.raise_fn = mock_w32_raise;
  h.get_module_handle_fn = mock_w32_get_module_handle;
  h.get_proc_address_fn = mock_w32_get_proc_address;
  h.get_last_error_fn = mock_w32_get_last_error;
  h.get_file_size_ex_fn = mock_w32_get_file_size_ex;
  h.set_file_pointer_ex_fn = mock_w32_set_file_pointer_ex;
  h.set_end_of_file_fn = mock_w32_set_end_of_file;
  h.lock_file_ex_fn = mock_w32_lock_file_ex;
  h.unlock_file_ex_fn = mock_w32_unlock_file_ex;
  h.create_file_mapping_fn = mock_w32_create_file_mapping;
  h.map_view_of_file_fn = mock_w32_map_view_of_file;
  h.unmap_view_of_file_fn = mock_w32_unmap_view_of_file;
  h.create_timer_fn = mock_w32_create_timer;
  h.delete_timer_fn = mock_w32_delete_timer;
  h.get_tick_count_fn = mock_w32_get_tick_count;
  h.get_file_attributes_fn = mock_w32_get_file_attributes;
  h.get_current_dir_fn = mock_w32_get_current_dir;
  h.set_current_dir_fn = mock_w32_set_current_dir;
  h.create_directory_fn = mock_w32_create_directory;
  h.create_file_fn = mock_w32_create_file;
  h.close_handle_fn = mock_w32_close_handle;
  h.read_file_fn = mock_w32_read_file;
  h.write_file_fn = mock_w32_write_file;
  h.device_io_control_fn = mock_w32_device_io_control;
  h.wide_char_to_multi_byte_fn = mock_w32_wide_char_to_multi_byte;
  h.create_hard_link_fn = mock_w32_create_hard_link;
  h.move_file_ex_fn = mock_w32_move_file_ex;
  h.copy_file_fn = mock_w32_copy_file;
  h.get_user_name_fn = mock_w32_get_user_name;
  h.sleep_ex_fn = mock_w32_sleep_ex;
  h.get_logical_drives_fn = mock_w32_get_logical_drives;
  h.get_drive_type_fn = mock_w32_get_drive_type;
  h.flush_file_buffers_fn = mock_w32_flush_file_buffers;
  h.get_system_info_fn = mock_w32_get_system_info;
  h.global_memory_status_ex_fn = mock_w32_global_memory_status_ex;
  h.get_module_file_name_fn = mock_w32_get_module_file_name;
  h.set_std_handle_fn = mock_w32_set_std_handle;
  h.set_handle_information_fn = mock_w32_set_handle_information;
  h.wait_for_single_object_fn = mock_w32_wait_for_single_object;
  h.get_exit_code_process_fn = mock_w32_get_exit_code_process;
  h.ioctlsocket_fn = mock_w32_ioctlsocket;
  h.wsa_get_last_error_fn = mock_w32_wsa_get_last_error;
  h.socketpair_fn = mock_w32_socketpair;
  h.crypt_acquire_context_fn = mock_w32_crypt_acquire_context;
  h.crypt_create_hash_fn = mock_w32_crypt_create_hash;
  h.crypt_hash_data_fn = mock_w32_crypt_hash_data;
  h.crypt_get_hash_param_fn = mock_w32_crypt_get_hash_param;
  h.crypt_destroy_hash_fn = mock_w32_crypt_destroy_hash;
  h.crypt_release_context_fn = mock_w32_crypt_release_context;
  h.crypt_derive_key_fn = mock_w32_crypt_derive_key;
  h.crypt_encrypt_fn = mock_w32_crypt_encrypt;
  h.crypt_decrypt_fn = mock_w32_crypt_decrypt;
  h.crypt_destroy_key_fn = mock_w32_crypt_destroy_key;
  posix_core_set_win32_hooks(&h);
}

static void reset_w32_mocks(void) {
  posix_core_reset_process_state(0, 0, 0);
  posix_core_set_win32_hooks(NULL);
  reset_common_mocks();
}

static void noop_sig_handler(int s) { (void)s; }

TEST test_core_win32_default_wrappers(void) {
  struct posix_core_win32_hooks def;
  void *p;
  char cwd[512];
  char full[512];
  char modpath[512];
  const char *no_argv[2];
  const char *no_envp[1];
  unsigned long dw = 0;
  unsigned long ioctl_arg = 0;
  TEST_MEM_STAT ms;
  void (*old_sig)(int);
  FILE *f;
  int fd;
  int fd2;
  int fd3;

  no_argv[0] = "nonexistent_awm_exe_xyz";
  no_argv[1] = NULL;
  no_envp[0] = NULL;

  posix_core_get_default_win32_hooks(NULL);
  posix_core_get_default_win32_hooks(&def);
  posix_core_set_win32_hooks(&def);
  posix_core_set_win32_hooks(NULL);

  /* realloc */
  p = def.malloc_fn(16);
  ASSERT(p != NULL);
  p = def.realloc_fn(p, 32);
  ASSERT(p != NULL);
  def.free_fn(p);

  /* Create temp file for CRT fd/file wrappers */
  f = fopen("awm_def_wrap.tmp", "wb");
  ASSERT(f != NULL);
  fwrite("hello", 1, 5, f);
  fclose(f);

  /* open_rdwr failure and success */
  ASSERT_EQ(-1, def.open_rdwr_fn("nonexistent_awm_file_987.tmp"));
  fd = def.open_rdwr_fn("awm_def_wrap.tmp");
  ASSERT(fd >= 0);
  fd2 = def.dup_fn(fd);
  ASSERT(fd2 >= 0);
  fd3 = def.dup_fn(fd);
  ASSERT(fd3 >= 0);
  ASSERT(def.dup2_fn(fd2, fd3) >= 0);
  ASSERT_EQ(0, def.locking_fn(fd, 2 /* _LK_NBLCK */, 4));
  ASSERT_EQ(0, def.locking_fn(fd, 0 /* _LK_UNLCK */, 4));
  ASSERT_EQ(0, def.chsize_fn(fd, 3));
  ASSERT_EQ(0, def.close_fn(fd3));
  ASSERT_EQ(0, def.close_fn(fd2));
  ASSERT_EQ(0, def.close_fn(fd));

  ASSERT_EQ(0, def.chmod_fn("awm_def_wrap.tmp", 0600));
  (void)def.copy_file_fn("awm_def_wrap.tmp", "awm_def_wrap2.tmp", 0);
  ASSERT_EQ(0, def.unlink_fn("awm_def_wrap2.tmp"));
  ASSERT_EQ(0, def.unlink_fn("awm_def_wrap.tmp"));

  /* rmdir, chdir, fullpath, get_current_dir, set_current_dir */
  ASSERT(def.create_directory_fn("awm_def_dir.tmp", NULL) != 0);
  ASSERT_EQ(0, def.rmdir_fn("awm_def_dir.tmp"));
  ASSERT(def.get_current_dir_fn((unsigned long)sizeof(cwd), cwd) > 0);
  ASSERT(def.set_current_dir_fn(cwd) != 0);
  ASSERT_EQ(0, def.chdir_fn(cwd));
  ASSERT(def.fullpath_fn(full, ".", sizeof(full)) != NULL);

  /* execve / spawnve / spawnvpe / spawnv on nonexistent binary */
  ASSERT_EQ((intptr_t)-1,
            def.execve_fn("nonexistent_awm_exe_999.exe", no_argv, no_envp));
  ASSERT_EQ((intptr_t)-1,
            def.spawnve_fn(0, "nonexistent_awm_exe_999.exe", no_argv, no_envp));
  ASSERT_EQ((intptr_t)-1, def.spawnvpe_fn(0, "nonexistent_awm_exe_999.exe",
                                          no_argv, no_envp));
  ASSERT_EQ((intptr_t)-1,
            def.spawnv_fn(0, "nonexistent_awm_exe_999.exe", no_argv));

  /* raise with safe handler */
  old_sig = signal(SIGINT, noop_sig_handler);
  (void)def.raise_fn(SIGINT);
  (void)signal(SIGINT, old_sig);

  /* Win32 API wrappers with safe arguments */
  {
    unsigned char ov_dummy[64];
    memset(ov_dummy, 0, sizeof(ov_dummy));
    (void)def.get_last_error_fn();
    (void)def.lock_file_ex_fn(NULL, 0, 0, 0, 0, ov_dummy);
    (void)def.unlock_file_ex_fn(NULL, 0, 0, 0, ov_dummy);
    (void)def.device_io_control_fn(NULL, 0, NULL, 0, NULL, 0, &dw, NULL);
  }
  ASSERT(def.wide_char_to_multi_byte_fn(0, 0, L"a", 1, NULL, 0, NULL, NULL) >
         0);
  (void)def.sleep_ex_fn(0, 0);
  memset(&ms, 0, sizeof(ms));
  ms.dwLength = (unsigned long)sizeof(ms);
  (void)def.global_memory_status_ex_fn(&ms);
  ASSERT(def.get_module_file_name_fn(NULL, modpath,
                                     (unsigned long)sizeof(modpath)) > 0);
  (void)def.set_std_handle_fn(0, NULL);
  (void)def.set_handle_information_fn(NULL, 0, 0);
  (void)def.wait_for_single_object_fn(NULL, 0);
  (void)def.get_exit_code_process_fn(NULL, &dw);
  (void)def.ioctlsocket_fn((uintptr_t)-1, 0, &ioctl_arg);
  (void)def.wsa_get_last_error_fn();

  PASS();
}

TEST test_core_win32_rid_ugid_and_exit(void) {
  gid_t grps[2];
  install_w32_mocks();

  /* safe_get_osfhandle branches via fdatasync: <0, >=2048, -2 */
  ASSERT_EQ(-1, fdatasync(-1));
  ASSERT_EQ(-1, fdatasync(2048));
  g_w32_osfh_default = -2;
  ASSERT_EQ(-1, fdatasync(3));
  g_w32_osfh_default = 100;

  /* my_exit_hook: k32 NULL, proc NULL, proc valid */
  g_w32_mod_k32_null = 1;
  my_exit_hook(1);
  g_w32_mod_k32_null = 0;
  g_w32_null_proc_name = "ExitProcess";
  my_exit_hook(1);
  g_w32_null_proc_name = NULL;
  my_exit_hook(1);
  ASSERT_EQ(1, g_w32_exit_proc_calls);

  /* auto_win_exit: k32 NULL, proc NULL, proc valid */
  g_w32_mod_k32_null = 1;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED, auto_win_exit(1));
  g_w32_mod_k32_null = 0;
  g_w32_null_proc_name = "TerminateProcess";
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED, auto_win_exit(1));
  g_w32_null_proc_name = NULL;
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, auto_win_exit(1));
  ASSERT_EQ(1, g_w32_term_proc_calls);

  /* get_current_rid error branches */
  g_w32_mod_advapi_null = 1;
  ASSERT_EQ((uid_t)0, getuid());
  ASSERT_EQ((uid_t)0, geteuid());
  ASSERT_EQ((gid_t)0, getgid());
  ASSERT_EQ((gid_t)0, getegid());
  ASSERT_EQ(1, getgroups(1, grps));
  ASSERT_EQ((gid_t)0, grps[0]);
  g_w32_mod_advapi_null = 0;

  g_w32_null_proc_name = "OpenProcessToken";
  (void)getuid();
  g_w32_null_proc_name = "GetTokenInformation";
  (void)getuid();
  g_w32_null_proc_name = "GetSidSubAuthority";
  (void)getuid();
  g_w32_null_proc_name = "GetSidSubAuthorityCount";
  (void)getuid();
  g_w32_null_proc_name = "IsValidSid";
  (void)getuid();
  g_w32_null_proc_name = NULL;

  /* OpenProcessToken false, dwLength == 0, malloc NULL, GetTokenInformation
   * false, pSid NULL, IsValidSid false, pCount NULL, *pCount == 0, pRid NULL,
   * full success */
  g_w32_open_token_ok = 0;
  (void)getuid();
  g_w32_open_token_ok = 1;

  g_w32_token_len = 0;
  (void)getuid();
  g_w32_token_len = 16;

  g_w32_malloc_fail_at = 0;
  g_w32_malloc_calls = 0;
  (void)getuid();
  g_w32_malloc_fail_at = -1;

  g_w32_token_info_ok = 0;
  (void)getuid();
  g_w32_token_info_ok = 1;

  g_w32_token_sid_null = 1;
  (void)getuid();
  g_w32_token_sid_null = 0;

  g_w32_valid_sid_ok = 0;
  (void)getuid();
  g_w32_valid_sid_ok = 1;

  g_w32_subauth_cnt_null = 1;
  (void)getuid();
  g_w32_subauth_cnt_null = 0;

  g_w32_subauth_cnt = 0;
  (void)getuid();
  g_w32_subauth_cnt = 2;

  g_w32_subauth_null = 1;
  (void)getuid();
  g_w32_subauth_null = 0;

  g_w32_rid_seq[0] = 1000;
  g_w32_rid_seq_idx = 0;
  ASSERT_EQ((uid_t)1000, getuid());
  g_w32_rid_seq_idx = 0;
  ASSERT_EQ((uid_t)1000, geteuid());
  g_w32_rid_seq_idx = 0;
  ASSERT_EQ((gid_t)1000, getgid());
  g_w32_rid_seq_idx = 0;
  ASSERT_EQ((gid_t)1000, getegid());

  /* getgroups / setgroups */
  ASSERT_EQ(-1, getgroups(-1, grps));
  ASSERT_EQ(1, getgroups(0, NULL));
  ASSERT_EQ(1, getgroups(1, NULL));
  g_w32_rid_seq_idx = 0;
  ASSERT_EQ(1, getgroups(1, grps));
  ASSERT_EQ((gid_t)1000, grps[0]);

  ASSERT_EQ(-1, setgroups(1, NULL));
  ASSERT_EQ(0, setgroups(0, NULL));
  ASSERT_EQ(0, setgroups(1, grps));

  /* uid/gid/pgid/sid setters & getters */
  ASSERT_EQ(-1, getpgid(-1));
  ASSERT_EQ(0, getpgid(0));
  ASSERT_EQ(0, getpgrp());
  ASSERT_EQ(0, getppid());
  ASSERT_EQ(-1, getsid(-1));
  ASSERT_EQ(0, getsid(0));
  ASSERT_EQ(0L, gethostid());

  ASSERT_EQ(-1, setegid((gid_t)-1));
  ASSERT_EQ(0, setegid(10));
  ASSERT_EQ(-1, seteuid((uid_t)-1));
  ASSERT_EQ(0, seteuid(10));
  ASSERT_EQ(-1, setgid((gid_t)-1));
  ASSERT_EQ(0, setgid(10));
  ASSERT_EQ(-1, setuid((uid_t)-1));
  ASSERT_EQ(0, setuid(10));
  ASSERT_EQ(-1, setpgid(-1, 0));
  ASSERT_EQ(-1, setpgid(0, -1));
  ASSERT_EQ(0, setpgid(0, 0));
  ASSERT_EQ(0, setpgrp());
  ASSERT_EQ(0, setsid());
  ASSERT_EQ(-1, setregid((gid_t)-1, (gid_t)-1));
  ASSERT_EQ(0, setregid((gid_t)-1, 10));
  ASSERT_EQ(0, setregid(10, (gid_t)-1));
  ASSERT_EQ(0, setregid(10, 10));
  ASSERT_EQ(-1, setreuid((uid_t)-1, (uid_t)-1));
  ASSERT_EQ(0, setreuid((uid_t)-1, 10));
  ASSERT_EQ(0, setreuid(10, (uid_t)-1));
  ASSERT_EQ(0, setreuid(10, 10));

  reset_w32_mocks();
  PASS();
}

TEST test_core_win32_fcntl_and_lockf(void) {
  struct flock fl;
  int i;
  int fd_real;
  int dup_res;
  FILE *f;

  install_w32_mocks();

  /* Create real temp file for F_DUPFD / F_DUPFD_CLOEXEC (calls posix_dup/dup2)
   */
  f = fopen("awm_fcntl_tmp.tmp", "wb");
  ASSERT(f != NULL);
  fclose(f);
  fd_real = posix_open("awm_fcntl_tmp.tmp", O_RDWR);
  ASSERT(fd_real >= 0);

  /* F_DUPFD / F_DUPFD_CLOEXEC invalid min_fd */
  ASSERT_EQ(-1, fcntl(fd_real, F_DUPFD, -1));
  ASSERT_EQ(-1, fcntl(fd_real, F_DUPFD, 2048));
  /* min_fd == 0 */
  dup_res = fcntl(fd_real, F_DUPFD, 0);
  ASSERT(dup_res >= 0);
  posix_close(dup_res);

  /* min_fd > 0: first slot occupied, next slot free -> succeeds */
  g_w32_osfh_map_enabled = 1;
  for (i = 0; i < 160; i++) {
    g_w32_osfh_map[i] = 100;
  }
  g_w32_osfh_map[42] = -1;
  dup_res = fcntl(fd_real, F_DUPFD_CLOEXEC, 40);
  ASSERT_EQ(42, dup_res);
  posix_close(dup_res);

  /* min_fd > 0: dup2 fails on free slot (fd = -1) -> EMFILE */
  ASSERT_EQ(-1, fcntl(-1, F_DUPFD, 40));
  ASSERT_EQ(EMFILE, errno);

  /* min_fd > 0: all slots occupied -> EMFILE */
  g_w32_osfh_map[42] = 100;
  ASSERT_EQ(-1, fcntl(fd_real, F_DUPFD, 40));
  ASSERT_EQ(EMFILE, errno);
  g_w32_osfh_map_enabled = 0;

  posix_close(fd_real);
  remove("awm_fcntl_tmp.tmp");

  /* F_GETFD / F_SETFD */
  ASSERT_EQ(0, fcntl(3, F_GETFD));
  ASSERT_EQ(0, fcntl(3, F_SETFD, 1));

  /* F_GETFL: dup_socket, crt_socket, raw fd, fallback get_nonblock(fd) */
  posix_set_dup_socket(15, 500);
  set_nonblock(500, 0);
  set_nonblock(15, 1);
  ASSERT(fcntl(15, F_GETFL) != 0);
  set_nonblock(500, 1);
  ASSERT(fcntl(15, F_GETFL) != 0);
  set_nonblock(500, 0);
  set_nonblock(15, 0);
  posix_set_dup_socket(15, 0);

  posix_register_crt_socket(16, 501);
  set_nonblock(501, 0);
  set_nonblock(16, 0);
  ASSERT_EQ(0, fcntl(16, F_GETFL));
  posix_register_crt_socket(16, 0);

  set_nonblock(17, 0);
  ASSERT_EQ(0, fcntl(17, F_GETFL));

  /* F_SETFL: O_NONBLOCK, 0x0004, 0; dup_sock, crt_sock, raw fd; ioctl retry
   * ok, WSAENOTSOCK, other WSA error */
  posix_set_dup_socket(15, 500);
  g_w32_ioctl_fail_count = 0;
  ASSERT_EQ(0, fcntl(15, F_SETFL, O_NONBLOCK));
  ASSERT_EQ(0, fcntl(15, F_SETFL, 0x0004));
  ASSERT_EQ(0, fcntl(15, F_SETFL, 0));

  /* First ioctl on s fails, second on fd succeeds */
  g_w32_ioctl_fail_count = 1;
  ASSERT_EQ(0, fcntl(15, F_SETFL, O_NONBLOCK));

  /* Both ioctls fail: WSAENOTSOCK -> 0, other -> -1 */
  g_w32_ioctl_fail_count = 2;
  g_w32_wsa_err = 10038;
  ASSERT_EQ(0, fcntl(15, F_SETFL, 0));

  g_w32_ioctl_fail_count = 2;
  g_w32_wsa_err = 10054;
  ASSERT_EQ(-1, fcntl(15, F_SETFL, 0));
  posix_set_dup_socket(15, 0);

  posix_register_crt_socket(16, 501);
  g_w32_ioctl_fail_count = 0;
  ASSERT_EQ(0, fcntl(16, F_SETFL, 0));
  posix_register_crt_socket(16, 0);

  /* Raw fd ioctl fails (s == fd) */
  g_w32_ioctl_fail_count = 1;
  g_w32_wsa_err = 10038;
  ASSERT_EQ(0, fcntl(17, F_SETFL, 0));

  /* F_SETLK / F_SETLKW / F_GETLK */
  ASSERT_EQ(-1, fcntl(3, F_SETLK, (struct flock *)NULL));
  g_w32_osfh_default = -1;
  memset(&fl, 0, sizeof(fl));
  ASSERT_EQ(-1, fcntl(3, F_SETLK, &fl));
  g_w32_osfh_default = 100;

  /* SEEK_CUR fail & ok, SEEK_END fail & ok, invalid whence, start < 0 */
  fl.l_whence = SEEK_CUR;
  fl.l_start = 0;
  fl.l_len = 10;
  fl.l_type = F_RDLCK;
  g_w32_set_fp_fail_at = 0;
  g_w32_set_fp_calls = 0;
  ASSERT_EQ(-1, fcntl(3, F_SETLK, &fl));
  g_w32_set_fp_fail_at = -1;
  ASSERT_EQ(0, fcntl(3, F_SETLK, &fl));

  fl.l_whence = SEEK_END;
  g_w32_get_fsize_ok = 0;
  ASSERT_EQ(-1, fcntl(3, F_SETLK, &fl));
  g_w32_get_fsize_ok = 1;
  ASSERT_EQ(0, fcntl(3, F_SETLK, &fl));

  fl.l_whence = 99;
  ASSERT_EQ(-1, fcntl(3, F_SETLK, &fl));

  fl.l_whence = SEEK_SET;
  fl.l_start = -5;
  ASSERT_EQ(-1, fcntl(3, F_SETLK, &fl));

  /* F_GETLK: len == 0, F_WRLCK lock ok, F_RDLCK lock fail, F_UNLCK lock fail */
  fl.l_start = 0;
  fl.l_len = 0;
  fl.l_type = F_WRLCK;
  g_w32_lock_ok = 1;
  ASSERT_EQ(0, fcntl(3, F_GETLK, &fl));
  ASSERT_EQ(F_UNLCK, fl.l_type);

  g_w32_lock_ok = 0;
  fl.l_type = F_UNLCK;
  ASSERT_EQ(0, fcntl(3, F_GETLK, &fl));
  ASSERT_EQ(F_WRLCK, fl.l_type);

  fl.l_type = F_RDLCK;
  ASSERT_EQ(0, fcntl(3, F_GETLK, &fl));
  ASSERT_EQ(F_RDLCK, fl.l_type);

  /* F_SETLK / F_SETLKW: F_UNLCK, F_WRLCK fail err 33, 997, other, success */
  fl.l_type = F_UNLCK;
  ASSERT_EQ(0, fcntl(3, F_SETLK, &fl));

  fl.l_type = F_WRLCK;
  g_w32_lock_ok = 0;
  g_w32_last_error = 33;
  ASSERT_EQ(-1, fcntl(3, F_SETLK, &fl));
  ASSERT_EQ(EAGAIN, errno);
  g_w32_last_error = 997;
  ASSERT_EQ(-1, fcntl(3, F_SETLKW, &fl));
  ASSERT_EQ(EAGAIN, errno);
  g_w32_last_error = 5;
  ASSERT_EQ(-1, fcntl(3, F_SETLKW, &fl));
  ASSERT_EQ(EINVAL, errno);

  g_w32_lock_ok = 1;
  ASSERT_EQ(0, fcntl(3, F_SETLKW, &fl));

  /* Unknown fcntl cmd */
  ASSERT_EQ(-1, fcntl(3, 9999));
  ASSERT_EQ(ENOSYS, errno);

  /* lockf */
  ASSERT_EQ(-1, lockf(3, 1, -1));
  g_w32_osfh_default = -1;
  ASSERT_EQ(-1, lockf(3, 1, 10));
  g_w32_osfh_default = 100;
  g_w32_locking_ret = 0;
  ASSERT_EQ(0, lockf(3, 0, 0));
  ASSERT_EQ(0, lockf(3, 1, 10));
  ASSERT_EQ(0, lockf(3, 2, 10));
  ASSERT_EQ(0, lockf(3, 3, 10));
  g_w32_locking_ret = -1;
  ASSERT_EQ(-1, lockf(3, 3, 10));
  ASSERT_EQ(-1, lockf(3, 99, 10));

  reset_w32_mocks();
  PASS();
}

TEST test_core_win32_at_paths_and_fs(void) {
  char buf[32];
  char tmpl[64];
  int fd;

  install_w32_mocks();

  /* posix_resolve_at_path via unlinkat / renameat / openat / faccessat */
  ASSERT_EQ(-1, unlinkat(AT_FDCWD, NULL, 0));
  g_w32_malloc_fail_at = 0;
  g_w32_malloc_calls = 0;
  ASSERT_EQ(-1, unlinkat(AT_FDCWD, "a", 0));
  g_w32_malloc_fail_at = -1;

  /* Absolute paths: /, \, C:, empty string */
  ASSERT_EQ(0, unlinkat(AT_FDCWD, "/abs", 0));
  ASSERT_EQ(0, unlinkat(AT_FDCWD, "\\abs", AT_REMOVEDIR));
  ASSERT_EQ(0, unlinkat(AT_FDCWD, "C:abs", 0));
  ASSERT_EQ(0, unlinkat(AT_FDCWD, "", 0));

  /* AT_FDCWD: getcwd NULL, empty, overflow, trailing \, trailing /, normal */
  g_w32_getcwd_mode = 1;
  ASSERT_EQ(-1, unlinkat(AT_FDCWD, "rel", 0));
  g_w32_getcwd_mode = 2;
  ASSERT_EQ(-1, unlinkat(AT_FDCWD, "rel", 0));
  g_w32_getcwd_mode = 4;
  ASSERT_EQ(-1, unlinkat(AT_FDCWD, "rel", 0));
  g_w32_getcwd_mode = 3;
  ASSERT_EQ(0, unlinkat(AT_FDCWD, "rel", 0));
  g_w32_getcwd_mode = 5;
  ASSERT_EQ(0, unlinkat(AT_FDCWD, "rel", 0));
  g_w32_getcwd_mode = 0;
  ASSERT_EQ(0, unlinkat(AT_FDCWD, "rel", 0));

  /* dirfd != AT_FDCWD */
  g_w32_osfh_default = -1;
  ASSERT_EQ(-1, unlinkat(5, "rel", 0));
  g_w32_osfh_default = 100;

  g_w32_mod_k32_null = 1;
  ASSERT_EQ(-1, unlinkat(5, "rel", 0));
  g_w32_mod_k32_null = 0;

  g_w32_null_proc_name = "GetFinalPathNameByHandleA";
  ASSERT_EQ(-1, unlinkat(5, "rel", 0));
  g_w32_null_proc_name = NULL;

  g_w32_final_path_mode = 2;
  ASSERT_EQ(-1, unlinkat(5, "rel", 0));
  g_w32_final_path_mode = 3;
  ASSERT_EQ(-1, unlinkat(5, "rel", 0));
  g_w32_final_path_mode = 4;
  ASSERT_EQ(-1, unlinkat(5, "rel", 0));
  g_w32_final_path_mode = 7;
  ASSERT_EQ(-1, unlinkat(5, "rel", 0));
  g_w32_final_path_mode = 0;
  ASSERT_EQ(0, unlinkat(5, "rel", 0));
  g_w32_final_path_mode = 1;
  ASSERT_EQ(0, unlinkat(5, "rel", 0));
  g_w32_final_path_mode = 5;
  ASSERT_EQ(0, unlinkat(5, "rel", 0));
  g_w32_final_path_mode = 6;
  ASSERT_EQ(0, unlinkat(5, "rel", 0));
  g_w32_final_path_mode = 8;
  ASSERT_EQ(0, unlinkat(5, "rel", 0));
  g_w32_final_path_mode = 9;
  ASSERT_EQ(0, unlinkat(5, "rel", 0));
  g_w32_final_path_mode = 10;
  ASSERT_EQ(0, unlinkat(5, "rel", 0));
  g_w32_final_path_mode = 11;
  ASSERT_EQ(0, unlinkat(5, "rel", 0));
  g_w32_final_path_mode = 0;

  /* renameat: old fail, new fail, success */
  ASSERT_EQ(-1, renameat(AT_FDCWD, NULL, AT_FDCWD, "b"));
  ASSERT_EQ(-1, renameat(AT_FDCWD, "a", AT_FDCWD, NULL));
  ASSERT_EQ(0, renameat(AT_FDCWD, "a", AT_FDCWD, "b"));

  /* openat: resolve fail, O_CREAT with mode 0 and nonzero, without O_CREAT */
  ASSERT_EQ(-1, openat(AT_FDCWD, NULL, O_RDONLY));
  fd = openat(AT_FDCWD, "awm_openat_cov.tmp", O_CREAT | O_RDWR, 0);
  if (fd >= 0) {
    posix_close(fd);
  }
  fd = openat(AT_FDCWD, "awm_openat_cov.tmp", 0x0200 | O_RDWR, 0600);
  if (fd >= 0) {
    posix_close(fd);
  }
  fd = openat(AT_FDCWD, "awm_openat_cov.tmp", O_RDONLY);
  if (fd >= 0) {
    posix_close(fd);
  }
  remove("awm_openat_cov.tmp");

  /* faccessat: bad flags, valid flags, resolve fail, success */
  ASSERT_EQ(-1, faccessat(AT_FDCWD, "a", 0, 1));
  ASSERT_EQ(-1, faccessat(AT_FDCWD, NULL, 0, 0x100));
  ASSERT_EQ(0, faccessat(AT_FDCWD, "a", 0, 0x200));
  ASSERT_EQ(0, faccessat(AT_FDCWD, "a", 0, 0));

  /* chown */
  ASSERT_EQ(-1, chown(NULL, 0, 0));
  g_w32_file_attrs = 0xFFFFFFFFUL;
  ASSERT_EQ(-1, chown("a", 0, 0));
  g_w32_file_attrs = 0x80UL;

  g_w32_rid_seq[0] = 1000;
  g_w32_rid_seq[1] = 1000;
  g_w32_rid_seq_idx = 0;
  ASSERT_EQ(-1, chown("a", 999, (gid_t)-1));

  /* owner matches, group != getgid() -> malloc fail -> EPERM */
  g_w32_rid_seq_idx = 0;
  g_w32_malloc_fail_at = 2;
  g_w32_malloc_calls = 0;
  ASSERT_EQ(-1, chown("a", 1000, 2000));
  g_w32_malloc_fail_at = -1;

  /* group != getgid() and not found in getgroups -> EPERM */
  g_w32_rid_seq[0] = 1000;
  g_w32_rid_seq[1] = 1000;
  g_w32_rid_seq[2] = 1000;
  g_w32_rid_seq_idx = 0;
  ASSERT_EQ(-1, chown("a", (uid_t)-1, 2000));

  /* group != getgid() first call (1000), then getgroups returns 2000 -> found
   */
  g_w32_rid_seq[0] = 1000;
  g_w32_rid_seq[1] = 2000;
  g_w32_rid_seq_idx = 0;
  ASSERT_EQ(0, chown("a", (uid_t)-1, 2000));

  /* group == getgid() -> 0 */
  g_w32_rid_seq[0] = 1000;
  g_w32_rid_seq[1] = 1000;
  g_w32_rid_seq_idx = 0;
  ASSERT_EQ(0, chown("a", 1000, 1000));

  /* fchown */
  ASSERT_EQ(-1, fchown(-1, (uid_t)-1, (gid_t)-1));
  g_w32_osfh_default = -1;
  ASSERT_EQ(-1, fchown(3, (uid_t)-1, (gid_t)-1));
  g_w32_osfh_default = 100;
  g_w32_mod_k32_null = 1;
  ASSERT_EQ(-1, fchown(3, (uid_t)-1, (gid_t)-1));
  g_w32_mod_k32_null = 0;
  g_w32_null_proc_name = "GetFinalPathNameByHandleA";
  ASSERT_EQ(-1, fchown(3, (uid_t)-1, (gid_t)-1));
  g_w32_null_proc_name = NULL;
  g_w32_malloc_fail_at = 0;
  g_w32_malloc_calls = 0;
  ASSERT_EQ(-1, fchown(3, (uid_t)-1, (gid_t)-1));
  g_w32_malloc_fail_at = -1;
  g_w32_final_path_mode = 2;
  ASSERT_EQ(-1, fchown(3, (uid_t)-1, (gid_t)-1));
  g_w32_final_path_mode = 3;
  ASSERT_EQ(-1, fchown(3, (uid_t)-1, (gid_t)-1));
  g_w32_final_path_mode = 0;
  ASSERT_EQ(0, fchown(3, (uid_t)-1, (gid_t)-1));
  g_w32_final_path_mode = 1;
  ASSERT_EQ(0, fchown(3, (uid_t)-1, (gid_t)-1));
  g_w32_final_path_mode = 8;
  ASSERT_EQ(0, fchown(3, (uid_t)-1, (gid_t)-1));
  g_w32_final_path_mode = 9;
  ASSERT_EQ(0, fchown(3, (uid_t)-1, (gid_t)-1));
  g_w32_final_path_mode = 10;
  ASSERT_EQ(0, fchown(3, (uid_t)-1, (gid_t)-1));
  g_w32_final_path_mode = 11;
  ASSERT_EQ(0, fchown(3, (uid_t)-1, (gid_t)-1));
  g_w32_final_path_mode = 0;

  /* fchownat */
  ASSERT_EQ(-1, fchownat(AT_FDCWD, NULL, (uid_t)-1, (gid_t)-1, 0));
  ASSERT_EQ(0, fchownat(AT_FDCWD, "a", (uid_t)-1, (gid_t)-1, 0));

  /* lchown */
  ASSERT_EQ(-1, lchown(NULL, 1, 1));
  ASSERT_EQ(-1, lchown("a", (uid_t)-1, 1));
  ASSERT_EQ(-1, lchown("a", 1, (gid_t)-1));
  g_w32_file_attrs = 0xFFFFFFFFUL;
  ASSERT_EQ(-1, lchown("a", 1, 1));
  g_w32_file_attrs = 0x80UL;
  ASSERT_EQ(0, lchown("a", 1, 1));

  /* link & linkat */
  ASSERT_EQ(-1, link(NULL, "b"));
  ASSERT_EQ(-1, link("a", NULL));
  g_w32_hard_link_ok = 0;
  ASSERT_EQ(-1, link("a", "b"));
  g_w32_hard_link_ok = 1;
  ASSERT_EQ(0, link("a", "b"));

  ASSERT_EQ(-1, linkat(AT_FDCWD, NULL, AT_FDCWD, "b", 0));
  ASSERT_EQ(-1, linkat(AT_FDCWD, "a", AT_FDCWD, NULL, 0));
  g_w32_hard_link_ok = 0;
  ASSERT_EQ(-1, linkat(AT_FDCWD, "a", AT_FDCWD, "b", 0));
  g_w32_hard_link_ok = 1;
  ASSERT_EQ(0, linkat(AT_FDCWD, "a", AT_FDCWD, "b", 0));

  /* symlink & symlinkat */
  ASSERT_EQ(-1, symlink(NULL, "l"));
  ASSERT_EQ(-1, symlink("t", NULL));
  g_w32_mod_k32_null = 1;
  ASSERT_EQ(-1, symlink("t", "l"));
  g_w32_mod_k32_null = 0;
  g_w32_null_proc_name = "CreateSymbolicLinkA";
  ASSERT_EQ(-1, symlink("t", "l"));
  g_w32_null_proc_name = NULL;

  g_w32_file_attrs = 0xFFFFFFFFUL;
  ASSERT_EQ(0, symlink("t", "l"));
  g_w32_file_attrs = 0x10UL;
  ASSERT_EQ(0, symlink("t", "l"));
  g_w32_file_attrs = 0x80UL;

  g_w32_symlink_fail_count = 1;
  g_w32_last_error = 87UL;
  ASSERT_EQ(0, symlink("t", "l"));
  g_w32_symlink_fail_count = 2;
  g_w32_last_error = 87UL;
  ASSERT_EQ(-1, symlink("t", "l"));
  g_w32_symlink_fail_count = 1;
  g_w32_last_error = 5UL;
  ASSERT_EQ(-1, symlink("t", "l"));
  g_w32_symlink_fail_count = 0;

  ASSERT_EQ(-1, symlinkat("t", AT_FDCWD, NULL));
  ASSERT_EQ(0, symlinkat("t", AT_FDCWD, "l"));

  /* readlink & readlinkat */
  ASSERT_EQ((ssize_t)-1, readlink(NULL, buf, sizeof(buf)));
  ASSERT_EQ((ssize_t)-1, readlink("l", NULL, sizeof(buf)));
  ASSERT_EQ((ssize_t)-1, readlink("l", buf, 0));

  g_w32_file_attrs = 0xFFFFFFFFUL;
  ASSERT_EQ((ssize_t)-1, readlink("l", buf, sizeof(buf)));
  g_w32_file_attrs = 0x80UL;
  ASSERT_EQ((ssize_t)-1, readlink("l", buf, sizeof(buf)));
  g_w32_file_attrs = 0x400UL;

  g_w32_create_file_ok = 0;
  ASSERT_EQ((ssize_t)-1, readlink("l", buf, sizeof(buf)));
  g_w32_create_file_ok = 1;

  g_w32_dev_io_ok = 0;
  ASSERT_EQ((ssize_t)-1, readlink("l", buf, sizeof(buf)));
  g_w32_dev_io_ok = 1;

  g_w32_reparse_tag = 0x12345678UL;
  ASSERT_EQ((ssize_t)-1, readlink("l", buf, sizeof(buf)));

  g_w32_reparse_tag = 0xA0000003UL;
  g_w32_wctmb_mode = 0;
  g_w32_wctmb_calls = 0;
  ASSERT_EQ((ssize_t)4, readlink("l", buf, sizeof(buf)));

  g_w32_reparse_tag = 0xA000000CUL;
  g_w32_wctmb_mode = 1;
  g_w32_wctmb_calls = 0;
  ASSERT_EQ((ssize_t)-1, readlink("l", buf, sizeof(buf)));

  g_w32_wctmb_mode = 0;
  g_w32_wctmb_calls = 0;
  g_w32_malloc_fail_at = 0;
  g_w32_malloc_calls = 0;
  ASSERT_EQ((ssize_t)-1, readlink("l", buf, 2));
  g_w32_malloc_fail_at = -1;

  g_w32_wctmb_mode = 2;
  g_w32_wctmb_calls = 0;
  ASSERT_EQ((ssize_t)-1, readlink("l", buf, 2));

  g_w32_wctmb_mode = 0;
  g_w32_wctmb_calls = 0;
  ASSERT_EQ((ssize_t)2, readlink("l", buf, 2));

  ASSERT_EQ((ssize_t)-1, readlinkat(AT_FDCWD, NULL, buf, sizeof(buf)));
  g_w32_wctmb_calls = 0;
  ASSERT_EQ((ssize_t)6, readlinkat(AT_FDCWD, "l", buf, sizeof(buf)));

  /* truncate */
  ASSERT_EQ(-1, truncate(NULL, 0));
  ASSERT_EQ(-1, truncate("a", -1));
  g_w32_open_rdwr_ret = -1;
  ASSERT_EQ(-1, truncate("a", 10));
  g_w32_open_rdwr_ret = 5;
  ASSERT_EQ(0, truncate("a", 10));

  /* posix_rename */
  ASSERT_EQ(-1, posix_rename(NULL, "b"));
  ASSERT_EQ(-1, posix_rename("a", NULL));
  g_w32_move_file_ok = 1;
  ASSERT_EQ(0, posix_rename("a", "b"));
  g_w32_move_file_ok = 0;
  g_w32_copy_file_fail_count = 0;
  ASSERT_EQ(0, posix_rename("a", "b"));
  g_w32_copy_file_fail_count = 1;
  ASSERT_EQ(0, posix_rename("a", "b"));
  g_w32_copy_file_fail_count = 2;
  ASSERT_EQ(-1, posix_rename("a", "b"));
  g_w32_move_file_ok = 1;
  g_w32_copy_file_fail_count = 0;

  /* posix_mkostemp & posix_mkstemp */
  ASSERT_EQ(-1, posix_mkostemp(NULL, 0));
  test_copy_str(tmpl, sizeof(tmpl), "abc");
  ASSERT_EQ(-1, posix_mkostemp(tmpl, 0));
  test_copy_str(tmpl, sizeof(tmpl), "abcdef123456");
  ASSERT_EQ(-1, posix_mkostemp(tmpl, 0));

  g_w32_mktemp_ret = -1;
  test_copy_str(tmpl, sizeof(tmpl), "/tmp/aXXXXXX");
  ASSERT_EQ(-1, posix_mkostemp(tmpl, 0));
  g_w32_mktemp_ret = 0;

  g_w32_create_file_ok = 0;
  test_copy_str(tmpl, sizeof(tmpl), "\\tmp\\aXXXXXX");
  ASSERT_EQ(-1, posix_mkostemp(tmpl, 0));
  g_w32_create_file_ok = 1;

  g_w32_open_osfh_ret = -1;
  test_copy_str(tmpl, sizeof(tmpl), "aXXXXXX");
  ASSERT_EQ(-1, posix_mkostemp(tmpl, 0));
  g_w32_open_osfh_ret = 7;

  test_copy_str(tmpl, sizeof(tmpl), "aXXXXXX");
  ASSERT_EQ(7, posix_mkstemp(tmpl));
  test_copy_str(tmpl, sizeof(tmpl), "aXXXXXX");
  ASSERT_EQ(
      7, posix_mkostemp(tmpl, _O_BINARY | _O_TEXT | _O_APPEND | _O_NOINHERIT));

  /* posix_mkdtemp */
  ASSERT(posix_mkdtemp(NULL) == NULL);
  g_w32_mktemp_ret = -1;
  test_copy_str(tmpl, sizeof(tmpl), "/tmp/dXXXXXX");
  ASSERT(posix_mkdtemp(tmpl) == NULL);
  g_w32_mktemp_ret = 0;
  g_w32_create_dir_ok = 0;
  test_copy_str(tmpl, sizeof(tmpl), "\\tmp\\dXXXXXX");
  ASSERT(posix_mkdtemp(tmpl) == NULL);
  g_w32_create_dir_ok = 1;
  test_copy_str(tmpl, sizeof(tmpl), "dXXXXXX");
  ASSERT(posix_mkdtemp(tmpl) == tmpl);

  reset_w32_mocks();
  PASS();
}

TEST test_core_win32_io_advice_crypto_sys(void) {
  char buf[64];
  char block[64];
  int pfd[2];
  int i;

  install_w32_mocks();

  /* posix_fadvise */
  g_w32_osfh_default = -1;
  ASSERT_EQ(EBADF, posix_fadvise(3, 0, 10, 0));
  g_w32_osfh_default = 100;
  ASSERT_EQ(EINVAL, posix_fadvise(3, -1, 10, 0));
  ASSERT_EQ(EINVAL, posix_fadvise(3, 0, -1, 0));
  ASSERT_EQ(EINVAL, posix_fadvise(3, 0, 10, -1));
  ASSERT_EQ(0, posix_fadvise(3, 0, 10, 0));

  g_w32_mod_k32_null = 1;
  ASSERT_EQ(0, posix_fadvise(3, 0, 10, 1));
  g_w32_mod_k32_null = 0;
  g_w32_null_proc_name = "PrefetchVirtualMemory";
  ASSERT_EQ(0, posix_fadvise(3, 0, 10, 1));
  g_w32_null_proc_name = NULL;
  g_w32_get_fsize_ok = 0;
  ASSERT_EQ(0, posix_fadvise(3, 0, 10, 1));
  g_w32_get_fsize_ok = 1;
  g_w32_fsize_val = 10;
  ASSERT_EQ(0, posix_fadvise(3, 10, 5, 1));
  g_w32_cfm_ok = 0;
  ASSERT_EQ(0, posix_fadvise(3, 0, 0, 1));
  g_w32_cfm_ok = 1;
  g_w32_mvof_ok = 0;
  ASSERT_EQ(0, posix_fadvise(3, 0, 20, 1));
  g_w32_mvof_ok = 1;
  ASSERT_EQ(0, posix_fadvise(3, 2, 4, 1));

  /* posix_fallocate */
  ASSERT_EQ(EBADF, posix_fallocate(-1, 0, 10));
  ASSERT_EQ(EINVAL, posix_fallocate(3, -1, 10));
  ASSERT_EQ(EINVAL, posix_fallocate(3, 0, 0));
  g_w32_osfh_default = -1;
  ASSERT_EQ(EBADF, posix_fallocate(3, 0, 10));
  g_w32_osfh_default = 100;
  g_w32_get_fsize_ok = 0;
  ASSERT_EQ(EINVAL, posix_fallocate(3, 0, 10));
  g_w32_get_fsize_ok = 1;
  g_w32_fsize_val = 100;
  ASSERT_EQ(0, posix_fallocate(3, 0, 50));

  g_w32_set_fp_fail_at = 0;
  g_w32_set_fp_calls = 0;
  ASSERT_EQ(EINVAL, posix_fallocate(3, 0, 200));
  g_w32_set_fp_fail_at = 1;
  g_w32_set_fp_calls = 0;
  ASSERT_EQ(EINVAL, posix_fallocate(3, 0, 200));
  g_w32_set_fp_fail_at = -1;
  g_w32_set_eof_ok = 0;
  ASSERT_EQ(ENOSPC, posix_fallocate(3, 0, 200));
  g_w32_set_eof_ok = 1;
  g_w32_set_fp_fail_at = 2;
  g_w32_set_fp_calls = 0;
  ASSERT_EQ(EINVAL, posix_fallocate(3, 0, 200));
  g_w32_set_fp_fail_at = -1;
  ASSERT_EQ(0, posix_fallocate(3, 0, 200));

  /* sync_file_range & fdatasync */
  ASSERT_EQ(-1, sync_file_range(3, -1, 10, 0));
  ASSERT_EQ(-1, sync_file_range(3, 0, -1, 0));
  g_w32_osfh_default = -1;
  ASSERT_EQ(-1, sync_file_range(3, 0, 10, 0));
  ASSERT_EQ(-1, fdatasync(3));
  g_w32_osfh_default = 100;
  g_w32_commit_ret = -1;
  ASSERT_EQ(-1, sync_file_range(3, 0, 10, 0));
  ASSERT_EQ(-1, fdatasync(3));
  g_w32_commit_ret = 0;
  ASSERT_EQ(0, sync_file_range(3, 0, 10, 0));
  ASSERT_EQ(0, fdatasync(3));

  /* alarm & alarm_timer_callback */
  posix_core_reset_process_state(0, 0, 0);
  g_w32_create_timer_invoke = 1;
  g_w32_tick_count = 1000;
  ASSERT_EQ(0U, alarm(5));
  ASSERT_EQ(1, g_w32_raise_calls);
  g_w32_create_timer_invoke = 0;
  g_w32_tick_count = 5500;
  ASSERT_EQ(1U, alarm(5));
  g_w32_tick_count = 7500;
  ASSERT_EQ(3U, alarm(5));
  g_w32_tick_count = 20000;
  ASSERT_EQ(0U, alarm(0));

  /* ualarm & ualarm_timer_callback */
  g_w32_create_timer_invoke = 1;
  g_w32_tick_count = 1000;
  ASSERT_EQ((useconds_t)0, ualarm(5000, 1000));
  ASSERT_EQ(2, g_w32_raise_calls);
  g_w32_create_timer_invoke = 0;
  g_w32_tick_count = 1002;
  ASSERT_EQ((useconds_t)3000, ualarm(5000, 0));
  g_w32_tick_count = 9000;
  ASSERT_EQ((useconds_t)0, ualarm(0, 0));

  /* pause */
  ASSERT_EQ(-1, pause());
  ASSERT_EQ(EINTR, errno);

  /* confstr */
  ASSERT_EQ((size_t)0, confstr(9999, buf, sizeof(buf)));
  ASSERT(confstr(_CS_PATH, NULL, 10) > 0);
  ASSERT(confstr(_CS_PATH, buf, 0) > 0);
  ASSERT(confstr(_CS_PATH, buf, sizeof(buf)) > 0);

  /* crypt */
  ASSERT(crypt(NULL, "ab") == NULL);
  ASSERT(crypt("pw", NULL) == NULL);
  for (i = 1; i <= 5; i++) {
    g_w32_crypt_fail_step = i;
    g_w32_crypt_hash_calls = 0;
    ASSERT(crypt("pw", "ab") == NULL);
  }
  g_w32_crypt_fail_step = 0;
  g_w32_crypt_hash_calls = 0;
  ASSERT(crypt("pw", "ab") != NULL);
  ASSERT(crypt("pw", "1234567890") != NULL);

  /* encrypt */
  encrypt(NULL, 0);
  for (i = 1; i <= 3; i++) {
    g_w32_crypt_fail_step = i;
    g_w32_crypt_hash_calls = 0;
    encrypt(block, 0);
  }
  g_w32_crypt_fail_step = 6;
  g_w32_crypt_hash_calls = 0;
  encrypt(block, 0);
  g_w32_crypt_fail_step = 0;
  memset(block, 0, sizeof(block));
  block[0] = 1;
  block[7] = 1;
  encrypt(block, 0);
  encrypt(block, 1);

  /* fpathconf / pathconf */
  ASSERT_EQ(-1L, fpathconf(-1, _PC_NAME_MAX));
  ASSERT_EQ(255L, fpathconf(3, _PC_NAME_MAX));
  ASSERT_EQ(260L, fpathconf(3, _PC_PATH_MAX));
  ASSERT_EQ(4096L, fpathconf(3, _PC_PIPE_BUF));
  ASSERT_EQ(-1L, fpathconf(3, 9999));

  ASSERT_EQ(-1L, pathconf(NULL, _PC_NAME_MAX));
  ASSERT_EQ(255L, pathconf("a", _PC_NAME_MAX));
  ASSERT_EQ(260L, pathconf("a", _PC_PATH_MAX));
  ASSERT_EQ(4096L, pathconf("a", _PC_PIPE_BUF));
  ASSERT_EQ(-1L, pathconf("a", 9999));

  /* getlogin / getlogin_r */
  g_w32_get_user_ok = 0;
  ASSERT(getlogin() == NULL);
  ASSERT_EQ(ERANGE, getlogin_r(NULL, 10));
  ASSERT_EQ(ERANGE, getlogin_r(buf, 0));
  ASSERT_EQ(ERANGE, getlogin_r(buf, sizeof(buf)));
  g_w32_get_user_ok = 1;
  ASSERT(getlogin() != NULL);
  ASSERT_EQ(0, getlogin_r(buf, sizeof(buf)));

  /* pipe / pipe2 */
  ASSERT_EQ(-1, pipe(NULL));
  g_w32_pipe_ret = 0;
  ASSERT_EQ(0, pipe(pfd));
  g_w32_pipe_ret = -1;
  g_w32_socketpair_ret = -1;
  ASSERT_EQ(-1, pipe(pfd));
  g_w32_socketpair_ret = 0;
  ASSERT_EQ(0, pipe(pfd));

  ASSERT_EQ(-1, pipe2(NULL, 0));
  ASSERT_EQ(-1, pipe2(pfd, -1));
  g_w32_socketpair_ret = -1;
  ASSERT_EQ(-1, pipe2(pfd, 0));
  g_w32_socketpair_ret = 0;
  ASSERT_EQ(0, pipe2(pfd, 0));

  /* pread / pwrite */
  ASSERT_EQ((ssize_t)-1, pread(-1, buf, 4, 0));
  ASSERT_EQ((ssize_t)-1, pread(3, NULL, 4, 0));
  ASSERT_EQ((ssize_t)-1, pread(3, buf, 4, -1));
  g_w32_osfh_default = -1;
  ASSERT_EQ((ssize_t)-1, pread(3, buf, 4, 0));
  g_w32_osfh_default = 100;
  g_w32_read_file_ok = 0;
  g_w32_last_error = 38;
  ASSERT_EQ((ssize_t)0, pread(3, buf, 4, 0));
  g_w32_last_error = 5;
  ASSERT_EQ((ssize_t)-1, pread(3, buf, 4, 0));
  g_w32_read_file_ok = 1;
  ASSERT_EQ((ssize_t)4, pread(3, buf, 4, 0));

  ASSERT_EQ((ssize_t)-1, pwrite(-1, buf, 4, 0));
  ASSERT_EQ((ssize_t)-1, pwrite(3, NULL, 4, 0));
  ASSERT_EQ((ssize_t)-1, pwrite(3, buf, 4, -1));
  g_w32_osfh_default = -1;
  ASSERT_EQ((ssize_t)-1, pwrite(3, buf, 4, 0));
  g_w32_osfh_default = 100;
  g_w32_write_file_ok = 0;
  ASSERT_EQ((ssize_t)-1, pwrite(3, buf, 4, 0));
  g_w32_write_file_ok = 1;
  ASSERT_EQ((ssize_t)4, pwrite(3, buf, 4, 0));

  /* sync */
  g_w32_drives_mask = 5UL;
  g_w32_drive_type = 2U;
  sync();
  g_w32_drive_type = 3U;
  g_w32_create_file_ok = 0;
  sync();
  g_w32_create_file_ok = 1;
  sync();

  /* sysconf */
  ASSERT_EQ(4096L, sysconf(_SC_PAGESIZE));
  ASSERT_EQ(4L, sysconf(_SC_NPROCESSORS_ONLN));
  ASSERT_EQ(4L, sysconf(_SC_NPROCESSORS_CONF));
  ASSERT_EQ(100L, sysconf(_SC_PHYS_PAGES));
  ASSERT_EQ(50L, sysconf(_SC_AVPHYS_PAGES));
  g_w32_page_size = 0;
  ASSERT_EQ(-1L, sysconf(_SC_PHYS_PAGES));
  g_w32_page_size = 4096;
  g_w32_memstat_ok = 0;
  ASSERT_EQ(-1L, sysconf(_SC_PHYS_PAGES));
  g_w32_memstat_ok = 1;
  ASSERT_EQ(1000L, sysconf(_SC_CLK_TCK));
  ASSERT_EQ(2048L, sysconf(_SC_OPEN_MAX));
  ASSERT_EQ(-1L, sysconf(9999));

  /* posix_isatty, tcgetpgrp, tcsetpgrp, ttyname, ttyname_r */
  ASSERT_EQ(0, posix_isatty(-1));
  ASSERT_EQ(0, posix_isatty(2048));
  g_w32_isatty_ret = 0;
  ASSERT_EQ(-1, tcgetpgrp(1));
  ASSERT_EQ(-1, tcsetpgrp(1, -1));
  ASSERT_EQ(-1, tcsetpgrp(1, 0));
  ASSERT(ttyname(1) == NULL);
  ASSERT_EQ(EINVAL, ttyname_r(1, NULL, 10));
  ASSERT_EQ(ENOTTY, ttyname_r(1, buf, sizeof(buf)));

  g_w32_isatty_ret = 1;
  ASSERT_EQ(0, tcgetpgrp(1));
  ASSERT_EQ(0, tcsetpgrp(1, 0));
  ASSERT(ttyname(1) != NULL);
  ASSERT_EQ(ERANGE, ttyname_r(1, buf, 2));
  ASSERT_EQ(0, ttyname_r(1, buf, sizeof(buf)));

#if defined(_MSC_VER) && !defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
  {
    char *gargv[8];
    /* Bundled options, inline arg, separate arg, -- terminator */
    gargv[0] = "prog";
    gargv[1] = "-abval";
    gargv[2] = "-b";
    gargv[3] = "val2";
    gargv[4] = "--";
    gargv[5] = NULL;
    optind = 0;
    opterr = 0;
    ASSERT_EQ('a', getopt(5, gargv, "ab:"));
    ASSERT_EQ('b', getopt(5, gargv, "ab:"));
    ASSERT_STR_EQ("val", optarg);
    ASSERT_EQ('b', getopt(5, gargv, "ab:"));
    ASSERT_STR_EQ("val2", optarg);
    ASSERT_EQ(-1, getopt(5, gargv, "ab:"));

    /* Single flag at end of token, "-", non-option, NULL, argc end */
    gargv[1] = "-a";
    gargv[2] = "-";
    optind = 0;
    ASSERT_EQ('a', getopt(3, gargv, "a"));
    ASSERT_EQ(-1, getopt(3, gargv, "a"));
    gargv[1] = "pos";
    optind = 0;
    ASSERT_EQ(-1, getopt(2, gargv, "a"));
    gargv[1] = NULL;
    optind = 0;
    ASSERT_EQ(-1, getopt(2, gargv, "a"));
    optind = 2;
    ASSERT_EQ(-1, getopt(2, gargv, "a"));

    /* Unknown option, ':' option, missing arg with opterr=0/1 and leading ':'
     */
    gargv[1] = "-z:";
    optind = 0;
    opterr = 1;
    ASSERT_EQ('?', getopt(2, gargv, "a:"));
    ASSERT_EQ('?', getopt(2, gargv, ":a:"));
    gargv[1] = "-a";
    optind = 0;
    opterr = 1;
    ASSERT_EQ('?', getopt(2, gargv, "a:"));
    optind = 0;
    opterr = 0;
    ASSERT_EQ(':', getopt(2, gargv, ":a:"));
    optind = 0;
    opterr = 1;
    ASSERT_EQ(':', getopt(2, gargv, ":a:"));
  }
#endif

  reset_w32_mocks();
  PASS();
}

TEST test_core_win32_fork_vfork_exec(void) {
  int i;
  int st = 0;
  int rpid;
  void *slot_val = (void *)(intptr_t)0xBEEF;
  char *argv_q[5];
  char *envp_q[2];
  FILE *f;

  install_w32_mocks();

  /* fexecve */
  argv_q[0] = "cmd";
  argv_q[1] = NULL;
  envp_q[0] = "A=1";
  envp_q[1] = NULL;
  ASSERT_EQ(-1, fexecve(-1, argv_q, envp_q));
  ASSERT_EQ(-1, fexecve(3, NULL, envp_q));
  ASSERT_EQ(-1, fexecve(3, argv_q, NULL));
  g_w32_osfh_default = -1;
  ASSERT_EQ(-1, fexecve(3, argv_q, envp_q));
  g_w32_osfh_default = 100;
  g_w32_mod_k32_null = 1;
  ASSERT_EQ(-1, fexecve(3, argv_q, envp_q));
  g_w32_mod_k32_null = 0;
  g_w32_null_proc_name = "GetFinalPathNameByHandleA";
  ASSERT_EQ(-1, fexecve(3, argv_q, envp_q));
  g_w32_null_proc_name = NULL;
  g_w32_malloc_fail_at = 0;
  g_w32_malloc_calls = 0;
  ASSERT_EQ(-1, fexecve(3, argv_q, envp_q));
  g_w32_malloc_fail_at = -1;
  g_w32_final_path_mode = 2;
  ASSERT_EQ(-1, fexecve(3, argv_q, envp_q));
  g_w32_final_path_mode = 3;
  ASSERT_EQ(-1, fexecve(3, argv_q, envp_q));
  g_w32_final_path_mode = 0;
  ASSERT_EQ(-1, fexecve(3, argv_q, envp_q));
  g_w32_final_path_mode = 1;
  ASSERT_EQ(-1, fexecve(3, argv_q, envp_q));
  g_w32_final_path_mode = 8;
  ASSERT_EQ(-1, fexecve(3, argv_q, envp_q));
  g_w32_final_path_mode = 9;
  ASSERT_EQ(-1, fexecve(3, argv_q, envp_q));
  g_w32_final_path_mode = 10;
  ASSERT_EQ(-1, fexecve(3, argv_q, envp_q));
  g_w32_final_path_mode = 11;
  ASSERT_EQ(-1, fexecve(3, argv_q, envp_q));
  g_w32_final_path_mode = 0;

  /* fork & vfork */
  g_w32_mod_ntdll_null = 1;
  ASSERT_EQ(-1, fork());
  g_w32_mod_ntdll_null = 0;

  g_w32_null_proc_name = "RtlCloneUserProcess";
  ASSERT_EQ(-1, fork());
  g_w32_null_proc_name = NULL;

  g_w32_get_cur_dir_ok = 0;
  ASSERT_EQ(-1, fork());
  g_w32_get_cur_dir_ok = 1;

  posix_core_reset_process_state(0, 0, 0);
  g_w32_clone_status = 0x00000000UL;
  ASSERT_EQ(12345, vfork());
  posix_core_reset_process_state(1024, 0, 0);
  ASSERT_EQ(12345, fork());
  posix_core_reset_process_state(0, 0, 0);

  g_w32_clone_status = 0x00000129UL;
  g_w32_mod_ucrt_null = 1;
  ASSERT_EQ(0, fork());
  g_w32_mod_ucrt_null = 0;

  g_w32_mod_k32_null = 1;
  ASSERT_EQ(0, fork());
  g_w32_mod_k32_null = 0;

  g_w32_null_proc_name = "_exit";
  ASSERT_EQ(0, fork());
  g_w32_null_proc_name = NULL;

  g_w32_vprotect_ok = 0;
  ASSERT_EQ(0, fork());
  g_w32_vprotect_ok = 1;
  ASSERT_EQ(0, fork());

  g_w32_clone_status = 0xC0000001UL;
  ASSERT_EQ(-1, fork());
  ASSERT_EQ(EAGAIN, errno);

  /* awm_vfork_push / jb / parent_pid / depth / exit / try_reap */
  posix_core_reset_process_state(0, 16, 0);
  ASSERT_EQ(-1, awm_vfork_push(NULL));
  posix_core_reset_process_state(0, 0, 0);

  awm_vfork_exit(3);
  ASSERT(g_w32_term_proc_calls > 0);

  g_w32_osfh_map_enabled = 1;
  for (i = 0; i < 160; i++) {
    g_w32_osfh_map[i] = -1;
  }
  g_w32_osfh_map[0] = 100;
  g_w32_osfh_map[1] = 101;
  g_w32_osfh_map[2] = 102;
  g_w32_osfh_map[3] = 103;
  g_w32_osfh_map[50] = 200;
  g_w32_osfh_map[51] = -1;
  g_w32_osfh_map[52] = 202;
  g_w32_dup_fail_fd = 3;

  ASSERT_EQ(0, awm_vfork_push(&slot_val));
  ASSERT_EQ(1, awm_vfork_depth());
  if (setjmp(*(jmp_buf *)awm_vfork_jb()) == 0) {
    slot_val = NULL;
    g_w32_osfh_map[4] = 104;
    g_w32_osfh_map[2] = -1;
    awm_vfork_exit(5);
  }
  ASSERT(slot_val == (void *)(intptr_t)0xBEEF);
  rpid = awm_vfork_parent_pid();
  ASSERT(rpid > 40000);
  g_w32_dup_fail_fd = -1;

  /* Push with NULL globals_slot, empty cwd, and reap table full */
  g_w32_getcwd_mode = 2;
  ASSERT_EQ(0, awm_vfork_push(NULL));
  if (setjmp(*(jmp_buf *)awm_vfork_jb()) == 0) {
    posix_core_reset_process_state(0, 1, 64);
    awm_vfork_exit(2);
  }
  g_w32_getcwd_mode = 0;
  posix_core_reset_process_state(0, 0, 0);

  /* Populate reap table with hProcess != NULL and hProcess == NULL */
  g_w32_osfh_map_enabled = 0;
  g_w32_osfh_default = -1;

  argv_q[0] = "cmd";
  argv_q[1] = "a\\b\\\"c\\\\";
  argv_q[2] = "plain\\end\\";
  argv_q[3] = NULL;

  ASSERT_EQ(0, awm_vfork_push(NULL));
  if (setjmp(*(jmp_buf *)awm_vfork_jb()) == 0) {
    g_w32_spawnve_ret = (intptr_t)0x3030;
    (void)posix_execve("prog", argv_q, envp_q);
  }
  rpid = awm_vfork_parent_pid();

  ASSERT_EQ(0, awm_vfork_push(NULL));
  if (setjmp(*(jmp_buf *)awm_vfork_jb()) == 0) {
    g_w32_spawnve_ret = (intptr_t)0x3031;
    (void)posix_execve("prog", argv_q, NULL);
  }

  ASSERT_EQ(0, awm_vfork_push(NULL));
  if (setjmp(*(jmp_buf *)awm_vfork_jb()) == 0) {
    awm_vfork_exit(9);
  }

  ASSERT_EQ(0, awm_vfork_try_reap(999999, &st));
  g_w32_get_exit_code_ok = 1;
  g_w32_exit_code_val = 7;
  ASSERT_EQ(rpid, awm_vfork_try_reap(rpid, &st));
  ASSERT_EQ(7 << 8, st);
  g_w32_get_exit_code_ok = 0;
  ASSERT(awm_vfork_try_reap(-1, NULL) > 0);
  ASSERT(awm_vfork_try_reap(0, &st) > 0);
  ASSERT_EQ(9 << 8, st);
  ASSERT_EQ(0, awm_vfork_try_reap(0, &st));

  /* posix_execve / posix_execv */
  g_w32_osfh_default = 100;
  ASSERT_EQ(-1, posix_execve(NULL, argv_q, NULL));
  ASSERT_EQ(-1, posix_execve("p", NULL, NULL));

  g_w32_spawnve_ret = 0;
  g_w32_mod_filename_fail = 1;
  (void)posix_execve("/proc/self/exe", argv_q, NULL);
  g_w32_mod_filename_fail = 0;
  (void)posix_execve("/proc/self/exe", argv_q, NULL);
  (void)posix_execve("\\proc\\self\\exe", argv_q, NULL);
  (void)posix_execve("busybox.exe", argv_q, NULL);
  (void)posix_execv("busybox", argv_q);

  /* build_quoted_argv OOM */
  g_w32_malloc_fail_at = 0;
  g_w32_malloc_calls = 0;
  (void)posix_execve("p", argv_q, NULL);
  g_w32_malloc_fail_at = 1;
  g_w32_malloc_calls = 0;
  (void)posix_execve("p", argv_q, NULL);
  g_w32_malloc_fail_at = 2;
  g_w32_malloc_calls = 0;
  (void)posix_execve("p", argv_q, NULL);
  g_w32_malloc_fail_at = -1;

  g_w32_spawnve_ret = -1;
  ASSERT_EQ(-1, posix_execve("nonexistent_awm_exec_999", argv_q, NULL));

  f = fopen("awm_noshebang.tmp", "wb");
  ASSERT(f != NULL);
  fwrite("AB", 1, 2, f);
  fclose(f);
  ASSERT_EQ(-1, posix_execve("awm_noshebang.tmp", argv_q, NULL));
  ASSERT_EQ(EACCES, errno);

  f = fopen("awm_oneshebang.tmp", "wb");
  ASSERT(f != NULL);
  fwrite("#", 1, 1, f);
  fclose(f);
  ASSERT_EQ(-1, posix_execve("awm_oneshebang.tmp", argv_q, NULL));

  f = fopen("awm_hashx.tmp", "wb");
  ASSERT(f != NULL);
  fwrite("#X", 1, 2, f);
  fclose(f);
  ASSERT_EQ(-1, posix_execve("awm_hashx.tmp", argv_q, NULL));

  f = fopen("awm_shebang.tmp", "wb");
  ASSERT(f != NULL);
  fwrite("#!/bin/sh\n", 1, 10, f);
  fclose(f);
  ASSERT_EQ(-1, posix_execve("awm_shebang.tmp", argv_q, NULL));
  ASSERT_EQ(ENOEXEC, errno);

  /* posix_execvp */
  ASSERT_EQ(-1, posix_execvp(NULL, argv_q));
  ASSERT_EQ(-1, posix_execvp("p", NULL));

  g_w32_spawnvpe_ret = 0;
  g_w32_mod_filename_fail = 1;
  (void)posix_execvp("/proc/self/exe", argv_q);
  g_w32_mod_filename_fail = 0;
  (void)posix_execvp("/proc/self/exe", argv_q);
  (void)posix_execvp("\\proc\\self\\exe", argv_q);
  (void)posix_execvp("busybox.exe", argv_q);
  (void)posix_execvp("busybox", argv_q);

  g_w32_malloc_fail_at = 0;
  g_w32_malloc_calls = 0;
  (void)posix_execvp("p", argv_q);
  g_w32_malloc_fail_at = -1;

  g_w32_spawnvpe_ret = -1;
  g_w32_spawnve_ret = 0;
  (void)posix_execvp("dir/cmd", argv_q);
  (void)posix_execvp("dir\\cmd", argv_q);
  (void)posix_execvp("nonexistent_cmd_xyz_987", argv_q);
  (void)posix_execvp("no_such_cmd", argv_q);
  (void)posix_execvp("nonexistent_other", argv_q);
  g_w32_mod_filename_fail = 1;
  (void)posix_execvp("ls", argv_q);
  g_w32_mod_filename_fail = 0;
  g_w32_mod_filename = "C:\\bin\\other.exe";
  (void)posix_execvp("ls", argv_q);

  g_w32_mod_filename = "C:\\bin\\busybox.exe";
  (void)posix_execvp("ls", argv_q);
  g_w32_mod_filename = "C:\\bin\\BUSYBOX.EXE";
  g_w32_malloc_fail_at = 4;
  g_w32_malloc_calls = 0;
  (void)posix_execvp("ls", argv_q);
  g_w32_malloc_fail_at = -1;
  {
    char *many_argv[66];
    for (i = 0; i < 65; i++) {
      many_argv[i] = "a";
    }
    many_argv[65] = NULL;
    (void)posix_execvp("ls", many_argv);
  }

  g_w32_mod_filename = "C:\\bin\\other.exe";
  ASSERT_EQ(-1, posix_execvp("awm_noshebang.tmp", argv_q));
  ASSERT_EQ(-1, posix_execvp("awm_oneshebang.tmp", argv_q));
  ASSERT_EQ(-1, posix_execvp("awm_hashx.tmp", argv_q));

  g_w32_mod_filename_fail = 1;
  ASSERT_EQ(-1, posix_execvp("awm_shebang.tmp", argv_q));
  g_w32_mod_filename_fail = 0;

  g_w32_spawnv_ret = -1;
  ASSERT_EQ(-1, posix_execvp("awm_shebang.tmp", argv_q));

  g_w32_spawnv_ret = 0;
  (void)posix_execvp("awm_shebang.tmp", argv_q);

  g_w32_malloc_fail_at = 4;
  g_w32_malloc_calls = 0;
  (void)posix_execvp("awm_shebang.tmp", argv_q);
  g_w32_malloc_fail_at = -1;

  {
    char *many_argv[64];
    for (i = 0; i < 63; i++) {
      many_argv[i] = "a";
    }
    many_argv[63] = NULL;
    (void)posix_execvp("awm_shebang.tmp", many_argv);
  }

  remove("awm_noshebang.tmp");
  remove("awm_oneshebang.tmp");
  remove("awm_hashx.tmp");
  remove("awm_shebang.tmp");

  reset_w32_mocks();
  PASS();
}

TEST test_core_win32_misc_env_and_str(void) {
  char optbuf[64];
  char *optp;
  char *valp;
  char *tokens[3];
  char *line = NULL;
  size_t cap = 0;
  FILE *f;
  char *s;
  char rbuf[64];
  char **saved_env;
  char *saved_env0;

  install_w32_mocks();

  /* getsubopt */
  tokens[0] = "ro";
  tokens[1] = "rw";
  tokens[2] = NULL;
  optp = NULL;
  ASSERT_EQ(-1, getsubopt(NULL, tokens, &valp));
  ASSERT_EQ(-1, getsubopt(&optp, tokens, &valp));
  test_copy_str(optbuf, sizeof(optbuf), " \t");
  optp = optbuf;
  ASSERT_EQ(-1, getsubopt(&optp, NULL, &valp));
  ASSERT_EQ(-1, getsubopt(&optp, tokens, NULL));
  ASSERT_EQ(-1, getsubopt(&optp, tokens, &valp));
  ASSERT(valp == NULL);

  test_copy_str(optbuf, sizeof(optbuf), " \tro,rw=1,unknown");
  optp = optbuf;
  ASSERT_EQ(0, getsubopt(&optp, tokens, &valp));
  ASSERT(valp == NULL);
  ASSERT_EQ(1, getsubopt(&optp, tokens, &valp));
  ASSERT_STR_EQ("1", valp);
  ASSERT_EQ(-1, getsubopt(&optp, tokens, &valp));
  ASSERT_STR_EQ("unknown", valp);

  /* getdelim & getline */
  f = fopen("awm_getdelim.tmp", "wb+");
  ASSERT(f != NULL);
  ASSERT_EQ((ssize_t)-1, getdelim(NULL, &cap, '\n', f));
  ASSERT_EQ((ssize_t)-1, getdelim(&line, NULL, '\n', f));
  ASSERT_EQ((ssize_t)-1, getdelim(&line, &cap, '\n', NULL));

  g_w32_malloc_fail_at = 0;
  g_w32_malloc_calls = 0;
  ASSERT_EQ((ssize_t)-1, getdelim(&line, &cap, '\n', f));
  g_w32_malloc_fail_at = -1;

  line = NULL;
  cap = 0;
  ASSERT_EQ((ssize_t)-1, getline(&line, &cap, f));
  ASSERT(line != NULL);
  free(line);

  /* Non-NULL line with cap == 0 */
  line = (char *)malloc(4);
  cap = 0;
  free(line);
  line = (char *)malloc(4);
  {
    char *orig = line;
    ASSERT_EQ((ssize_t)-1, getline(&line, &cap, f));
    free(orig);
    free(line);
  }

  {
    char big[200];
    memset(big, 'A', 150);
    big[150] = '\n';
    big[151] = 'B';
    fwrite(big, 1, 152, f);
  }
  rewind(f);
  line = (char *)malloc(8);
  cap = 8;
  g_w32_realloc_fail = 1;
  ASSERT_EQ((ssize_t)-1, getdelim(&line, &cap, '\n', f));
  g_w32_realloc_fail = 0;
  rewind(f);
  ASSERT_EQ((ssize_t)151, getline(&line, &cap, f));
  ASSERT_EQ((ssize_t)1, getline(&line, &cap, f));
  free(line);
  fclose(f);
  remove("awm_getdelim.tmp");

  /* setenv / unsetenv */
  ASSERT_EQ(-1, setenv(NULL, "1", 1));
  ASSERT_EQ(-1, setenv("", "1", 1));
  ASSERT_EQ(-1, setenv("A=B", "1", 1));
  ASSERT_EQ(-1, unsetenv(NULL));
  ASSERT_EQ(-1, unsetenv(""));
  ASSERT_EQ(-1, unsetenv("A=B"));

  ASSERT_EQ(0, unsetenv("AWM_COV_ENV_VAR"));
  ASSERT_EQ(0, setenv("AWM_COV_ENV_VAR", "v1", 0));
  ASSERT_EQ(0, setenv("AWM_COV_ENV_VAR", "v2", 0));
  ASSERT_EQ(0, setenv("AWM_COV_ENV_VAR", NULL, 1));
#if !defined(_MSC_VER)
  g_w32_malloc_fail_at = 0;
  g_w32_malloc_calls = 0;
  ASSERT_EQ(-1, setenv("AWM_COV_ENV_VAR", "v3", 1));
  g_w32_malloc_fail_at = -1;
#endif
  ASSERT_EQ(0, unsetenv("AWM_COV_ENV_VAR"));

  /* clearenv: non-NULL environ and NULL environ (save & restore safely) */
  saved_env = _environ;
  saved_env0 = (saved_env != NULL) ? saved_env[0] : NULL;
  ASSERT_EQ(0, clearenv());
  if (saved_env != NULL) {
    saved_env[0] = saved_env0;
  }
  _environ = NULL;
  ASSERT_EQ(0, clearenv());
  _environ = saved_env;

  /* strndup */
  ASSERT(strndup(NULL, 5) == NULL);
  g_w32_malloc_fail_at = 0;
  g_w32_malloc_calls = 0;
  ASSERT(strndup("hello", 3) == NULL);
  g_w32_malloc_fail_at = -1;
  s = strndup("hello", 3);
  ASSERT(s != NULL);
  ASSERT_STR_EQ("hel", s);
  free(s);
  s = strndup("hi", 10);
  ASSERT(s != NULL);
  ASSERT_STR_EQ("hi", s);
  free(s);

  /* getpagesize */
  ASSERT_EQ(4096, getpagesize());

  /* realpath */
  ASSERT(realpath(NULL, rbuf) == NULL);
  s = realpath(".", NULL);
  ASSERT(s != NULL);
  free(s);
  ASSERT(realpath(".", rbuf) == rbuf);

  /* fchdir / chroot / settimeofday */
  ASSERT_EQ(-1, fchdir(-1));
  ASSERT_EQ(EBADF, errno);
  ASSERT_EQ(-1, fchdir(0));
  ASSERT_EQ(ENOSYS, errno);

  ASSERT_EQ(-1, chroot(NULL));
  ASSERT_EQ(EINVAL, errno);
  ASSERT_EQ(-1, chroot("/"));
  ASSERT_EQ(ENOSYS, errno);

  ASSERT_EQ(0, settimeofday(NULL, NULL));
  ASSERT_EQ(-1, settimeofday((const void *)1, NULL));
  ASSERT_EQ(-1, settimeofday(NULL, (const void *)1));

  reset_w32_mocks();
  PASS();
}
#endif

SUITE(suite_posix_core_coverage) {
  RUN_TEST(test_core_common_hooks_and_scratch);
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
  RUN_TEST(test_core_posix_branches);
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
  RUN_TEST(test_core_win32_default_wrappers);
  RUN_TEST(test_core_win32_rid_ugid_and_exit);
  RUN_TEST(test_core_win32_fcntl_and_lockf);
  RUN_TEST(test_core_win32_at_paths_and_fs);
  RUN_TEST(test_core_win32_io_advice_crypto_sys);
  RUN_TEST(test_core_win32_fork_vfork_exec);
  RUN_TEST(test_core_win32_misc_env_and_str);
#endif
}
