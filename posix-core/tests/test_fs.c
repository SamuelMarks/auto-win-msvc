#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(_MSC_VER) || defined(_WIN32)
#include <process.h>
#include <io.h>
#else
#include <unistd.h>
#endif
/* clang-format on */

TEST test_chown(void) {
  FILE *f;
  int rc;

  ASSERT_EQ(-1, chown(NULL, (uid_t)-1, (gid_t)-1));
  ASSERT_EQ(-1,
            chown("nonexistent_file_chown_xyz_123.tmp", (uid_t)-1, (gid_t)-1));

  f = fopen("test_chown.tmp", "w");
  ASSERT(f != NULL);
  fputs("data", f);
  fclose(f);

  rc = chown("test_chown.tmp", (uid_t)-1, (gid_t)-1);
  (void)rc;
  remove("test_chown.tmp");
  PASS();
}

TEST test_faccessat(void) {
  FILE *f;
  int rc;

  ASSERT_EQ(-1, faccessat(AT_FDCWD, NULL, 0, 0));
  ASSERT_EQ(-1, faccessat(AT_FDCWD, "nonexistent_faccessat_xyz.tmp", 0, 0));

  f = fopen("test_faccessat.tmp", "w");
  ASSERT(f != NULL);
  fputs("data", f);
  fclose(f);

  rc = faccessat(AT_FDCWD, "test_faccessat.tmp", 0, 0);
  ASSERT_EQ(0, rc);

  remove("test_faccessat.tmp");
  PASS();
}

TEST test_fchown(void) {
  FILE *f;
  int fd;
  int rc;

  ASSERT_EQ(-1, fchown(-1, (uid_t)-1, (gid_t)-1));

  f = fopen("test_fchown.tmp", "w");
  ASSERT(f != NULL);
  fputs("data", f);
  fclose(f);

  fd = open("test_fchown.tmp", O_RDWR, 0666);
  if (fd >= 0) {
    rc = fchown(fd, (uid_t)-1, (gid_t)-1);
    (void)rc;
    close(fd);
  }
  remove("test_fchown.tmp");
  PASS();
}

TEST test_fchownat(void) {
  FILE *f;
  int rc;

  ASSERT_EQ(-1, fchownat(AT_FDCWD, NULL, (uid_t)-1, (gid_t)-1, 0));
  ASSERT_EQ(-1, fchownat(AT_FDCWD, "nonexistent_fchownat.tmp", (uid_t)-1,
                         (gid_t)-1, 0));

  f = fopen("test_fchownat.tmp", "w");
  ASSERT(f != NULL);
  fputs("data", f);
  fclose(f);

  rc = fchownat(AT_FDCWD, "test_fchownat.tmp", (uid_t)-1, (gid_t)-1, 0);
  (void)rc;
  remove("test_fchownat.tmp");
  PASS();
}

TEST test_lchown(void) {
  FILE *f;
  int rc;

  ASSERT_EQ(-1, lchown(NULL, (uid_t)-1, (gid_t)-1));
  ASSERT_EQ(-1, lchown("nonexistent_lchown.tmp", (uid_t)-1, (gid_t)-1));

  f = fopen("test_lchown.tmp", "w");
  ASSERT(f != NULL);
  fputs("data", f);
  fclose(f);

  rc = lchown("test_lchown.tmp", (uid_t)-1, (gid_t)-1);
  (void)rc;
  remove("test_lchown.tmp");
  PASS();
}

TEST test_link(void) {
  FILE *f;
  int rc;

  ASSERT_EQ(-1, link(NULL, NULL));
  ASSERT_EQ(-1, link("nonexistent_link_src.tmp", "test_link_dst.tmp"));

  f = fopen("test_link1.tmp", "w");
  ASSERT(f != NULL);
  fputs("data", f);
  fclose(f);

  rc = link("test_link1.tmp", "test_link2.tmp");
  if (rc == 0) {
    remove("test_link2.tmp");
  }
  remove("test_link1.tmp");
  PASS();
}

TEST test_linkat(void) {
  FILE *f;
  int rc;

  ASSERT_EQ(-1, linkat(AT_FDCWD, NULL, AT_FDCWD, "test_linkat_dst.tmp", 0));
  ASSERT_EQ(-1, linkat(AT_FDCWD, "nonexistent_linkat_src.tmp", AT_FDCWD,
                       "test_linkat_dst.tmp", 0));

  f = fopen("test_linkat1.tmp", "w");
  ASSERT(f != NULL);
  fputs("data", f);
  fclose(f);

  rc = linkat(AT_FDCWD, "test_linkat1.tmp", AT_FDCWD, "test_linkat2.tmp", 0);
  if (rc == 0) {
    remove("test_linkat2.tmp");
  }
  remove("test_linkat1.tmp");
  PASS();
}

TEST test_symlink(void) {
  FILE *f;
  int rc;

  ASSERT_EQ(-1, symlink(NULL, NULL));

  f = fopen("test_symlink_tgt.tmp", "w");
  ASSERT(f != NULL);
  fputs("target", f);
  fclose(f);

  rc = symlink("test_symlink_tgt.tmp", "test_symlink_lnk.tmp");
  if (rc == 0) {
    remove("test_symlink_lnk.tmp");
  }
  remove("test_symlink_tgt.tmp");
  PASS();
}

TEST test_symlinkat(void) {
  int rc;

  ASSERT_EQ(-1, symlinkat(NULL, AT_FDCWD, "test_symlinkat_lnk.tmp"));

  rc = symlinkat("test_target.tmp", AT_FDCWD, "test_symlinkat_lnk.tmp");
  if (rc == 0) {
    remove("test_symlinkat_lnk.tmp");
  }
  PASS();
}

SUITE(suite_posix_core_fs) {
  RUN_TEST(test_chown);
  RUN_TEST(test_faccessat);
  RUN_TEST(test_fchown);
  RUN_TEST(test_fchownat);
  RUN_TEST(test_lchown);
  RUN_TEST(test_link);
  RUN_TEST(test_linkat);
  RUN_TEST(test_symlink);
  RUN_TEST(test_symlinkat);
}
