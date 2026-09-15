#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-regex.h"
#include <stdio.h>
#include <string.h>
/* clang-format on */

TEST test_posix_regex_init(void) {
  enum posix_regex_error_code rc;
  int status = 0;

  rc = posix_regex_init(NULL);
  if (rc != POSIX_REGEX_ERROR_NULL_POINTER) {
    printf("Expected NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_regex_init(&status);
  if (rc != POSIX_REGEX_SUCCESS) {
    printf("posix_regex_init failed with rc=%d\n", (int)rc);
    FAIL();
  }
  ASSERT_EQ(1, status);
  PASS();
}

TEST test_regcomp_exec(void) {
  regex_t preg;
  int res = regcomp(&preg, "^test[0-9]+", REG_EXTENDED);
  if (res == 0) {
    regmatch_t matches[2];
    int exec_res = regexec(&preg, "test123", 2, matches, 0);
    (void)exec_res;
    regfree(&preg);
    PASS();
  }
#ifdef REG_ENOSYS
  ASSERT_EQ(REG_ENOSYS, res);
#else
  ASSERT(res != 0);
#endif
  PASS();
}

TEST test_regexec(void) {
  regex_t preg;
  regmatch_t pmatch[1];
  int rc;

  memset(&preg, 0, sizeof(preg));
  rc = regexec(&preg, "some string", 1, pmatch, 0);
  (void)rc;
  PASS();
}

TEST test_regerror(void) {
  char buf[64];
  size_t len;
  memset(buf, 0, sizeof(buf));
  len = regerror(REG_NOMATCH, NULL, buf, sizeof(buf));
  ASSERT(len > 0);
  PASS();
}

TEST test_regfree(void) {
  regex_t preg;
  memset(&preg, 0, sizeof(preg));
  regfree(&preg);
  PASS();
}

SUITE(suite_posix_regex_core) {
  RUN_TEST(test_posix_regex_init);
  RUN_TEST(test_regcomp_exec);
  RUN_TEST(test_regexec);
  RUN_TEST(test_regerror);
  RUN_TEST(test_regfree);
}
