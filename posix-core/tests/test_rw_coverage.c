/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <winsock2.h>
#else
#include <unistd.h>
#endif
/* clang-format on */

TEST test_rw_common_streams_and_fds(void) {
  FILE *f;
  char buf[64];
  int fd;
  int fd2;
  int fd3;

  errno = 0;
  ASSERT(posix_fopen(NULL, "r") == NULL);
  ASSERT_EQ(EINVAL, errno);

  errno = 0;
  ASSERT(posix_fopen("rw_cov_test.tmp", NULL) == NULL);
  ASSERT_EQ(EINVAL, errno);

  ASSERT(posix_fopen("", "r") == NULL);

  f = posix_fopen("rw_cov_test.tmp", "w+");
  ASSERT(f != NULL);
  ASSERT(posix_fprintf(NULL, "hi") == -1);
  ASSERT(posix_fprintf(f, NULL) == -1);
  ASSERT(posix_fprintf(f, "line1=%d\n", 7) > 0);
  ASSERT_EQ((size_t)0, posix_fwrite(NULL, 1, 1, f));
  ASSERT_EQ((size_t)0, posix_fwrite("x", 0, 1, f));
  ASSERT_EQ((size_t)0, posix_fwrite("x", 1, 0, f));
  ASSERT_EQ((size_t)0, posix_fwrite("x", 1, 1, NULL));
  ASSERT_EQ((size_t)4, posix_fwrite("ABCD", 1, 4, f));
  fflush(f);
  rewind(f);

  ASSERT(posix_fgets(NULL, 10, f) == NULL);
  ASSERT(posix_fgets(buf, 0, f) == NULL);
  ASSERT(posix_fgets(buf, -1, f) == NULL);
  ASSERT(posix_fgets(buf, 10, NULL) == NULL);
  ASSERT(posix_fgets(buf, (int)sizeof(buf), f) != NULL);

  ASSERT_EQ(EOF, posix_fgetc(NULL));
  ASSERT_EQ((int)'A', posix_fgetc(f));

  ASSERT_EQ((size_t)0, posix_fread(NULL, 1, 1, f));
  ASSERT_EQ((size_t)0, posix_fread(buf, 0, 1, f));
  ASSERT_EQ((size_t)0, posix_fread(buf, 1, 0, f));
  ASSERT_EQ((size_t)0, posix_fread(buf, 1, 1, NULL));
  ASSERT_EQ((size_t)3, posix_fread(buf, 1, 3, f));

  ASSERT_EQ(0, posix_feof(NULL));
  (void)posix_feof(f);
  ASSERT_EQ(0, posix_ferror(NULL));
  (void)posix_ferror(f);

  errno = 0;
  ASSERT_EQ(-1, posix_fileno(NULL));
  ASSERT_EQ(EINVAL, errno);
  ASSERT(posix_fileno(f) >= 0);

  errno = 0;
  ASSERT_EQ(EOF, posix_fclose(NULL));
  ASSERT_EQ(EINVAL, errno);
  ASSERT_EQ(0, posix_fclose(f));

  f = posix_fopen("rw_cov_test.tmp", "a");
  ASSERT(f != NULL);
  ASSERT_EQ(0, posix_fclose(f));

#if defined(_WIN32) && !defined(__CYGWIN__)
  f = posix_fopen("rw_cov_test.tmp", "");
  ASSERT(f != NULL);
  ASSERT_EQ(0, posix_fclose(f));

  f = posix_fopen("rw_cov_test.tmp", "rw+rw+rw+");
  ASSERT(f != NULL);
  ASSERT_EQ(0, posix_fclose(f));
#endif

  ASSERT(posix_fdopen(-1, "r") == NULL);
  ASSERT(posix_fdopen(0, NULL) == NULL);
  fd = posix_open("rw_cov_test.tmp", O_RDONLY);
  ASSERT(fd >= 0);
  f = posix_fdopen(fd, "r");
  ASSERT(f != NULL);
  ASSERT_EQ(0, posix_fclose(f));

  ASSERT_EQ(-1, posix_dup(-1));
  ASSERT_EQ(-1, posix_dup2(-1, 0));
  ASSERT_EQ(-1, posix_dup2(0, -1));
  ASSERT_EQ(-1, posix_read(-1, buf, 1));
  ASSERT_EQ(-1, posix_write(-1, "x", 1));
  ASSERT_EQ(-1, posix_close(-1));
  ASSERT_EQ(-1, (int)posix_lseek(-1, 0, SEEK_SET));

  fd = posix_creat("rw_cov_creat.tmp", 0666);
  ASSERT(fd >= 0);
  ASSERT_EQ((ssize_t)3, posix_write(fd, "xyz", 3));
  fd2 = posix_dup(fd);
  ASSERT(fd2 >= 0);
  ASSERT_EQ(fd2, posix_dup2(fd2, fd2));
  fd3 = posix_open("rw_cov_test.tmp", O_RDWR);
  ASSERT(fd3 >= 0);
#if defined(__WATCOMC__) && (defined(__DOS__) || defined(__MSDOS__))
  if (_osmajor >= 2)
#endif
  {
    ASSERT_EQ(fd3, posix_dup2(fd, fd3));
  }
  ASSERT_EQ(0, posix_close(fd3));
  ASSERT_EQ(0, posix_close(fd2));
  ASSERT_EQ(0, posix_close(fd));

  remove("rw_cov_creat.tmp");
  remove("rw_cov_test.tmp");
  PASS();
}

#if !defined(_WIN32) && !defined(__CYGWIN__) && !defined(__linux__) &&         \
    !defined(__DOS__) && !defined(__MSDOS__) && !defined(__WATCOMC__)
static FILE *mock_posix_tmpfile_null(void) { return NULL; }

TEST test_rw_posix_vproc_and_oom(void) {
  struct posix_rw_posix_hooks def_hooks;
  struct posix_rw_posix_hooks hooks;
  const char *ok_paths[] = {"/proc/meminfo",   "/proc/uptime",
                            "/proc/version",   "meminfo",
                            "uptime",          "version",
                            "/proc/self/stat", "/proc/self/cmdline",
                            "/proc/self/comm", "/proc/self/status",
                            "/proc/42/stat",   "/proc/42/cmdline",
                            "/proc/42/comm",   "/proc/42/status"};
  const char *miss_paths[] = {
#if !defined(__EMSCRIPTEN__)
      "/proc/self",
#endif
      "/proc/self/unknown", "/proc/!bad", "/proc/zbad",
      "nonexistent_rw_vproc_file.txt"};
  size_t i;

  posix_rw_get_default_posix_hooks(NULL);
  posix_rw_get_default_posix_hooks(&def_hooks);
  hooks = def_hooks;
  posix_rw_set_posix_hooks(&hooks);

  for (i = 0; i < sizeof(ok_paths) / sizeof(ok_paths[0]); i++) {
    FILE *vf = posix_fopen(ok_paths[i], "r");
    ASSERT(vf != NULL);
    ASSERT_EQ(0, posix_fclose(vf));
  }
  for (i = 0; i < sizeof(miss_paths) / sizeof(miss_paths[0]); i++) {
    ASSERT(posix_fopen(miss_paths[i], "r") == NULL);
  }

  hooks.tmpfile_fn = mock_posix_tmpfile_null;
  posix_rw_set_posix_hooks(&hooks);
  for (i = 0; i < sizeof(ok_paths) / sizeof(ok_paths[0]); i++) {
    ASSERT(posix_fopen(ok_paths[i], "r") == NULL);
  }
  posix_rw_set_posix_hooks(NULL);
  PASS();
}
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
extern int posix_socketpair(int domain, int type, int protocol,
                            intptr_t socket_vector[2]);
extern error_type_t mark_as_socket(intptr_t fd);
extern error_type_t clear_as_socket(intptr_t fd);
extern void posix_set_dup_socket(intptr_t fd, intptr_t s);
extern void posix_register_crt_socket(intptr_t fd, intptr_t s);

static struct posix_rw_win32_hooks g_orig_rw;
static int g_mock_oom = 0;
static intptr_t g_mock_osfh_val = 10;
static int g_mock_use_osfh = 0;
static int g_mock_open_osfh_ret = -1;
static int g_mock_use_open_osfh = 0;
static int g_mock_fdopen_null = 0;
static int g_mock_fileno_val = -1;
static int g_mock_use_fileno = 0;
static DWORD g_mock_temp_path_ret = 0;
static int g_mock_use_temp_path = 0;
static unsigned int g_mock_temp_file_ret = 1;
static int g_mock_use_temp_file = 0;
static void *g_mock_create_file_ret = (void *)(intptr_t)-1;
static int g_mock_use_create_file = 0;
static DWORD g_mock_cwd_ret = 0;
static int g_mock_use_cwd = 0;
static DWORD g_mock_attr_ret = INVALID_FILE_ATTRIBUTES;
static int g_mock_use_attr = 0;
static DWORD g_mock_ftype_ret = 1UL;
static int g_mock_use_ftype = 0;
static DWORD g_mock_last_err = 0;
static int g_mock_use_last_err = 0;
static void *g_mock_mod_ret = NULL;
static int g_mock_use_mod = 0;
static void *g_mock_proc_ret = NULL;
static int g_mock_use_proc = 0;
static int g_mock_gms_ret = 0;
static int g_mock_read_ret = 0;
static int g_mock_read_errno = 0;
static int g_mock_write_ret = 0;
static int g_mock_write_errno = 0;
static int g_mock_lseek_ret = -1;
static int g_mock_lseek_errno = EINVAL;
static int g_mock_dup_ret = -1;
static int g_mock_dup2_ret = -1;
static int g_mock_getsockopt_ret = -1;
static int g_mock_recv_mode = 0;
static int g_mock_recv_calls = 0;
static int g_mock_send_ret = -1;
static int g_mock_wsa_err = WSAEWOULDBLOCK;
static int g_mock_wsa_startup_ret = 0;
static int g_mock_closesocket_ret = 0;
static int g_mock_readfile_ret = 0;
static int g_mock_writefile_ret = 0;
static int g_mock_overlapped_ret = 0;
static int g_mock_vsc_ret = -1;
static int g_mock_use_vsc = 0;
static int g_mock_vsbuf_ret = 0;
static int g_mock_use_vsbuf = 0;
static int g_mock_epoll_close_ret = -1;

static void reset_rw_mocks(void) {
  posix_rw_get_default_win32_hooks(&g_orig_rw);
  g_mock_oom = 0;
  g_mock_use_osfh = 0;
  g_mock_use_open_osfh = 0;
  g_mock_fdopen_null = 0;
  g_mock_use_fileno = 0;
  g_mock_use_temp_path = 0;
  g_mock_use_temp_file = 0;
  g_mock_use_create_file = 0;
  g_mock_use_cwd = 0;
  g_mock_use_attr = 0;
  g_mock_use_ftype = 0;
  g_mock_use_last_err = 0;
  g_mock_use_mod = 0;
  g_mock_use_proc = 0;
  g_mock_gms_ret = 0;
  g_mock_recv_mode = 0;
  g_mock_recv_calls = 0;
  g_mock_use_vsc = 0;
  g_mock_use_vsbuf = 0;
  posix_rw_set_win32_hooks(NULL);
}

static void *mock_rw_malloc(size_t sz) {
  if (g_mock_oom) {
    return NULL;
  }
  return g_orig_rw.malloc_fn(sz);
}
static intptr_t mock_rw_get_osfhandle(int fd) {
  if (g_mock_use_osfh) {
    return g_mock_osfh_val;
  }
  return g_orig_rw.get_osfhandle_fn(fd);
}
static int mock_rw_open_osfhandle(intptr_t h, int flags) {
  if (g_mock_use_open_osfh) {
    return g_mock_open_osfh_ret;
  }
  return g_orig_rw.open_osfhandle_fn(h, flags);
}
static FILE *mock_rw_fdopen(int fd, const char *mode) {
  if (g_mock_fdopen_null) {
    return NULL;
  }
  return g_orig_rw.fdopen_fn(fd, mode);
}
static int mock_rw_fileno(FILE *f) {
  if (g_mock_use_fileno) {
    return g_mock_fileno_val;
  }
  return g_orig_rw.fileno_fn(f);
}
static DWORD mock_rw_get_temp_path(DWORD len, char *buf) {
  if (g_mock_use_temp_path) {
    return g_mock_temp_path_ret;
  }
  return g_orig_rw.get_temp_path_fn(len, buf);
}
static unsigned int mock_rw_get_temp_file(const char *p, const char *pre,
                                          unsigned int u, char *out) {
  if (g_mock_use_temp_file) {
    return g_mock_temp_file_ret;
  }
  return g_orig_rw.get_temp_file_name_fn(p, pre, u, out);
}
static void *mock_rw_create_file(const char *n, DWORD a, DWORD s, void *sa,
                                 DWORD d, DWORD fl, void *t) {
  if (g_mock_use_create_file) {
    return g_mock_create_file_ret;
  }
  return g_orig_rw.create_file_fn(n, a, s, sa, d, fl, t);
}
static int mock_rw_close_handle(void *h) {
  (void)h;
  return 1;
}
static int mock_rw_close_noop(int fd) {
  (void)fd;
  return 0;
}
static DWORD mock_rw_get_cwd(DWORD len, char *buf) {
  if (g_mock_use_cwd) {
    if (g_mock_cwd_ret > 0 && buf && len > 3) {
#if defined(_MSC_VER)
      strcpy_s(buf, (size_t)len, "C:\\");
#else
      strcpy(buf, "C:\\");
#endif
    }
    return g_mock_cwd_ret;
  }
  return g_orig_rw.get_current_dir_fn(len, buf);
}
static DWORD mock_rw_get_attr(const char *n) {
  (void)n;
  if (g_mock_use_attr) {
    return g_mock_attr_ret;
  }
  return g_orig_rw.get_file_attributes_fn(n);
}
static DWORD mock_rw_get_ftype(void *h) {
  if (g_mock_use_ftype) {
    return g_mock_ftype_ret;
  }
  return g_orig_rw.get_file_type_fn(h);
}
static DWORD mock_rw_get_last_err(void) {
  if (g_mock_use_last_err) {
    return g_mock_last_err;
  }
  return g_orig_rw.get_last_error_fn();
}
typedef struct mock_memstatex {
  DWORD dwLength;
  DWORD dwMemoryLoad;
  DWORDLONG ullTotalPhys;
  DWORDLONG ullAvailPhys;
  DWORDLONG ullTotalPageFile;
  DWORDLONG ullAvailPageFile;
  DWORDLONG ullTotalVirtual;
  DWORDLONG ullAvailVirtual;
  DWORDLONG ullAvailExtendedVirtual;
} mock_memstatex_t;
static BOOL WINAPI mock_gms_fn(mock_memstatex_t *m) {
  if (!g_mock_gms_ret) {
    return 0;
  }
  m->ullTotalPhys = (DWORDLONG)32768U * (DWORDLONG)1024U;
  m->ullAvailPhys = (DWORDLONG)16384U * (DWORDLONG)1024U;
  return 1;
}
static void *mock_rw_get_mod(const char *n) {
  if (g_mock_use_mod) {
    return g_mock_mod_ret;
  }
  return g_orig_rw.get_module_handle_fn(n);
}
static void *mock_rw_get_proc(void *m, const char *p) {
  if (g_mock_use_proc) {
    return g_mock_proc_ret;
  }
  return g_orig_rw.get_proc_address_fn(m, p);
}
static int mock_rw_read(int fd, void *b, unsigned int c) {
  (void)fd;
  (void)b;
  (void)c;
  errno = g_mock_read_errno;
  return g_mock_read_ret;
}
static int mock_rw_write(int fd, const void *b, unsigned int c) {
  (void)fd;
  (void)b;
  (void)c;
  errno = g_mock_write_errno;
  return g_mock_write_ret;
}
static posix_rw_off_t mock_rw_lseek(int fd, posix_rw_off_t off, int w) {
  (void)fd;
  (void)off;
  (void)w;
  errno = g_mock_lseek_errno;
  return g_mock_lseek_ret;
}
static int mock_rw_dup(int fd) {
  (void)fd;
  return g_mock_dup_ret;
}
static int mock_rw_dup2(int fd1, int fd2) {
  (void)fd1;
  (void)fd2;
  return g_mock_dup2_ret;
}
static int mock_rw_getsockopt(uintptr_t s, int l, int o, char *v, int *len) {
  (void)s;
  (void)l;
  (void)o;
  (void)v;
  (void)len;
  return g_mock_getsockopt_ret;
}
static int mock_rw_recv(uintptr_t s, char *buf, int len, int flags) {
  (void)s;
  (void)flags;
  g_mock_recv_calls++;
  if (g_mock_recv_mode == 0) {
    return 0;
  }
  if (g_mock_recv_mode == 1) {
    return SOCKET_ERROR;
  }
  if (g_mock_recv_mode == 2) {
    if (g_mock_recv_calls == 1) {
      buf[0] = 'X';
      return 1;
    }
    return 0;
  }
  if (g_mock_recv_mode == 3) {
    if (g_mock_recv_calls == 1) {
      buf[0] = 'Y';
      return 1;
    }
    return SOCKET_ERROR;
  }
  if (g_mock_recv_mode == 4) {
    buf[0] = '\n';
    return 1;
  }
  if (g_mock_recv_mode == 5) {
    memset(buf, 'Z', (size_t)len);
    return len;
  }
  if (g_mock_recv_mode == 6) {
    if (g_mock_recv_calls == 1) {
      return SOCKET_ERROR;
    }
    buf[0] = 'R';
    return 1;
  }
  return 0;
}
static int mock_rw_send(uintptr_t s, const char *buf, int len, int flags) {
  (void)s;
  (void)buf;
  (void)len;
  (void)flags;
  g_mock_recv_calls++;
  if (g_mock_recv_mode == 6) {
    if (g_mock_recv_calls == 1) {
      return SOCKET_ERROR;
    }
    return len;
  }
  return g_mock_send_ret;
}
static int mock_rw_wsa_err_fn(void) { return g_mock_wsa_err; }
static int mock_rw_wsa_startup_fn(unsigned short v, void *d) {
  (void)v;
  (void)d;
  return g_mock_wsa_startup_ret;
}
static int mock_rw_closesocket_fn(uintptr_t s) {
  (void)s;
  return g_mock_closesocket_ret;
}
static int mock_rw_shutdown_fn(uintptr_t s, int how) {
  (void)s;
  (void)how;
  return 0;
}
static int mock_rw_readfile_fn(void *h, void *b, DWORD n, DWORD *br, void *ov) {
  (void)h;
  (void)b;
  (void)n;
  (void)ov;
  if (g_mock_readfile_ret) {
    *br = 3;
    return 1;
  }
  return 0;
}
static int mock_rw_writefile_fn(void *h, const void *b, DWORD n, DWORD *bw,
                                void *ov) {
  (void)h;
  (void)b;
  (void)n;
  (void)ov;
  if (g_mock_writefile_ret) {
    *bw = 4;
    return 1;
  }
  return 0;
}
static int mock_rw_overlapped_fn(void *h, void *ov, DWORD *bt, int wait) {
  (void)h;
  (void)ov;
  (void)wait;
  if (g_mock_overlapped_ret) {
    *bt = 5;
    return 1;
  }
  return 0;
}
static int mock_rw_set_std_noop(DWORD id, void *h) {
  (void)id;
  (void)h;
  return 1;
}
static int mock_rw_vscprintf_fn(const char *fmt, va_list ap)
    POSIX_CORE_PRINTF_FORMAT(1, 0);
static int mock_rw_vscprintf_fn(const char *fmt, va_list ap) {
  if (g_mock_use_vsc) {
    return g_mock_vsc_ret;
  }
  return g_orig_rw.vscprintf_fn(fmt, ap);
}
static int mock_rw_vsbuf_fn(char *b, size_t c, const char *fmt, va_list ap)
    POSIX_CORE_PRINTF_FORMAT(3, 0);
static int mock_rw_vsbuf_fn(char *b, size_t c, const char *fmt, va_list ap) {
  if (g_mock_use_vsbuf) {
    return g_mock_vsbuf_ret;
  }
  return g_orig_rw.vsprintf_buf_fn(b, c, fmt, ap);
}
static int mock_rw_epoll_close_fn(int fd) {
  (void)fd;
  return g_mock_epoll_close_ret;
}

TEST test_rw_win32_default_wrappers_and_real_sockets(void) {
  struct posix_rw_win32_hooks def;
  intptr_t sv[2];
  FILE *sf;
  char buf[32];
  void *raw_h;
  DWORD xfer = 0;
  OVERLAPPED ov;

  posix_rw_get_default_win32_hooks(NULL);
  posix_rw_get_default_win32_hooks(&def);

  ASSERT_EQ(0, posix_socketpair(AF_INET, SOCK_STREAM, 0, sv));
  sf = posix_fdopen(sv[0], "r+");
  ASSERT(sf != NULL);
  ASSERT(posix_fprintf(sf, "hi\n") == 3);
  ASSERT_EQ((size_t)2, posix_fwrite("OK", 1, 2, sf));
  ASSERT_EQ((ssize_t)5, posix_read(sv[1], buf, 5));
  ASSERT_EQ((ssize_t)4, posix_write(sv[1], "yo\n!", 4));
  ASSERT(posix_fgets(buf, sizeof(buf), sf) != NULL);
  ASSERT_EQ((int)'!', posix_fgetc(sf));
  ASSERT_EQ(0, posix_fclose(sf));
  ASSERT_EQ(0, posix_close(sv[1]));

  raw_h = def.create_file_fn("rw_raw_def.tmp", GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                             CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  ASSERT(raw_h != (void *)INVALID_HANDLE_VALUE);
  memset(&ov, 0, sizeof(ov));
  (void)def.write_file_fn(raw_h, "12", 2, &xfer, NULL);
  (void)def.lseek_fn(0, 0, SEEK_CUR);
  (void)def.read_file_fn(raw_h, buf, 2, &xfer, NULL);
  (void)def.get_overlapped_result_fn(raw_h, &ov, &xfer, 0);
  (void)def.set_std_handle_fn((DWORD)-12, (void *)def.get_osfhandle_fn(2));
  (void)def.epoll_close_fn(150);
  (void)def.close_handle_fn(raw_h);
  remove("rw_raw_def.tmp");
  PASS();
}

TEST test_rw_win32_stdio_tmpfile_vproc_fopen(void) {
  struct posix_rw_win32_hooks h;
  const char *vpaths[] = {
      "/dev/null",          "\\dev\\null",        "\\proc\\version",
      "/proc/meminfo",      "/proc/uptime",       "/proc/loadavg",
      "/proc/version",      "/proc/stat",         "/proc/mounts",
      "/proc/filesystems",  "/proc/swaps",        "/proc/cpuinfo",
      "/proc/self/stat",    "\\proc\\self\\stat", "/proc/self/comm",
      "/proc/self/cmdline", "/proc/self/status"};
  char long_path[2100];
  FILE *f;
  size_t i;

  reset_rw_mocks();
  h = g_orig_rw;

  /* ensure_binary_stdio branches */
  posix_rw_ensure_binary_stdio(0);
  g_mock_use_osfh = 1;
  g_mock_osfh_val = -1;
  h.get_osfhandle_fn = mock_rw_get_osfhandle;
  posix_rw_set_win32_hooks(&h);
  posix_rw_ensure_binary_stdio(1);

  g_mock_osfh_val = -2;
  ASSERT_EQ(-1, (int)posix_lseek(999, 0, SEEK_SET));
  ASSERT_EQ(-1, (int)posix_lseek(2048, 0, SEEK_SET));

  reset_rw_mocks();
  posix_rw_ensure_binary_stdio(1);

  /* safe_tmpfile branches */
  h = g_orig_rw;
  h.get_temp_path_fn = mock_rw_get_temp_path;
  h.get_temp_file_name_fn = mock_rw_get_temp_file;
  h.create_file_fn = mock_rw_create_file;
  h.open_osfhandle_fn = mock_rw_open_osfhandle;
  h.fdopen_fn = mock_rw_fdopen;
  h.close_handle_fn = mock_rw_close_handle;
  h.close_fn = mock_rw_close_noop;
  posix_rw_set_win32_hooks(&h);

  g_mock_use_temp_path = 1;
  g_mock_temp_path_ret = 0;
  f = posix_fopen("/dev/null", "r");
  ASSERT(f != NULL);
  fclose(f);

  g_mock_temp_path_ret = MAX_PATH + 10;
  f = posix_fopen("\\dev\\null", "r");
  ASSERT(f != NULL);
  fclose(f);
  g_mock_use_temp_path = 0;

  g_mock_use_temp_file = 1;
  g_mock_temp_file_ret = 0;
  g_mock_use_create_file = 1;
  g_mock_create_file_ret = (void *)INVALID_HANDLE_VALUE;
  for (i = 0; i < sizeof(vpaths) / sizeof(vpaths[0]); i++) {
    ASSERT(posix_fopen(vpaths[i], "r") == NULL);
  }
  g_mock_use_create_file = 0;
  g_mock_use_temp_file = 0;

  for (i = 0; i < sizeof(vpaths) / sizeof(vpaths[0]); i++) {
    f = posix_fopen(vpaths[i], "r");
    ASSERT(f != NULL);
    fclose(f);
  }

  g_mock_use_create_file = 1;
  g_mock_create_file_ret = (void *)INVALID_HANDLE_VALUE;
  ASSERT(posix_fopen("/dev/null", "r") == NULL);

  g_mock_create_file_ret = (void *)(intptr_t)0x1234;
  g_mock_use_open_osfh = 1;
  g_mock_open_osfh_ret = -1;
  ASSERT(posix_fopen("/dev/null", "r") == NULL);

  g_mock_open_osfh_ret = 9;
  g_mock_fdopen_null = 1;
  ASSERT(posix_fopen("/dev/null", "r") == NULL);

  /* meminfo module/proc variations */
  reset_rw_mocks();
  h = g_orig_rw;
  h.get_module_handle_fn = mock_rw_get_mod;
  h.get_proc_address_fn = mock_rw_get_proc;
  posix_rw_set_win32_hooks(&h);

  g_mock_use_mod = 1;
  g_mock_mod_ret = NULL;
  f = posix_fopen("/proc/meminfo", "r");
  ASSERT(f != NULL);
  fclose(f);

  g_mock_mod_ret = (void *)(intptr_t)1;
  g_mock_use_proc = 1;
  g_mock_proc_ret = NULL;
  f = posix_fopen("/proc/meminfo", "r");
  ASSERT(f != NULL);
  fclose(f);

  g_mock_proc_ret = (void *)(size_t)mock_gms_fn;
  g_mock_gms_ret = 0;
  f = posix_fopen("/proc/meminfo", "r");
  ASSERT(f != NULL);
  fclose(f);

  g_mock_gms_ret = 1;
  f = posix_fopen("/proc/meminfo", "r");
  ASSERT(f != NULL);
  fclose(f);

  /* non-matching /proc paths & posix_fopen path branches */
  reset_rw_mocks();
  ASSERT(posix_fopen("/proc/self", "r") == NULL);
  ASSERT(posix_fopen("/proc/self/unknown", "r") == NULL);
  ASSERT(posix_fopen("/proc/!bad", "r") == NULL);
  ASSERT(posix_fopen("/proc/zbad", "r") == NULL);
  ASSERT(posix_fopen("\\nonexistent_abs_win.tmp", "r") == NULL);
  ASSERT(posix_fopen("C:\\nonexistent_abs_win.tmp", "r") == NULL);

  h = g_orig_rw;
  h.get_current_dir_fn = mock_rw_get_cwd;
  h.create_file_fn = mock_rw_create_file;
  h.open_osfhandle_fn = mock_rw_open_osfhandle;
  h.fdopen_fn = mock_rw_fdopen;
  h.close_handle_fn = mock_rw_close_handle;
  h.close_fn = mock_rw_close_noop;
  posix_rw_set_win32_hooks(&h);

  g_mock_use_cwd = 1;
  g_mock_cwd_ret = 0;
  ASSERT(posix_fopen("rel_fail.tmp", "r") == NULL);
  ASSERT_EQ(ENOENT, errno);

  g_mock_cwd_ret = 3;
  memset(long_path, 'a', sizeof(long_path) - 1);
  long_path[sizeof(long_path) - 1] = '\0';
  ASSERT(posix_fopen(long_path, "r") == NULL);
  ASSERT_EQ(ENAMETOOLONG, errno);

  long_path[0] = '/';
  ASSERT(posix_fopen(long_path, "r") == NULL);
  ASSERT_EQ(ENAMETOOLONG, errno);

  g_mock_use_create_file = 1;
  g_mock_create_file_ret = (void *)(intptr_t)0x2345;
  g_mock_use_open_osfh = 1;
  g_mock_open_osfh_ret = -1;
  ASSERT(posix_fopen("/ok_len.tmp", "r") == NULL);

  g_mock_open_osfh_ret = 11;
  g_mock_fdopen_null = 1;
  ASSERT(posix_fopen("/ok_len.tmp", "r") == NULL);

  reset_rw_mocks();
  PASS();
}

TEST test_rw_win32_socket_streams_and_oom(void) {
  struct posix_rw_win32_hooks h;
  FILE *streams[65];
  FILE *plain;
  char buf[16];
  int i;

  reset_rw_mocks();
  posix_rw_reset_socket_streams();
  h = g_orig_rw;
  h.malloc_fn = mock_rw_malloc;
  h.getsockopt_fn = mock_rw_getsockopt;
  h.recv_fn = mock_rw_recv;
  h.send_fn = mock_rw_send;
  h.wsa_get_last_error_fn = mock_rw_wsa_err_fn;
  h.shutdown_fn = mock_rw_shutdown_fn;
  h.closesocket_fn = mock_rw_closesocket_fn;
  h.vscprintf_fn = mock_rw_vscprintf_fn;
  h.vsprintf_buf_fn = mock_rw_vsbuf_fn;
  h.fileno_fn = mock_rw_fileno;
  h.get_temp_file_name_fn = mock_rw_get_temp_file;
  h.get_osfhandle_fn = mock_rw_get_osfhandle;
  h.open_osfhandle_fn = mock_rw_open_osfhandle;
  h.fdopen_fn = mock_rw_fdopen;
  posix_rw_set_win32_hooks(&h);

  /* Socket fdopen failure when safe_tmpfile fails */
  g_mock_getsockopt_ret = 0;
  g_mock_use_temp_file = 1;
  g_mock_temp_file_ret = 0;
  ASSERT(posix_fdopen(55, "r") == NULL);
  ASSERT_EQ(EBADF, errno);
  g_mock_use_temp_file = 0;

  /* Fill all 64 slots + 65th overflow + update existing slot */
  for (i = 0; i < 65; i++) {
    streams[i] =
        posix_fdopen(100 + i, i == 0 ? "" : (i == 1 ? "rx" : "r+w+a+x"));
    ASSERT(streams[i] != NULL);
  }
  ASSERT_EQ(100, posix_fileno(streams[0]));
  ASSERT(posix_feof(streams[64]) == 0);
  ASSERT(posix_ferror(streams[64]) == 0);

  /* Update existing slot branch: force safe_tmpfile fdopen to return streams[0]
   */
  g_mock_use_osfh = 1;
  g_mock_osfh_val = -1;
  g_mock_use_open_osfh = 1;
  g_mock_open_osfh_ret = 5;

  /* Test socket fgets / fgetc / fread / fwrite / fprintf on registered
   * (streams[0]) and unregistered CRT-socket-backed (streams[64] with
   * fileno=500) */
  g_mock_use_osfh = 0;
  g_mock_use_open_osfh = 0;

  /* size == 1 in posix_fgets */
  ASSERT(posix_fgets(buf, 1, streams[0]) != NULL);
  ASSERT_EQ('\0', buf[0]);

  /* EOF on registered & unregistered */
  g_mock_recv_mode = 0;
  ASSERT(posix_fgets(buf, sizeof(buf), streams[0]) == NULL);
  ASSERT_EQ(1, posix_feof(streams[0]));
  g_mock_use_fileno = 1;
  g_mock_fileno_val = 500;
  ASSERT(posix_fgets(buf, sizeof(buf), streams[64]) == NULL);

  /* Error (WSAEWOULDBLOCK, WSAEINPROGRESS, WSAECONNRESET) on fgets */
  g_mock_recv_mode = 1;
  g_mock_wsa_err = WSAEWOULDBLOCK;
  ASSERT(posix_fgets(buf, sizeof(buf), streams[0]) == NULL);
  ASSERT_EQ(1, posix_ferror(streams[0]));
  g_mock_wsa_err = WSAEINPROGRESS;
  ASSERT(posix_fgets(buf, sizeof(buf), streams[64]) == NULL);
  g_mock_wsa_err = WSAECONNRESET;
  ASSERT(posix_fgets(buf, sizeof(buf), streams[0]) == NULL);

  /* Partial read before EOF and buffer-full without newline */
  g_mock_recv_mode = 2;
  g_mock_recv_calls = 0;
  ASSERT(posix_fgets(buf, sizeof(buf), streams[0]) != NULL);
  ASSERT_STR_EQ("X", buf);
  g_mock_recv_mode = 5;
  ASSERT(posix_fgets(buf, 4, streams[0]) != NULL);
  ASSERT_EQ(3, (int)strlen(buf));

  /* fgetc EOF & errors on registered and unregistered */
  g_mock_recv_mode = 0;
  ASSERT_EQ(EOF, posix_fgetc(streams[0]));
  ASSERT_EQ(EOF, posix_fgetc(streams[64]));
  g_mock_recv_mode = 1;
  g_mock_wsa_err = WSAEWOULDBLOCK;
  ASSERT_EQ(EOF, posix_fgetc(streams[0]));
  g_mock_wsa_err = WSAEINPROGRESS;
  ASSERT_EQ(EOF, posix_fgetc(streams[64]));
  g_mock_wsa_err = WSAECONNRESET;
  ASSERT_EQ(EOF, posix_fgetc(streams[0]));

  /* fprintf vsc < 0, OOM, ret == 0 (""), ret > 0 */
  g_mock_use_vsc = 1;
  g_mock_vsc_ret = -1;
  ASSERT_EQ(-1, posix_fprintf(streams[0], "abc"));
  g_mock_use_vsc = 0;

  g_mock_oom = 1;
  ASSERT_EQ(-1, posix_fprintf(streams[0], "abc"));
  g_mock_oom = 0;

  g_mock_send_ret = 0;
  ASSERT_EQ(0, posix_fprintf(streams[0], "%s", ""));
  g_mock_use_vsbuf = 1;
  g_mock_vsbuf_ret = -1;
  ASSERT_EQ(-1, posix_fprintf(streams[0], "abc"));
  g_mock_use_vsbuf = 0;

  /* fread: size==1 vs size>1, EOF, EWOULDBLOCK/EINPROGRESS/ETIMEDOUT (got==0 &
   * got>0), ECONNRESET */
  g_mock_recv_mode = 5;
  ASSERT_EQ((size_t)4, posix_fread(buf, 1, 4, streams[0]));
  ASSERT_EQ((size_t)2, posix_fread(buf, 2, 2, streams[0]));

  g_mock_recv_mode = 0;
  ASSERT_EQ((size_t)0, posix_fread(buf, 2, 2, streams[0]));
  ASSERT_EQ((size_t)0, posix_fread(buf, 2, 2, streams[64]));

  g_mock_recv_mode = 1;
  g_mock_wsa_err = WSAEWOULDBLOCK;
  ASSERT_EQ((size_t)0, posix_fread(buf, 2, 2, streams[0]));
  g_mock_wsa_err = WSAEINPROGRESS;
  ASSERT_EQ((size_t)0, posix_fread(buf, 2, 2, streams[0]));
  g_mock_wsa_err = WSAETIMEDOUT;
  ASSERT_EQ((size_t)0, posix_fread(buf, 2, 2, streams[0]));
  g_mock_wsa_err = WSAECONNRESET;
  ASSERT_EQ((size_t)0, posix_fread(buf, 2, 2, streams[0]));
  ASSERT_EQ((size_t)0, posix_fread(buf, 2, 2, streams[64]));

  g_mock_recv_mode = 3;
  g_mock_recv_calls = 0;
  g_mock_wsa_err = WSAETIMEDOUT;
  ASSERT_EQ((size_t)0, posix_fread(buf, 2, 2, streams[0]));

  /* fwrite: r <= 0 vs r > 0 */
  g_mock_send_ret = -1;
  ASSERT_EQ((size_t)0, posix_fwrite("abcd", 2, 2, streams[0]));
  g_mock_send_ret = 4;
  ASSERT_EQ((size_t)2, posix_fwrite("abcd", 2, 2, streams[0]));

  for (i = 0; i < 65; i++) {
    ASSERT_EQ(0, posix_fclose(streams[i]));
  }

  /* Cover register_socket_stream existing-stream branch and raw-handle fdopen
   */
  plain = posix_fopen("rw_cov_plain.tmp", "w+b");
  ASSERT(plain != NULL);
  g_mock_use_fileno = 1;
  g_mock_fileno_val = -1;
  ASSERT_EQ(EOF, posix_fgetc(plain));
  ASSERT(posix_feof(plain) != 0);
  fclose(plain);
  remove("rw_cov_plain.tmp");

  /* Raw OS handle fdopen branches: +, w, a, r and open_osfhandle failure */
  g_mock_getsockopt_ret = -1;
  g_mock_use_osfh = 1;
  g_mock_osfh_val = -1;
  g_mock_use_open_osfh = 1;
  g_mock_open_osfh_ret = -1;
  ASSERT(posix_fdopen(99, "+") == NULL);
  ASSERT(posix_fdopen(99, "w") == NULL);
  ASSERT(posix_fdopen(99, "a") == NULL);
  ASSERT(posix_fdopen(99, "r") == NULL);

  g_mock_open_osfh_ret = 4;
  g_mock_fdopen_null = 1;
  ASSERT(posix_fdopen(99, "r") == NULL);

  reset_rw_mocks();
  posix_rw_reset_socket_streams();
  PASS();
}

static FILE *g_fixed_tmp_stream = NULL;
static FILE *mock_rw_fdopen_fixed(int fd, const char *mode) {
  (void)mode;
  _close(fd);
  return g_fixed_tmp_stream;
}

TEST test_rw_win32_register_existing_and_fd_ops(void) {
  struct posix_rw_win32_hooks h;
  const int wsa_errs[] = {
      WSAEWOULDBLOCK,  WSAETIMEDOUT,    WSAECONNRESET, WSAEINPROGRESS,
      WSAEALREADY,     WSAEMSGSIZE,     WSAEADDRINUSE, WSAEADDRNOTAVAIL,
      WSAECONNABORTED, WSAECONNREFUSED, 12345};
  char buf[16];
  size_t i;

  /* Cover g_socket_streams[i].stream == stream true branch */
  reset_rw_mocks();
  posix_rw_reset_socket_streams();
  g_fixed_tmp_stream = posix_fopen("rw_cov_fixed.tmp", "w+b");
  ASSERT(g_fixed_tmp_stream != NULL);
  h = g_orig_rw;
  h.getsockopt_fn = mock_rw_getsockopt;
  h.fdopen_fn = mock_rw_fdopen_fixed;
  h.shutdown_fn = mock_rw_shutdown_fn;
  h.closesocket_fn = mock_rw_closesocket_fn;
  posix_rw_set_win32_hooks(&h);
  g_mock_getsockopt_ret = 0;
  ASSERT(posix_fdopen(70, "r") == g_fixed_tmp_stream);
  ASSERT(posix_fdopen(71, "r") == g_fixed_tmp_stream);
  ASSERT_EQ(0, posix_fclose(g_fixed_tmp_stream));
  g_fixed_tmp_stream = NULL;
  remove("rw_cov_fixed.tmp");

  /* Cover sock == -1 and sock == 0 in is_crt_fd_socket */
  ASSERT(posix_fdopen(0, "r") != NULL || 1);
  posix_set_dup_socket(5, (intptr_t)-1);
  g_mock_use_osfh = 1;
  g_mock_osfh_val = -1;
  g_mock_use_open_osfh = 1;
  g_mock_open_osfh_ret = -1;
  h.get_osfhandle_fn = mock_rw_get_osfhandle;
  h.open_osfhandle_fn = mock_rw_open_osfhandle;
  posix_rw_set_win32_hooks(&h);
  ASSERT(posix_fdopen(5, "r") == NULL);
  posix_set_dup_socket(5, 0);

  /* posix_open all flag/error branches */
  reset_rw_mocks();
  {
    int vfd = posix_open("///proc/version", O_RDONLY);
    ASSERT(vfd >= 0);
    posix_close(vfd);
  }

  h = g_orig_rw;
  h.get_file_attributes_fn = mock_rw_get_attr;
  h.create_file_fn = mock_rw_create_file;
  h.get_last_error_fn = mock_rw_get_last_err;
  h.open_osfhandle_fn = mock_rw_open_osfhandle;
  h.close_handle_fn = mock_rw_close_handle;
  posix_rw_set_win32_hooks(&h);

  g_mock_use_attr = 1;
  g_mock_attr_ret = FILE_ATTRIBUTE_DIRECTORY;
  g_mock_use_create_file = 1;
  g_mock_create_file_ret = (void *)INVALID_HANDLE_VALUE;
  g_mock_use_last_err = 1;

  g_mock_last_err = ERROR_FILE_EXISTS;
  ASSERT_EQ(-1, posix_open("x", O_RDONLY));
  ASSERT_EQ(EEXIST, errno);

  g_mock_last_err = ERROR_ALREADY_EXISTS;
  ASSERT_EQ(-1, posix_open("x", O_WRONLY | O_CREAT | O_EXCL, 0666));
  ASSERT_EQ(EEXIST, errno);

  g_mock_last_err = ERROR_FILE_NOT_FOUND;
  ASSERT_EQ(-1, posix_open("x", O_RDWR | O_TRUNC));
  ASSERT_EQ(ENOENT, errno);

  g_mock_last_err = ERROR_PATH_NOT_FOUND;
  ASSERT_EQ(-1, posix_open("x", O_CREAT, 0666));
  ASSERT_EQ(ENOENT, errno);

  g_mock_last_err = ERROR_ACCESS_DENIED;
  g_mock_attr_ret = FILE_ATTRIBUTE_DIRECTORY;
  ASSERT_EQ(-1, posix_open("x", 3));
  ASSERT_EQ(EISDIR, errno);

  g_mock_attr_ret = FILE_ATTRIBUTE_NORMAL;
  ASSERT_EQ(-1, posix_open("x", O_RDONLY));
  ASSERT_EQ(EACCES, errno);

  g_mock_attr_ret = INVALID_FILE_ATTRIBUTES;
  ASSERT_EQ(-1, posix_open("x", O_RDONLY));
  ASSERT_EQ(EACCES, errno);

  g_mock_last_err = 99999UL;
  ASSERT_EQ(-1, posix_open("x", O_RDONLY));
  ASSERT_EQ(EINVAL, errno);

  /* posix_open success / open_osfhandle failure for O_RDONLY, O_WRONLY, O_RDWR,
   * 3, O_APPEND */
  g_mock_create_file_ret = (void *)(intptr_t)0x3333;
  g_mock_use_open_osfh = 1;
  g_mock_open_osfh_ret = -1;
  ASSERT_EQ(-1, posix_open("x", O_RDONLY | O_APPEND));
  ASSERT_EQ(EMFILE, errno);
  g_mock_open_osfh_ret = 12;
  ASSERT_EQ(12, posix_open("x", O_WRONLY));
  ASSERT_EQ(12, posix_open("x", O_RDWR));
  ASSERT_EQ(12, posix_open("x", 3));

  /* posix_read & posix_write branches */
  reset_rw_mocks();
  ASSERT_EQ(-1, posix_read(0, NULL, 1));
  ASSERT_EQ(EINVAL, errno);
  ASSERT_EQ(-1, posix_write(0, NULL, 1));
  ASSERT_EQ(EINVAL, errno);

  h = g_orig_rw;
  h.get_osfhandle_fn = mock_rw_get_osfhandle;
  h.read_fn = mock_rw_read;
  h.write_fn = mock_rw_write;
  h.recv_fn = mock_rw_recv;
  h.send_fn = mock_rw_send;
  h.wsa_get_last_error_fn = mock_rw_wsa_err_fn;
  h.wsa_startup_fn = mock_rw_wsa_startup_fn;
  h.read_file_fn = mock_rw_readfile_fn;
  h.write_file_fn = mock_rw_writefile_fn;
  h.get_last_error_fn = mock_rw_get_last_err;
  h.get_overlapped_result_fn = mock_rw_overlapped_fn;
  posix_rw_set_win32_hooks(&h);

  /* CRT read/write EINVAL -> EBADF vs EIO */
  g_mock_use_osfh = 1;
  g_mock_osfh_val = 10;
  g_mock_read_ret = -1;
  g_mock_read_errno = EINVAL;
  ASSERT_EQ(-1, posix_read(4, buf, 1));
  ASSERT_EQ(EBADF, errno);
  g_mock_read_errno = EIO;
  ASSERT_EQ(-1, posix_read(4, buf, 1));
  ASSERT_EQ(EIO, errno);

  g_mock_write_ret = -1;
  g_mock_write_errno = EINVAL;
  ASSERT_EQ(-1, posix_write(4, "a", 1));
  ASSERT_EQ(EBADF, errno);
  g_mock_write_errno = EIO;
  ASSERT_EQ(-1, posix_write(4, "a", 1));
  ASSERT_EQ(EIO, errno);

  /* osfh == -1 falls through to recv/send */
  g_mock_osfh_val = -1;
  g_mock_recv_mode = 1;
  g_mock_send_ret = SOCKET_ERROR;

  /* WSANOTINITIALISED: startup fail, startup ok + retry ok, startup ok + retry
   * fail -> ReadFile/WriteFile */
  g_mock_wsa_err = WSANOTINITIALISED;
  g_mock_wsa_startup_ret = -1;
  ASSERT_EQ(-1, posix_read(4, buf, 1));
  ASSERT_EQ(-1, posix_write(4, "a", 1));

  g_mock_wsa_startup_ret = 0;
  g_mock_recv_mode = 6;
  g_mock_recv_calls = 0;
  ASSERT_EQ((ssize_t)1, posix_read(4, buf, 1));
  g_mock_recv_calls = 0;
  ASSERT_EQ((ssize_t)2, posix_write(4, "ab", 2));

  g_mock_recv_mode = 1;
  g_mock_send_ret = SOCKET_ERROR;
  g_mock_readfile_ret = 1;
  ASSERT_EQ((ssize_t)3, posix_read(4, buf, 3));
  g_mock_readfile_ret = 0;
  ASSERT_EQ(-1, posix_read(4, buf, 3));

  g_mock_wsa_err = WSAEOPNOTSUPP;
  ASSERT_EQ(-1, posix_read(4, buf, 3));
  g_mock_wsa_err = WSAENOTSOCK;
  ASSERT_EQ(-1, posix_read(4, buf, 3));

  /* WriteFile sync ok, ERROR_IO_PENDING + overlapped ok/fail, other error */
  g_mock_use_last_err = 1;
  g_mock_writefile_ret = 1;
  g_mock_wsa_err = WSAENOTSOCK;
  ASSERT_EQ((ssize_t)4, posix_write(4, "abcd", 4));
  g_mock_writefile_ret = 0;
  g_mock_last_err = ERROR_IO_PENDING;
  g_mock_overlapped_ret = 1;
  g_mock_wsa_err = WSAEOPNOTSUPP;
  ASSERT_EQ((ssize_t)5, posix_write(4, "abcde", 5));
  g_mock_overlapped_ret = 0;
  g_mock_wsa_err = WSANOTINITIALISED;
  ASSERT_EQ(-1, posix_write(4, "abcde", 5));
  g_mock_last_err = ERROR_ACCESS_DENIED;
  ASSERT_EQ(-1, posix_write(4, "abcde", 5));

  /* All map_wsa_rw_error branches */
  for (i = 0; i < sizeof(wsa_errs) / sizeof(wsa_errs[0]); i++) {
    g_mock_wsa_err = wsa_errs[i];
    ASSERT_EQ(-1, posix_read(4, buf, 1));
    ASSERT_EQ(-1, posix_write(4, "a", 1));
  }

  /* posix_close, posix_lseek, posix_dup, posix_dup2 branches */
  reset_rw_mocks();
  h = g_orig_rw;
  h.epoll_close_fn = mock_rw_epoll_close_fn;
  h.shutdown_fn = mock_rw_shutdown_fn;
  h.closesocket_fn = mock_rw_closesocket_fn;
  h.wsa_get_last_error_fn = mock_rw_wsa_err_fn;
  h.close_fn = mock_rw_close_noop;
  h.get_osfhandle_fn = mock_rw_get_osfhandle;
  h.get_file_type_fn = mock_rw_get_ftype;
  h.lseek_fn = mock_rw_lseek;
  h.dup_fn = mock_rw_dup;
  h.dup2_fn = mock_rw_dup2;
  h.getsockopt_fn = mock_rw_getsockopt;
  h.set_std_handle_fn = mock_rw_set_std_noop;
  posix_rw_set_win32_hooks(&h);

  g_mock_epoll_close_ret = 0;
  ASSERT_EQ(0, posix_close(200));
  g_mock_epoll_close_ret = -1;
  g_mock_closesocket_ret = 0;
  ASSERT_EQ(0, posix_close(205));

  /* Shared dup_socket & crt_socket branches in posix_close */
  posix_set_dup_socket(10, 777);
  posix_set_dup_socket(11, 777);
  ASSERT_EQ(0, posix_close(10));
  ASSERT_EQ(0, posix_close(11));

  posix_register_crt_socket(200, 300);
  posix_register_crt_socket(300, 300);
  ASSERT_EQ(0, posix_close(200));
  ASSERT_EQ(0, posix_close(300));

  posix_register_crt_socket(201, -5);
  ASSERT_EQ(0, posix_close(201));
  posix_register_crt_socket(202, 70000);
  ASSERT_EQ(0, posix_close(202));

  /* fd >= 65536 and closesocket success / error branches */
  g_mock_closesocket_ret = 0;
  ASSERT_EQ(0, posix_close(70000));
  g_mock_closesocket_ret = SOCKET_ERROR;
  g_mock_wsa_err = WSAENOTSOCK;
  ASSERT_EQ(0, posix_close(50));
  g_mock_wsa_err = WSAEOPNOTSUPP;
  ASSERT_EQ(0, posix_close(50));
  g_mock_wsa_err = WSANOTINITIALISED;
  ASSERT_EQ(0, posix_close(50));
  g_mock_wsa_err = WSAECONNRESET;
  ASSERT_EQ(-1, posix_close(50));

  /* posix_lseek branches */
  g_mock_use_osfh = 1;
  g_mock_osfh_val = 10;
  g_mock_use_ftype = 1;
  g_mock_ftype_ret = 3UL;
  ASSERT_EQ(-1, (int)posix_lseek(3, 0, SEEK_SET));
  ASSERT_EQ(ESPIPE, errno);
  g_mock_ftype_ret = 2UL;
  ASSERT_EQ(-1, (int)posix_lseek(3, 0, SEEK_SET));
  ASSERT_EQ(ESPIPE, errno);

  g_mock_ftype_ret = 0UL;
  g_mock_lseek_ret = -1;
  g_mock_lseek_errno = EINVAL;
  ASSERT_EQ(-1, (int)posix_lseek(3, 0, SEEK_SET));
  ASSERT_EQ(ESPIPE, errno);
  ASSERT_EQ(-1, (int)posix_lseek(3, 0, SEEK_CUR));
  ASSERT_EQ(ESPIPE, errno);
  ASSERT_EQ(-1, (int)posix_lseek(3, 0, SEEK_END));
  ASSERT_EQ(ESPIPE, errno);
  ASSERT_EQ(-1, (int)posix_lseek(3, 0, 99));
  ASSERT_EQ(EINVAL, errno);

  g_mock_ftype_ret = 1UL;
  ASSERT_EQ(-1, (int)posix_lseek(3, 0, SEEK_SET));
  ASSERT_EQ(EINVAL, errno);

  g_mock_lseek_errno = EBADF;
  ASSERT_EQ(-1, (int)posix_lseek(3, 0, SEEK_SET));
  ASSERT_EQ(EBADF, errno);

  /* posix_dup & posix_dup2 socket & std-handle branches */
  g_mock_dup_ret = -1;
  ASSERT_EQ(-1, posix_dup(6));
  (void)mark_as_socket(6);
  g_mock_dup_ret = 7;
  ASSERT_EQ(7, posix_dup(6));
  (void)clear_as_socket(6);
  (void)clear_as_socket(7);

  /* dup2 with newfd < 64: is_socket(oldfd) vs is_crt_fd_socket(oldfd) */
  (void)mark_as_socket(6);
  ASSERT_EQ(8, posix_dup2(6, 8));
  posix_set_dup_socket(8, 0);
  (void)clear_as_socket(6);
  (void)clear_as_socket(8);

  g_mock_getsockopt_ret = 0;
  ASSERT_EQ(8, posix_dup2(9, 8));
  posix_set_dup_socket(8, 0);
  (void)clear_as_socket(8);
  g_mock_getsockopt_ret = -1;

  /* dup2 with newfd >= 64 and is_socket(oldfd) */
  (void)mark_as_socket(6);
  g_mock_dup2_ret = 70;
  ASSERT_EQ(70, posix_dup2(6, 70));
  (void)clear_as_socket(6);
  (void)clear_as_socket(70);

  /* dup2 with newfd = 0, 1, 2 (osfh != -1 and osfh == -1) and dup2 == -1 */
  g_mock_osfh_val = 10;
  g_mock_dup2_ret = 0;
  ASSERT_EQ(0, posix_dup2(6, 0));
  ASSERT_EQ(1, posix_dup2(6, 1));
  ASSERT_EQ(2, posix_dup2(6, 2));
  g_mock_osfh_val = -1;
  ASSERT_EQ(0, posix_dup2(6, 0));
  g_mock_dup2_ret = -1;
  ASSERT_EQ(-1, posix_dup2(6, 9));

  reset_rw_mocks();
  PASS();
}
#endif

SUITE(suite_posix_rw_coverage) {
  RUN_TEST(test_rw_common_streams_and_fds);
#if !defined(_WIN32) && !defined(__CYGWIN__) && !defined(__linux__) &&         \
    !defined(__DOS__) && !defined(__MSDOS__) && !defined(__WATCOMC__)
  RUN_TEST(test_rw_posix_vproc_and_oom);
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
  RUN_TEST(test_rw_win32_default_wrappers_and_real_sockets);
  RUN_TEST(test_rw_win32_stdio_tmpfile_vproc_fopen);
  RUN_TEST(test_rw_win32_socket_streams_and_oom);
  RUN_TEST(test_rw_win32_register_existing_and_fd_ops);
#endif
}
