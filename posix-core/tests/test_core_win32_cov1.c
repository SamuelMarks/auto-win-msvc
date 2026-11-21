/**
 * @file test_core_win32_cov1.c
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
#if defined(_MSC_VER) || defined(__WATCOMC__)
typedef unsigned __int64 test_cov_u64_t;
#elif defined(__GNUC__) || defined(__clang__)
__extension__ typedef unsigned long long test_cov_u64_t;
#else
typedef unsigned long test_cov_u64_t;
#endif
typedef struct _TEST_MEM_STAT {
  unsigned long dwLength;
  unsigned long dwMemoryLoad;
  test_cov_u64_t ullTotalPhys;
  test_cov_u64_t ullAvailPhys;
  test_cov_u64_t ullTotalPageFile;
  test_cov_u64_t ullAvailPageFile;
  test_cov_u64_t ullTotalVirtual;
  test_cov_u64_t ullAvailVirtual;
  test_cov_u64_t ullAvailExtendedVirtual;
} TEST_MEM_STAT;
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

#endif

SUITE(suite_posix_core_win32_cov1) {
#if defined(_WIN32) && !defined(__CYGWIN__)
  RUN_TEST(test_core_win32_default_wrappers);
  RUN_TEST(test_core_win32_rid_ugid_and_exit);
  RUN_TEST(test_core_win32_fcntl_and_lockf);
#endif
}
