/* clang-format off */
#include "greatest.h"
#include <sys/capsicum.h>
#include <capsicum_helpers.h>
#include <libcasper.h>
#include <casper/cap_fileargs.h>
#include <casper/cap_pwd.h>
#include <casper/cap_grp.h>
#include <casper/cap_sysctl.h>
#include <sys/procdesc.h>
/* clang-format on */

TEST test_bsd_capsicum_rights_and_helpers(void) {
  cap_rights_t rights, r2;
  unsigned int mode = 1;
  uint32_t fcntls = 0;

  ASSERT_EQ(0, cap_enter());
  ASSERT_EQ(0, cap_getmode(&mode));
  ASSERT_EQ(0U, mode);
  ASSERT_EQ(0, cap_sandboxed());

  cap_rights_init(&rights, CAP_READ, CAP_WRITE);
  ASSERT(cap_rights_is_valid(&rights));
  ASSERT(cap_rights_is_set(&rights, CAP_READ));
  cap_rights_set(&rights, CAP_SEEK);
  ASSERT(cap_rights_is_set(&rights, CAP_SEEK));

  cap_rights_init(&r2, CAP_READ);
  ASSERT(cap_rights_contains(&rights, &r2));
  cap_rights_merge(&r2, &rights);
  cap_rights_clear(&rights, CAP_WRITE);
  ASSERT(!cap_rights_is_set(&rights, CAP_WRITE));
  cap_rights_remove(&rights, &r2);

  ASSERT_EQ(0, cap_rights_limit(0, &r2));
  ASSERT_EQ(0, cap_rights_get(0, &rights));
  ASSERT_EQ(0, cap_ioctls_limit(0, NULL, 0));
  ASSERT_EQ(0, cap_fcntls_limit(0, CAP_FCNTL_ALL));
  ASSERT_EQ(0, cap_fcntls_get(0, &fcntls));

  ASSERT_EQ(0, caph_enter());
  ASSERT_EQ(0, caph_enter_casper());
  ASSERT_EQ(0, caph_limit_stdio());
  caph_cache_tzdata();
  caph_cache_catpages();
  PASS();
}

TEST test_bsd_casper_and_fileargs(void) {
  cap_channel_t *ch, *s;
  fileargs_t *fa;
  int fd = -1;

  ch = cap_init();
  ASSERT(ch != NULL);
  s = cap_service_open(ch, "system.fileargs");
  ASSERT(s != NULL);
  ASSERT_EQ(0, cap_limit_set(s, NULL));
  cap_close(s);
  cap_close(ch);

  fa = fileargs_init(0, NULL, 0, 0644, NULL, FA_OPEN);
  ASSERT(fa != NULL);
  ASSERT_EQ(0, fileargs_caph_enter(fa));
  fileargs_free(fa);

  ASSERT_EQ(-1, pdfork(&fd, 0));
  PASS();
}

SUITE(suite_bsd_capsicum_core) {
  RUN_TEST(test_bsd_capsicum_rights_and_helpers);
  RUN_TEST(test_bsd_casper_and_fileargs);
}
