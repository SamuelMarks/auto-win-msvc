#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-libgen.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

TEST test_posix_libgen_get_info(void) {
  enum posix_libgen_error_code rc;
  int info;

  info = 0;
  rc = posix_libgen_get_info(NULL);
  if (rc != POSIX_LIBGEN_ERROR_NULL_POINTER) {
    printf("Expected POSIX_LIBGEN_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_libgen_get_info(&info);
  if (rc != POSIX_LIBGEN_SUCCESS) {
    printf("posix_libgen_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_basename(void) {
  char buf[128];
  char *res;

  /* NULL and empty */
  res = basename(NULL);
  ASSERT_STR_EQ(".", res);

  buf[0] = '\0';
  res = basename(buf);
  ASSERT_STR_EQ(".", res);

  /* All slashes */
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "/");
#else
  strcpy(buf, "/");
#endif
  res = basename(buf);
  ASSERT_STR_EQ("/", res);

#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "///");
#else
  strcpy(buf, "///");
#endif
  res = basename(buf);
  ASSERT_STR_EQ("/", res);

#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "\\\\\\");
#else
  strcpy(buf, "\\\\\\");
#endif
  res = basename(buf);
  ASSERT_STR_EQ("/", res);

  /* Trailing slashes */
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "/usr/lib///");
#else
  strcpy(buf, "/usr/lib///");
#endif
  res = basename(buf);
  ASSERT_STR_EQ("lib", res);

  /* Drive letter alone */
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "C:");
#else
  strcpy(buf, "C:");
#endif
  res = basename(buf);
  ASSERT_STR_EQ(".", res);

  /* Drive letter with path */
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "C:\\foo\\bar.txt");
#else
  strcpy(buf, "C:\\foo\\bar.txt");
#endif
  res = basename(buf);
  ASSERT_STR_EQ("bar.txt", res);

  /* Simple filename without slash */
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "file.txt");
#else
  strcpy(buf, "file.txt");
#endif
  res = basename(buf);
  ASSERT_STR_EQ("file.txt", res);

  /* Normal UNIX path */
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "/usr/bin");
#else
  strcpy(buf, "/usr/bin");
#endif
  res = basename(buf);
  ASSERT_STR_EQ("bin", res);

  PASS();
}

TEST test_dirname(void) {
  char buf[128];
  char *res;

  /* NULL and empty */
  res = dirname(NULL);
  ASSERT_STR_EQ(".", res);

  buf[0] = '\0';
  res = dirname(buf);
  ASSERT_STR_EQ(".", res);

  /* All slashes */
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "/");
#else
  strcpy(buf, "/");
#endif
  res = dirname(buf);
  ASSERT_STR_EQ("/", res);

#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "///");
#else
  strcpy(buf, "///");
#endif
  res = dirname(buf);
  ASSERT_STR_EQ("/", res);

#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "\\\\\\");
#else
  strcpy(buf, "\\\\\\");
#endif
  res = dirname(buf);
  ASSERT_STR_EQ("/", res);

  /* Simple path without slashes */
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "usr");
#else
  strcpy(buf, "usr");
#endif
  res = dirname(buf);
  ASSERT_STR_EQ(".", res);

  /* Relative path with slash */
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "usr/lib");
#else
  strcpy(buf, "usr/lib");
#endif
  res = dirname(buf);
  ASSERT_STR_EQ("usr", res);

  /* Path with trailing slash */
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "usr/lib/");
#else
  strcpy(buf, "usr/lib/");
#endif
  res = dirname(buf);
  ASSERT_STR_EQ("usr", res);

  /* Absolute path */
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "/usr/lib");
#else
  strcpy(buf, "/usr/lib");
#endif
  res = dirname(buf);
  ASSERT_STR_EQ("/usr", res);

  /* Absolute root child */
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "/usr");
#else
  strcpy(buf, "/usr");
#endif
  res = dirname(buf);
  ASSERT_STR_EQ("/", res);

  /* Multiple slashes */
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "/usr///lib");
#else
  strcpy(buf, "/usr///lib");
#endif
  res = dirname(buf);
  ASSERT_STR_EQ("/usr", res);

  /* Windows drive paths */
#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "C:\\usr\\lib");
#else
  strcpy(buf, "C:\\usr\\lib");
#endif
  res = dirname(buf);
  ASSERT_STR_EQ("C:\\usr", res);

#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "C:\\usr");
#else
  strcpy(buf, "C:\\usr");
#endif
  res = dirname(buf);
  ASSERT_STR_EQ("C:\\", res);

#if defined(_MSC_VER)
  strcpy_s(buf, sizeof(buf), "C:foo");
#else
  strcpy(buf, "C:foo");
#endif
  res = dirname(buf);
  ASSERT_STR_EQ("C:", res);

  PASS();
}

SUITE(suite_posix_libgen_core) {
  RUN_TEST(test_posix_libgen_get_info);
  RUN_TEST(test_basename);
  RUN_TEST(test_dirname);
}
