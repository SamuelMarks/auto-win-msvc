/**
 * @file test_core_win32_mocks.c
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

static void test_copy_str(char *dst, size_t cap, const char *src) {
#if defined(_MSC_VER)
  strcpy_s(dst, cap, src);
#else
  (void)cap;
  strcpy(dst, src);
#endif
}

/* Extern socket helpers exported by posix-sockets / posix_read_write */
extern void posix_set_dup_socket(intptr_t fd, intptr_t sock);
extern void posix_register_crt_socket(intptr_t crt_fd, intptr_t sock);
extern void set_nonblock(uintptr_t s, int nb);
extern int get_nonblock(uintptr_t s);

extern struct posix_core_win32_hooks g_w32_def;
struct posix_core_win32_hooks g_w32_def;

/* --- Win32 mock state --- */
extern int g_w32_malloc_fail_at;
int g_w32_malloc_fail_at = -1;
extern int g_w32_malloc_calls;
int g_w32_malloc_calls = 0;
extern int g_w32_realloc_fail;
int g_w32_realloc_fail = 0;

extern int g_w32_use_osfh;
int g_w32_use_osfh = 0;
extern intptr_t g_w32_osfh_default;
intptr_t g_w32_osfh_default = 100;
extern int g_w32_osfh_map_enabled;
int g_w32_osfh_map_enabled = 0;
extern intptr_t g_w32_osfh_map[160];
intptr_t g_w32_osfh_map[160];

extern int g_w32_open_osfh_ret;
int g_w32_open_osfh_ret = -2;
extern int g_w32_dup_ret;
int g_w32_dup_ret = -2;
extern int g_w32_dup_fail_fd;
int g_w32_dup_fail_fd = -1;
extern int g_w32_dup2_ret;
int g_w32_dup2_ret = -2;
extern int g_w32_close_ret;
int g_w32_close_ret = 0;
extern int g_w32_commit_ret;
int g_w32_commit_ret = 0;
extern int g_w32_locking_ret;
int g_w32_locking_ret = 0;
extern int g_w32_pipe_ret;
int g_w32_pipe_ret = -1;
extern int g_w32_isatty_ret;
int g_w32_isatty_ret = 0;
extern int g_w32_open_rdwr_ret;
int g_w32_open_rdwr_ret = 5;
extern int g_w32_chsize_ret;
int g_w32_chsize_ret = 0;
extern int g_w32_access_ret;
int g_w32_access_ret = 0;
extern int g_w32_chmod_ret;
int g_w32_chmod_ret = 0;
extern int g_w32_unlink_ret;
int g_w32_unlink_ret = 0;
extern int g_w32_rmdir_ret;
int g_w32_rmdir_ret = 0;
extern int g_w32_getcwd_mode;
int g_w32_getcwd_mode = 0;
extern int g_w32_chdir_ret;
int g_w32_chdir_ret = 0;
extern int g_w32_fullpath_null;
int g_w32_fullpath_null = 0;
extern int g_w32_mktemp_ret;
int g_w32_mktemp_ret = 0;
extern intptr_t g_w32_execve_ret;
intptr_t g_w32_execve_ret = -1;
extern intptr_t g_w32_spawnve_ret;
intptr_t g_w32_spawnve_ret = 0;
extern intptr_t g_w32_spawnvpe_ret;
intptr_t g_w32_spawnvpe_ret = 0;
extern intptr_t g_w32_spawnv_ret;
intptr_t g_w32_spawnv_ret = 0;
extern int g_w32_raise_calls;
int g_w32_raise_calls = 0;

extern int g_w32_mod_advapi_null;
int g_w32_mod_advapi_null = 0;
extern int g_w32_mod_k32_null;
int g_w32_mod_k32_null = 0;
extern int g_w32_mod_ntdll_null;
int g_w32_mod_ntdll_null = 0;
extern int g_w32_mod_ucrt_null;
int g_w32_mod_ucrt_null = 0;
extern const char *g_w32_null_proc_name;
const char *g_w32_null_proc_name = NULL;

extern unsigned long g_w32_last_error;
unsigned long g_w32_last_error = 0;
extern int g_w32_get_fsize_ok;
int g_w32_get_fsize_ok = 1;
extern posix_core_i64_t g_w32_fsize_val;
posix_core_i64_t g_w32_fsize_val = 100;
extern int g_w32_set_fp_fail_at;
int g_w32_set_fp_fail_at = -1;
extern int g_w32_set_fp_calls;
int g_w32_set_fp_calls = 0;
extern posix_core_i64_t g_w32_fp_pos;
posix_core_i64_t g_w32_fp_pos = 10;
extern int g_w32_set_eof_ok;
int g_w32_set_eof_ok = 1;
extern int g_w32_lock_ok;
int g_w32_lock_ok = 1;
extern int g_w32_unlock_ok;
int g_w32_unlock_ok = 1;
extern int g_w32_cfm_ok;
int g_w32_cfm_ok = 1;
extern int g_w32_mvof_ok;
int g_w32_mvof_ok = 1;
extern int g_w32_create_timer_invoke;
int g_w32_create_timer_invoke = 0;
extern unsigned long g_w32_tick_count;
unsigned long g_w32_tick_count = 1000;
extern unsigned long g_w32_file_attrs;
unsigned long g_w32_file_attrs = 0x80UL;
extern int g_w32_get_cur_dir_ok;
int g_w32_get_cur_dir_ok = 1;
extern int g_w32_create_dir_ok;
int g_w32_create_dir_ok = 1;
extern int g_w32_create_file_ok;
int g_w32_create_file_ok = 1;
extern int g_w32_read_file_ok;
int g_w32_read_file_ok = 1;
extern unsigned long g_w32_read_file_bytes;
unsigned long g_w32_read_file_bytes = 4;
extern int g_w32_write_file_ok;
int g_w32_write_file_ok = 1;
extern unsigned long g_w32_write_file_bytes;
unsigned long g_w32_write_file_bytes = 4;
extern int g_w32_dev_io_ok;
int g_w32_dev_io_ok = 1;
extern unsigned long g_w32_reparse_tag;
unsigned long g_w32_reparse_tag = 0xA000000CUL;
extern int g_w32_wctmb_mode;
int g_w32_wctmb_mode = 0;
extern int g_w32_wctmb_calls;
int g_w32_wctmb_calls = 0;
extern int g_w32_hard_link_ok;
int g_w32_hard_link_ok = 1;
extern int g_w32_move_file_ok;
int g_w32_move_file_ok = 1;
extern int g_w32_copy_file_fail_count;
int g_w32_copy_file_fail_count = 0;
extern int g_w32_get_user_ok;
int g_w32_get_user_ok = 1;
extern unsigned long g_w32_drives_mask;
unsigned long g_w32_drives_mask = 0;
extern unsigned int g_w32_drive_type;
unsigned int g_w32_drive_type = 3U;
extern unsigned long g_w32_page_size;
unsigned long g_w32_page_size = 4096UL;
extern unsigned long g_w32_num_procs;
unsigned long g_w32_num_procs = 4UL;
extern int g_w32_memstat_ok;
int g_w32_memstat_ok = 1;
extern const char *g_w32_mod_filename;
const char *g_w32_mod_filename = "C:\\bin\\busybox.exe";
extern int g_w32_mod_filename_fail;
int g_w32_mod_filename_fail = 0;
extern int g_w32_get_exit_code_ok;
int g_w32_get_exit_code_ok = 1;
extern unsigned long g_w32_exit_code_val;
unsigned long g_w32_exit_code_val = 7;
extern int g_w32_ioctl_fail_count;
int g_w32_ioctl_fail_count = 0;
extern int g_w32_wsa_err;
int g_w32_wsa_err = 0;
extern int g_w32_socketpair_ret;
int g_w32_socketpair_ret = 0;
extern int g_w32_crypt_fail_step;
int g_w32_crypt_fail_step = 0;
extern int g_w32_crypt_hash_calls;
int g_w32_crypt_hash_calls = 0;

/* Dynamic proc mocks */
extern int g_w32_open_token_ok;
int g_w32_open_token_ok = 1;
extern unsigned long g_w32_token_len;
unsigned long g_w32_token_len = 16;
extern int g_w32_token_info_ok;
int g_w32_token_info_ok = 1;
extern int g_w32_token_sid_null;
int g_w32_token_sid_null = 0;
extern int g_w32_valid_sid_ok;
int g_w32_valid_sid_ok = 1;
extern int g_w32_subauth_cnt_null;
int g_w32_subauth_cnt_null = 0;
extern unsigned char g_w32_subauth_cnt;
unsigned char g_w32_subauth_cnt = 2;
extern int g_w32_subauth_null;
int g_w32_subauth_null = 0;
extern unsigned long g_w32_rid_seq[4];
unsigned long g_w32_rid_seq[4] = {1000, 1000, 1000, 1000};
extern int g_w32_rid_seq_idx;
int g_w32_rid_seq_idx = 0;

extern int g_w32_exit_proc_calls;
int g_w32_exit_proc_calls = 0;
extern int g_w32_term_proc_calls;
int g_w32_term_proc_calls = 0;
extern int g_w32_final_path_mode;
int g_w32_final_path_mode = 0;
extern int g_w32_symlink_fail_count;
int g_w32_symlink_fail_count = 0;
extern unsigned long g_w32_clone_status;
unsigned long g_w32_clone_status = 0;
extern int g_w32_vprotect_ok;
int g_w32_vprotect_ok = 1;
static unsigned char g_w32_fake_exit_code[2][32];
extern unsigned char g_w32_fake_map_buf[64];
unsigned char g_w32_fake_map_buf[64];

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

void install_w32_mocks(void);
void install_w32_mocks(void) {
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

void reset_w32_mocks(void);
void reset_w32_mocks(void) {
  posix_core_reset_process_state(0, 0, 0);
  posix_core_set_win32_hooks(NULL);
  posix_core_set_common_hooks(NULL);
}

void noop_sig_handler(int s);
void noop_sig_handler(int s) { (void)s; }

#endif
typedef int make_iso_compilers_happy_tu_test_core_w32_mocks;
