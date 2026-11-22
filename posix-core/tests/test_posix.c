#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_MSC_VER) || defined(_WIN32)
#include <process.h>
#include <io.h>
#else
#include <unistd.h>
#endif
/* clang-format on */

TEST test_posix_core_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;

  rc = posix_core_init(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_core_init(&status);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    printf("posix_core_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  PASS();
}

TEST test_posix_close(void) {
  int fd = -1;
  (void)fd;

  fd = open("test_pclose.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    posix_close(fd);
  }

  remove("test_pclose.tmp");
  PASS();
}

TEST test_posix_read(void) {
  int fd = -1;
  char buf[16];
  ssize_t bytes_read = 0;
  (void)fd;
  (void)bytes_read;

  fd = open("test_pread_t.tmp", O_RDWR | O_CREAT, 0666);

  posix_write(fd, "testdata", 8);
  posix_close(fd);

  fd = open("test_pread_t.tmp", O_RDONLY, 0);

  memset(buf, 0, sizeof(buf));
  bytes_read = posix_read(fd, buf, 8);

  posix_close(fd);

  remove("test_pread_t.tmp");
  PASS();
}

TEST test_posix_write(void) {
  int fd = -1;
  ssize_t written = 0;
  (void)fd;
  (void)written;

  fd = open("test_pwrite_t.tmp", O_RDWR | O_CREAT, 0666);

  written = posix_write(fd, "write_check", 11);

  posix_close(fd);

  remove("test_pwrite_t.tmp");
  PASS();
}

TEST test_posix_fopen(void) {
  FILE *f;

  ASSERT_EQ(0, posix_feof(NULL));
  ASSERT_EQ(0, posix_ferror(NULL));
  ASSERT_EQ(-1, posix_fileno(NULL));

  f = posix_fopen("test_pfopen.tmp", "w+");

  ASSERT_EQ(0, posix_feof(f));
  ASSERT_EQ(0, posix_ferror(f));
  ASSERT(posix_fileno(f) >= 0);

  fputs("abc", f);
  fclose(f);

  remove("test_pfopen.tmp");
  PASS();
}

TEST test_posix_fadvise(void) {
  int fd = -1;
  (void)fd;

  fd = open("test_pfadv.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    posix_write(fd, "1234567890", 10);
    posix_fadvise(fd, 0, 10, 1);
    posix_close(fd);
    remove("test_pfadv.tmp");
  }

  posix_fadvise(-1, 0, 10, 1);
  posix_fadvise(0, -1, 10, 1);
  posix_fadvise(0, 0, -1, 1);
  posix_fadvise(0, 0, 10, -1);

  PASS();
}

TEST test_posix_fallocate(void) {
  int fd = -1;
  (void)fd;

  fd = open("test_pfalloc.tmp", O_RDWR | O_CREAT, 0666);
  if (fd >= 0) {
    posix_fallocate(fd, 0, 100);
    posix_close(fd);
    remove("test_pfalloc.tmp");
  }

  posix_fallocate(-1, 0, 100);
  posix_fallocate(0, -1, 100);
  posix_fallocate(0, 0, -1);
  posix_fallocate(0, 0, 0);

  PASS();
}

TEST test_posix_rename(void) {
  FILE *f;
  int rc;

  f = fopen("pren1.tmp", "w");
  if (f) {
    fputs("data", f);
    fclose(f);
  }

  rc = posix_rename("pren1.tmp", "pren2.tmp");
  ASSERT_EQ(0, rc);

  remove("pren1.tmp");
  remove("pren2.tmp");
  PASS();
}

TEST test_posix_mkstemp(void) {
  char tmpl[32];
  int fd = -1;
  (void)fd;

#if defined(_MSC_VER)
  strcpy_s(tmpl, sizeof(tmpl), "pmXXXXXX");
#else
  strcpy(tmpl, "pmXXXXXX");
#endif

  fd = posix_mkstemp(tmpl);
  if (fd >= 0) {
    posix_close(fd);
    remove(tmpl);
  }
  PASS();
}

TEST test_posix_mkostemp_and_dos83(void) {
  char tmpl[32];
  char out[32];
  int fd;

  ASSERT_EQ(-1, posix_mkostemp(NULL, 0));
#if defined(_MSC_VER)
  strcpy_s(tmpl, sizeof(tmpl), "bad");
#else
  strcpy(tmpl, "bad");
#endif
  ASSERT_EQ(-1, posix_mkostemp(tmpl, 0));
#if defined(_MSC_VER)
  strcpy_s(tmpl, sizeof(tmpl), "poXXXXXX");
#else
  strcpy(tmpl, "poXXXXXX");
#endif
  fd = posix_mkostemp(tmpl, 0);
  ASSERT(fd >= 0);
  posix_close(fd);
  remove(tmpl);

  ASSERT_EQ(-1, posix_rename(NULL, "b"));
  ASSERT_EQ(-1, posix_rename("a", NULL));

  ASSERT_EQ(-1,
            awm_dos_mkstemp_format_83(NULL, sizeof(out), "./sedXXXXXX", 0U));
  ASSERT_EQ(-1, awm_dos_mkstemp_format_83(out, sizeof(out), NULL, 0U));
  ASSERT_EQ(-1, awm_dos_mkstemp_format_83(out, sizeof(out), "short", 0U));
  ASSERT_EQ(-1, awm_dos_mkstemp_format_83(out, 5U, "./sedXXXXXX", 0U));
  ASSERT_EQ(-1, awm_dos_mkstemp_format_83(out, sizeof(out), "./sedYYYYYY", 0U));

  ASSERT_EQ(0, awm_dos_mkstemp_format_83(out, 12U, "./sedXXXXXX", 7U));
  ASSERT_STR_EQ("SED00007", out);

  ASSERT_EQ(0, awm_dos_mkstemp_format_83(out, sizeof(out), "XXXXXX", 12U));
  ASSERT_STR_EQ("T00012", out);

  ASSERT_EQ(0, awm_dos_mkstemp_format_83(out, sizeof(out), "aXXXXXX", 3U));
  ASSERT_STR_EQ("A000003", out);

  ASSERT_EQ(0, awm_dos_mkstemp_format_83(out, sizeof(out), "-XXXXXX", 4U));
  ASSERT_STR_EQ("T000004", out);

  ASSERT_EQ(0, awm_dos_mkstemp_format_83(out, sizeof(out),
                                         "C:\\TMP\\long-XXXXXX", 99U));
  ASSERT_STR_EQ("C:\\TMP\\LON00099", out);

  ASSERT_EQ(0, awm_dos_mkstemp_format_83(out, sizeof(out), "abXXXXXX", 8U));
  ASSERT_STR_EQ("AB00008", out);

  ASSERT_EQ(0,
            awm_dos_mkstemp_format_83(out, sizeof(out), "dir/a-bXXXXXX", 5U));
  ASSERT_STR_EQ("dir/ATB00005", out);

  ASSERT_EQ(0, awm_dos_mkstemp_format_83(out, sizeof(out), ".\\sedXXXXXX", 9U));
  ASSERT_STR_EQ("SED00009", out);

  PASS();
}

TEST test_awm_scratch_popen_and_fwriting(void) {
  FILE *fp;
  char buf[64];

  ASSERT_EQ(0, awm_fwriting(NULL));
  fp = fopen("test_fwriting.tmp", "w");
  ASSERT(fp != NULL);
  fputc('x', fp);
  ASSERT_EQ(1, awm_fwriting(fp));
  fclose(fp);

  fp = fopen("test_fwriting.tmp", "r");
  ASSERT(fp != NULL);
  ASSERT_EQ(0, awm_fwriting(fp));
  fclose(fp);
  remove("test_fwriting.tmp");

  ASSERT(awm_popen_scratch(NULL, "r") == NULL);
  ASSERT(awm_popen_scratch("echo hi", NULL) == NULL);
  ASSERT(awm_popen_scratch("echo hi", "x") == NULL);
  ASSERT(awm_popen_scratch("echo hi", "rzz") == NULL);
  ASSERT_EQ(-1, awm_pclose_scratch(NULL));

#if !defined(__DOS__) && !defined(__MSDOS__) && !defined(__WATCOMC__) &&       \
    !defined(__EMSCRIPTEN__)
  fp = awm_popen_scratch("echo sed_pipe_ok", "r");
  ASSERT(fp != NULL);
  memset(buf, 0, sizeof(buf));
  ASSERT(fgets(buf, (int)sizeof(buf), fp) != NULL);
  ASSERT(strstr(buf, "sed_pipe_ok") != NULL);
  (void)awm_pclose_scratch(fp);

  fp = fopen("test_untracked.tmp", "w");
  ASSERT(fp != NULL);
  ASSERT_EQ(-1, awm_pclose_scratch(fp));
  fclose(fp);
  remove("test_untracked.tmp");

  fp = awm_popen_scratch("echo bin_ok", "rb");
  ASSERT(fp != NULL);
  (void)awm_pclose_scratch(fp);

  fp = awm_popen_scratch("echo txt_ok", "rt");
  ASSERT(fp != NULL);
  (void)awm_pclose_scratch(fp);

  fp = awm_popen_scratch("cat > test_pw_sink.tmp", "wb");
  if (fp != NULL) {
    fputs("sink_ok\n", fp);
    (void)awm_pclose_scratch(fp);
    remove("test_pw_sink.tmp");
  }

  {
    FILE *slots[8];
    int idx;
    for (idx = 0; idx < 8; ++idx) {
      slots[idx] = awm_popen_scratch("echo slot", "r");
      ASSERT(slots[idx] != NULL);
    }
    ASSERT(awm_popen_scratch("echo overflow", "r") == NULL);
    for (idx = 0; idx < 8; ++idx) {
      (void)awm_pclose_scratch(slots[idx]);
    }
  }
#else
  (void)buf;
#endif

  PASS();
}

SUITE(suite_posix_core_posix) {
  RUN_TEST(test_posix_core_init);
  RUN_TEST(test_posix_close);
  RUN_TEST(test_posix_read);
  RUN_TEST(test_posix_write);
  RUN_TEST(test_posix_fopen);
  RUN_TEST(test_posix_fadvise);
  RUN_TEST(test_posix_fallocate);
  RUN_TEST(test_posix_rename);
  RUN_TEST(test_posix_mkstemp);
  RUN_TEST(test_posix_mkostemp_and_dos83);
  RUN_TEST(test_awm_scratch_popen_and_fwriting);
}
