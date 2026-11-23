/* test_namespace_win_shim.c - Unit tests for namespace_win_shim.h and
 * auto-win-msvc-error.h */
/* clang-format off */
#include "namespace_win_shim.h"
#include "auto-win-msvc-error.h"
#include "greatest.h"
/* clang-format on */

TEST test_namespace_win_intrinsics(void) {
  ASSERT_EQ(0, posix_builtin_popcountll((namespace_win_uint64_t)0U));
  ASSERT_EQ(1, posix_builtin_popcountll((namespace_win_uint64_t)1U));
  ASSERT_EQ(4, posix_builtin_popcountll((namespace_win_uint64_t)0x1010101UL));

  ASSERT_EQ(64, posix_builtin_ctzll((namespace_win_uint64_t)0U));
  ASSERT_EQ(0, posix_builtin_ctzll((namespace_win_uint64_t)1U));
  ASSERT_EQ(3, posix_builtin_ctzll((namespace_win_uint64_t)8U));
  ASSERT_EQ(5, posix_builtin_ctzll((namespace_win_uint64_t)96U));
  PASS();
}

TEST test_auto_win_msvc_error_helpers(void) {
  static const auto_win_msvc_error_t all_codes[] = {
      AUTO_WIN_MSVC_SUCCESS,
      AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT,
      AUTO_WIN_MSVC_ERROR_OUT_OF_MEMORY,
      AUTO_WIN_MSVC_ERROR_NOT_IMPLEMENTED,
      AUTO_WIN_MSVC_ERROR_OS_CALL_FAILED,
      AUTO_WIN_MSVC_ERROR_IO,
      AUTO_WIN_MSVC_ERROR_PERMISSION_DENIED,
      AUTO_WIN_MSVC_ERROR_NOT_FOUND,
      AUTO_WIN_MSVC_ERROR_ALREADY_EXISTS,
      AUTO_WIN_MSVC_ERROR_TIMEOUT,
      AUTO_WIN_MSVC_ERROR_WOULD_BLOCK,
      AUTO_WIN_MSVC_ERROR_UNKNOWN};
  size_t i;

  for (i = 0; i < sizeof(all_codes) / sizeof(all_codes[0]); i++) {
    const char *msg = auto_win_msvc_error_string(all_codes[i]);
    int err_no = auto_win_msvc_error_to_errno(all_codes[i]);
    ASSERT(msg != NULL);
    ASSERT(msg[0] != '\0');
    if (all_codes[i] == AUTO_WIN_MSVC_SUCCESS) {
      ASSERT_EQ(0, err_no);
    } else {
      ASSERT(err_no != 0);
    }
  }

  ASSERT_STR_EQ("Unknown error",
                auto_win_msvc_error_string((auto_win_msvc_error_t)999));
  ASSERT_EQ(EINVAL, auto_win_msvc_error_to_errno((auto_win_msvc_error_t)999));
  PASS();
}

TEST test_namespace_win_posix_close_coexistence(void) {
  errno = 0;
  ASSERT_EQ(-1, posix_close((intptr_t)-1));
  ASSERT_EQ(EBADF, errno);
  PASS();
}

SUITE(suite_namespace_win_shim) {
  RUN_TEST(test_namespace_win_intrinsics);
  RUN_TEST(test_auto_win_msvc_error_helpers);
  RUN_TEST(test_namespace_win_posix_close_coexistence);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_namespace_win_shim);
  GREATEST_MAIN_END();
}
