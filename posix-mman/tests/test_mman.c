/* clang-format off */
#include "greatest.h"
#include "posix-mman.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_WIN32) || defined(_MSC_VER)
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#if defined(_MSC_VER)
#include <share.h>
#endif
#else
#include <unistd.h>
#endif
/* clang-format on */

#if defined(_WIN32) || defined(_WIN64)
typedef struct _TEST_MMAN_MBI {
  void *BaseAddress;
  void *AllocationBase;
  unsigned long AllocationProtect;
  size_t RegionSize;
  unsigned long State;
  unsigned long Protect;
  unsigned long Type;
} TEST_MMAN_MBI;

typedef struct _TEST_MMAN_SI {
  unsigned long dwOemId;
  unsigned long dwPageSize;
  void *lpMinimumApplicationAddress;
  void *lpMaximumApplicationAddress;
  size_t dwActiveProcessorMask;
  unsigned long dwNumberOfProcessors;
  unsigned long dwProcessorType;
  unsigned long dwAllocationGranularity;
  unsigned short wProcessorLevel;
  unsigned short wProcessorRevision;
} TEST_MMAN_SI;

__declspec(dllimport) void *__stdcall
CreateFileMappingA(void *hFile, void *attr, unsigned long prot,
                   unsigned long hi, unsigned long lo, const char *name);

extern void posix_mman_set_win32_hooks(
    void (*sysinfo_fn)(TEST_MMAN_SI *),
    size_t (*vquery_fn)(const void *, TEST_MMAN_MBI *, size_t),
    unsigned long (*temp_path_fn)(unsigned long, char *),
    unsigned long (*temp_file_fn)(const char *, const char *, unsigned int,
                                  char *),
    void *(*create_map_fn)(void *, void *, unsigned long, unsigned long,
                           unsigned long, const char *),
    void *(*map_view_fn)(void *, unsigned long, unsigned long, unsigned long,
                         size_t, void *),
    int (*vprotect_fn)(void *, size_t, unsigned long, unsigned long *),
    void *(*get_mod_fn)(const char *),
    void *(*get_proc_fn)(void *, const char *),
    int (*open_file_fn)(const char *, int));
extern void posix_mman_set_lock_hooks(int (*vlock_fn)(void *, size_t),
                                      int (*vunlock_fn)(void *, size_t),
                                      int (*flush_fn)(const void *, size_t),
                                      int (*unmap_fn)(const void *));
extern void posix_mman_reset_win32_hooks(void);

static int mock_lock_fail(void *addr, size_t len) {
  (void)addr;
  (void)len;
  return 0;
}
static int mock_flush_fail(const void *addr, size_t len) {
  (void)addr;
  (void)len;
  return 0;
}
static int mock_unmap_aligned_only(const void *addr) {
  return (((size_t)addr & 0xFFFFU) == 0) ? 1 : 0;
}

static int g_vq_mode = 0;
static int g_vq_step = 0;
static unsigned long g_tp_ret = 0;
static unsigned long g_tf_ret = 0;
static int g_map_mode = 0;
static int g_map_calls = 0;
static int g_vp_mode = 0;
static int g_vp_calls = 0;
static void *g_mod_ret = NULL;
static void *g_proc_ret = NULL;
static int g_open_ret = -1;

static void mock_sysinfo(TEST_MMAN_SI *si) {
  memset(si, 0, sizeof(*si));
  si->lpMaximumApplicationAddress = (void *)(size_t)0x80000;
}

static size_t mock_vquery(const void *addr, TEST_MMAN_MBI *mbi, size_t len) {
  (void)addr;
  (void)len;
  memset(mbi, 0, sizeof(*mbi));
  if (g_vq_mode == 0) {
    return 0;
  }
  if (g_vq_mode == 1) {
    /* Region covering NOACCESS, GUARD, normal, then wraparound next_addr <=
     * addr */
    g_vq_step++;
    if (g_vq_step == 1) {
      mbi->BaseAddress = (void *)(size_t)0x1000;
      mbi->RegionSize = 0x1000;
      mbi->State = 0x1000; /* MEM_COMMIT */
      mbi->Protect = 0x01; /* PAGE_NOACCESS */
      return sizeof(*mbi);
    }
    if (g_vq_step == 2) {
      mbi->BaseAddress = (void *)(size_t)0x2000;
      mbi->RegionSize = 0x1000;
      mbi->State = 0x1000;
      mbi->Protect = 0x100; /* PAGE_GUARD */
      return sizeof(*mbi);
    }
    if (g_vq_step == 3) {
      mbi->BaseAddress = (void *)(size_t)0x3000;
      mbi->RegionSize = 0x1000;
      mbi->State = 0x2000; /* not MEM_COMMIT */
      mbi->Protect = 0x04;
      return sizeof(*mbi);
    }
    /* Force next_addr <= addr wraparound break */
    mbi->BaseAddress = (void *)(size_t)0x4000;
    mbi->RegionSize = (size_t)0 - (size_t)0x4000;
    mbi->State = 0x2000;
    return sizeof(*mbi);
  }
  return 0;
}

static unsigned long mock_temp_path(unsigned long len, char *buf) {
  if (g_tp_ret && len > 4) {
    buf[0] = '.';
    buf[1] = (char)92;
    buf[2] = '\0';
  }
  return g_tp_ret;
}

static unsigned long mock_temp_file(const char *path, const char *pfx,
                                    unsigned int uniq, char *out) {
  (void)path;
  (void)pfx;
  (void)uniq;
  if (g_tf_ret) {
    out[0] = 't';
    out[1] = '\0';
  }
  return g_tf_ret;
}

static void *mock_create_map(void *hFile, void *attr, unsigned long prot,
                             unsigned long hi, unsigned long lo,
                             const char *name) {
  (void)hFile;
  (void)attr;
  (void)prot;
  (void)hi;
  (void)lo;
  (void)name;
  if (g_map_mode == 0) {
    return NULL;
  }
  return CreateFileMappingA((void *)(ptrdiff_t)-1, NULL, 0x04, 0, 4096, NULL);
}

static char g_fake_view[8192];

static void *mock_map_view(void *hMap, unsigned long acc, unsigned long hi,
                           unsigned long lo, size_t len, void *addr) {
  (void)hMap;
  (void)acc;
  (void)hi;
  (void)lo;
  (void)len;
  (void)addr;
  g_map_calls++;
  if (g_map_mode == 1) {
    return NULL; /* Both calls fail */
  }
  if (g_map_mode == 2) {
    return (g_map_calls == 1) ? NULL : (void *)g_fake_view;
  }
  return (void *)g_fake_view;
}

static int mock_vprotect(void *addr, size_t len, unsigned long prot,
                         unsigned long *old_prot) {
  (void)addr;
  (void)len;
  (void)prot;
  *old_prot = 0;
  g_vp_calls++;
  if (g_vp_mode == 0) {
    return 0; /* Fail primary + fallback */
  }
  if (g_vp_mode == 1) {
    return (g_vp_calls == 2) ? 1 : 0; /* Fail primary, succeed fallback */
  }
  return 1;
}

static void *mock_get_mod(const char *name) {
  (void)name;
  return g_mod_ret;
}

static void *mock_get_proc(void *mod, const char *name) {
  (void)mod;
  (void)name;
  return g_proc_ret;
}

static int mock_open_file(const char *path, int oflags) {
  (void)path;
  (void)oflags;
  return g_open_ret;
}
#endif

TEST test_mman(void) {
  void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NEQ(MAP_FAILED, ptr);
  munmap(ptr, 4096);
  PASS();
}

TEST test_posix_cygwin_mmap(void) {
  void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NEQ(MAP_FAILED, ptr);
  munmap(ptr, 4096);
  PASS();
}

TEST test_posix_cygwin_madvise(void) {
  void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NEQ(MAP_FAILED, ptr);
  ASSERT_EQ(0, madvise(ptr, 4096, MADV_NORMAL));
  munmap(ptr, 4096);
  PASS();
}

TEST test_memfd_create(void) {
  int fd;
  int invalid_fd;

  invalid_fd = memfd_create(NULL, 0);
  ASSERT_EQ(-1, invalid_fd);

  invalid_fd = memfd_create("test_invalid", 0xFFFFFFFFU);
  ASSERT_EQ(-1, invalid_fd);

  fd = memfd_create("test_memfd", 0);
  ASSERT(fd >= 0);
#if defined(_WIN32) || defined(_MSC_VER)
  _close(fd);
#else
  close(fd);
#endif
  PASS();
}

#if defined(_WIN32) || defined(_WIN64)
TEST test_mman_win32_edge_cases(void) {
  void *ptr;
  int fd;
  char long_name[220];
  const char *tmp_filename = "test_mman_edge_tmp.dat";

  ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
             -1, 0);
  ASSERT_NEQ(MAP_FAILED, ptr);

  /* Zero-length and invalid-address checks for mlock, munlock, msync, munmap,
   * mprotect */
  ASSERT_EQ(-1, mlock(ptr, 0));
  ASSERT_EQ(-1, munlock(ptr, 0));
  ASSERT_EQ(-1, msync(ptr, 0, MS_SYNC));
  ASSERT_EQ(-1, mprotect(ptr, 0, PROT_READ));
  ASSERT_EQ(-1, munmap(NULL, 4096));
  ASSERT_EQ(-1, munmap(ptr, 0));
  {
    void *heap_blk = malloc(131072);
    void *aligned_heap =
        (void *)(((size_t)heap_blk + 0xFFFFU) & ~(size_t)0xFFFFU);
    ASSERT_NEQ(NULL, heap_blk);
    ASSERT_EQ(-1, munmap(aligned_heap, 4096));
    free(heap_blk);
  }

  posix_mman_set_lock_hooks(mock_lock_fail, mock_lock_fail, mock_flush_fail,
                            mock_unmap_aligned_only);
  ASSERT_EQ(-1, mlock(ptr, 4096));
  ASSERT_EQ(-1, munlock(ptr, 4096));
  ASSERT_EQ(-1, msync(ptr, 4096, MS_SYNC));
  ASSERT_EQ(0, munmap((void *)(size_t)0x20004, 4096));
  posix_mman_reset_win32_hooks();

  ASSERT_EQ(0, munmap(ptr, 4096));

  /* safe_get_osfhandle out-of-range fds (< 0 and >= 2048) and invalid closed fd
   */
  ASSERT_EQ(MAP_FAILED, mmap(NULL, 4096, PROT_READ, MAP_SHARED, -2, 0));
  ASSERT_EQ(MAP_FAILED, mmap(NULL, 4096, PROT_READ, MAP_SHARED, 3000, 0));
  ASSERT_EQ(MAP_FAILED, mmap(NULL, 4096, PROT_READ, MAP_SHARED, 999, 0));

  /* Unaligned file offset mmap + munmap(unaligned) fallback */
#if defined(_MSC_VER)
  _sopen_s(&fd, tmp_filename, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY,
           _SH_DENYNO, _S_IREAD | _S_IWRITE);
#else
  fd = _open(tmp_filename, _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY,
             _S_IREAD | _S_IWRITE);
#endif
  ASSERT(fd >= 0);
  ASSERT_EQ(16, _write(fd, "0123456789abcdef", 16));
  ptr = mmap(NULL, 8, PROT_READ, MAP_SHARED, fd, 4);
  ASSERT_NEQ(MAP_FAILED, ptr);
  ASSERT_EQ('4', ((char *)ptr)[0]);
  ASSERT_EQ(0, munmap(ptr, 8));

  /* Hooked edge cases */
  posix_mman_set_win32_hooks(mock_sysinfo, mock_vquery, mock_temp_path,
                             mock_temp_file, mock_create_map, mock_map_view,
                             mock_vprotect, mock_get_mod, mock_get_proc,
                             mock_open_file);

  /* madvise WILLNEED with hKernel32 == NULL and proc == NULL */
  g_mod_ret = NULL;
  ASSERT_EQ(0, madvise(g_fake_view, 4096, MADV_WILLNEED));
  g_mod_ret = (void *)(size_t)0x100;
  g_proc_ret = NULL;
  ASSERT_EQ(0, madvise(g_fake_view, 4096, MADV_WILLNEED));

  /* mlockall & munlockall with VirtualQuery == 0 and wraparound/guard pages */
  g_vq_mode = 0;
  ASSERT_EQ(0, mlockall(MCL_CURRENT));
  ASSERT_EQ(0, munlockall());

  g_vq_mode = 1;
  g_vq_step = 0;
  ASSERT_EQ(0, mlockall(MCL_CURRENT));
  g_vq_step = 0;
  ASSERT_EQ(0, munlockall());

  /* mmap CreateFileMappingA failure, MapViewOfFileEx failure & 2nd-try success
   */
  g_map_mode = 0;
  ASSERT_EQ(MAP_FAILED, mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, 0));

  g_map_mode = 1;
  g_map_calls = 0;
  ASSERT_EQ(MAP_FAILED, mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, 0));
  g_map_calls = 0;
  ASSERT_EQ(MAP_FAILED,
            mmap(NULL, 4096, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

  g_map_mode = 2;
  g_map_calls = 0;
  ASSERT_NEQ(MAP_FAILED, mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, 0));

  /* mprotect copy-on-write and execute-read fallbacks (both succeed and fail)
   */
  g_vp_mode = 1;
  g_vp_calls = 0;
  ASSERT_EQ(0, mprotect(g_fake_view, 4096, PROT_WRITE));
  g_vp_calls = 0;
  ASSERT_EQ(0, mprotect(g_fake_view, 4096, PROT_EXEC | PROT_WRITE));
  g_vp_calls = 0;
  ASSERT_EQ(0, mprotect(g_fake_view, 4096, PROT_EXEC));

  g_vp_mode = 0;
  g_vp_calls = 0;
  ASSERT_EQ(-1, mprotect(g_fake_view, 4096, PROT_WRITE));
  g_vp_calls = 0;
  ASSERT_EQ(-1, mprotect(g_fake_view, 4096, PROT_EXEC | PROT_WRITE));
  g_vp_calls = 0;
  ASSERT_EQ(-1, mprotect(g_fake_view, 4096, PROT_EXEC));
  g_vp_calls = 0;
  ASSERT_EQ(-1, mprotect(g_fake_view, 4096, PROT_READ));

  /* generate_shm_path GetTempPathA failure + >200 char name + open fail */
  g_tp_ret = 0;
  ASSERT_EQ(-1, shm_open("/fail_tp", O_CREAT | O_RDWR, 0666));
  ASSERT_EQ(-1, posix_memfd_create("fail_tp", 0));

  g_tp_ret = 2;
  g_tf_ret = 0;
  ASSERT_EQ(-1, posix_memfd_create("fail_tf", 0));

  g_tf_ret = 1;
  g_open_ret = -1;
  ASSERT_EQ(-1, posix_memfd_create("fail_open", 0));

  memset(long_name, 'a', sizeof(long_name) - 1);
  long_name[0] = '/';
  long_name[sizeof(long_name) - 1] = '\0';
  ASSERT_EQ(-1, shm_open(long_name, O_RDONLY, 0));

  posix_mman_reset_win32_hooks();

  _close(fd);
  _unlink(tmp_filename);
  PASS();
}
#endif

SUITE(suite_posix_mman_mman) {
  RUN_TEST(test_mman);
  RUN_TEST(test_posix_cygwin_mmap);
  RUN_TEST(test_posix_cygwin_madvise);
  RUN_TEST(test_memfd_create);
#if defined(_WIN32) || defined(_WIN64)
  RUN_TEST(test_mman_win32_edge_cases);
#endif
}
