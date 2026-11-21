/**
 * @file test_core_win32_cov2.c
 * @brief Win32 unit and branch coverage tests for posix-core.c.
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

#if defined(_WIN32) && !defined(__CYGWIN__)
extern void posix_set_dup_socket(intptr_t fd, intptr_t sock);
extern void posix_register_crt_socket(intptr_t crt_fd, intptr_t sock);
extern void set_nonblock(uintptr_t s, int nb);
extern int get_nonblock(uintptr_t s);
extern struct posix_core_win32_hooks g_w32_def;
extern int g_w32_malloc_fail_at;
extern int g_w32_malloc_calls;
extern int g_w32_realloc_fail;
extern int g_w32_use_osfh;
extern intptr_t g_w32_osfh_default;
extern int g_w32_osfh_map_enabled;
extern intptr_t g_w32_osfh_map[160];
extern int g_w32_open_osfh_ret;
extern int g_w32_dup_ret;
extern int g_w32_dup_fail_fd;
extern int g_w32_dup2_ret;
extern int g_w32_close_ret;
extern int g_w32_commit_ret;
extern int g_w32_locking_ret;
extern int g_w32_pipe_ret;
extern int g_w32_isatty_ret;
extern int g_w32_open_rdwr_ret;
extern int g_w32_chsize_ret;
extern int g_w32_access_ret;
extern int g_w32_chmod_ret;
extern int g_w32_unlink_ret;
extern int g_w32_rmdir_ret;
extern int g_w32_getcwd_mode;
extern int g_w32_chdir_ret;
extern int g_w32_fullpath_null;
extern int g_w32_mktemp_ret;
extern intptr_t g_w32_execve_ret;
extern intptr_t g_w32_spawnve_ret;
extern intptr_t g_w32_spawnvpe_ret;
extern intptr_t g_w32_spawnv_ret;
extern int g_w32_raise_calls;
extern int g_w32_mod_advapi_null;
extern int g_w32_mod_k32_null;
extern int g_w32_mod_ntdll_null;
extern int g_w32_mod_ucrt_null;
extern const char *g_w32_null_proc_name;
extern unsigned long g_w32_last_error;
extern int g_w32_get_fsize_ok;
extern posix_core_i64_t g_w32_fsize_val;
extern int g_w32_set_fp_fail_at;
extern int g_w32_set_fp_calls;
extern posix_core_i64_t g_w32_fp_pos;
extern int g_w32_set_eof_ok;
extern int g_w32_lock_ok;
extern int g_w32_unlock_ok;
extern int g_w32_cfm_ok;
extern int g_w32_mvof_ok;
extern int g_w32_create_timer_invoke;
extern unsigned long g_w32_tick_count;
extern unsigned long g_w32_file_attrs;
extern int g_w32_get_cur_dir_ok;
extern int g_w32_create_dir_ok;
extern int g_w32_create_file_ok;
extern int g_w32_read_file_ok;
extern unsigned long g_w32_read_file_bytes;
extern int g_w32_write_file_ok;
extern unsigned long g_w32_write_file_bytes;
extern int g_w32_dev_io_ok;
extern unsigned long g_w32_reparse_tag;
extern int g_w32_wctmb_mode;
extern int g_w32_wctmb_calls;
extern int g_w32_hard_link_ok;
extern int g_w32_move_file_ok;
extern int g_w32_copy_file_fail_count;
extern int g_w32_get_user_ok;
extern unsigned long g_w32_drives_mask;
extern unsigned int g_w32_drive_type;
extern unsigned long g_w32_page_size;
extern unsigned long g_w32_num_procs;
extern int g_w32_memstat_ok;
extern const char *g_w32_mod_filename;
extern int g_w32_mod_filename_fail;
extern int g_w32_get_exit_code_ok;
extern unsigned long g_w32_exit_code_val;
extern int g_w32_ioctl_fail_count;
extern int g_w32_wsa_err;
extern int g_w32_socketpair_ret;
extern int g_w32_crypt_fail_step;
extern int g_w32_crypt_hash_calls;
extern int g_w32_open_token_ok;
extern unsigned long g_w32_token_len;
extern int g_w32_token_info_ok;
extern int g_w32_token_sid_null;
extern int g_w32_valid_sid_ok;
extern int g_w32_subauth_cnt_null;
extern unsigned char g_w32_subauth_cnt;
extern int g_w32_subauth_null;
extern unsigned long g_w32_rid_seq[4];
extern int g_w32_rid_seq_idx;
extern int g_w32_exit_proc_calls;
extern int g_w32_term_proc_calls;
extern int g_w32_final_path_mode;
extern int g_w32_symlink_fail_count;
extern unsigned long g_w32_clone_status;
extern int g_w32_vprotect_ok;
extern unsigned char g_w32_fake_map_buf[64];
void install_w32_mocks(void);
void reset_w32_mocks(void);
void noop_sig_handler(int s);
static void test_copy_str(char *dst, size_t cap, const char *src) {
#if defined(_MSC_VER)
  strcpy_s(dst, cap, src);
#else
  (void)cap;
  strcpy(dst, src);
#endif
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
  static char s_cmd[] = "cmd";
  static char s_env_a1[] = "A=1";
  static char s_q1[] = "a\\b\\\"c\\\\";
  static char s_q2[] = "plain\\end\\";
  static char s_a[] = "a";
  FILE *f;

  install_w32_mocks();

  /* fexecve */
  argv_q[0] = s_cmd;
  argv_q[1] = NULL;
  envp_q[0] = s_env_a1;
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
  ASSERT(awm_vfork_jb() != NULL);

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

  argv_q[0] = s_cmd;
  argv_q[1] = s_q1;
  argv_q[2] = s_q2;
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
      many_argv[i] = s_a;
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
      many_argv[i] = s_a;
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
  char rbuf[1024];
  char **saved_env;
  char *saved_env0;

  install_w32_mocks();

  /* getsubopt */
  {
    static char s_ro[] = "ro";
    static char s_rw[] = "rw";
    tokens[0] = s_ro;
    tokens[1] = s_rw;
    tokens[2] = NULL;
  }
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

SUITE(suite_posix_core_win32_cov2) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  RUN_TEST(test_core_win32_at_paths_and_fs);
  RUN_TEST(test_core_win32_io_advice_crypto_sys);
  RUN_TEST(test_core_win32_fork_vfork_exec);
  RUN_TEST(test_core_win32_misc_env_and_str);
#endif
}
