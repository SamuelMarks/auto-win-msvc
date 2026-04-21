#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
#include "greatest.h"
#include "posix-core.h"
#include <process.h> /* For _getpid if needed */

TEST test_openat(void) { SKIP(); /* Generated stub for openat */ }

TEST test_sync_file_range(void) {
  SKIP(); /* Generated stub for sync_file_range */
}

TEST test_fdatasync(void) { SKIP(); /* Generated stub for fdatasync */ }

TEST test_pipe(void) { SKIP(); /* Generated stub for pipe */ }

TEST test_pipe2(void) { SKIP(); /* Generated stub for pipe2 */ }

TEST test_pread(void) { SKIP(); /* Generated stub for pread */ }

TEST test_pwrite(void) { SKIP(); /* Generated stub for pwrite */ }

TEST test_readlink(void) { SKIP(); /* Generated stub for readlink */ }

TEST test_readlinkat(void) { SKIP(); /* Generated stub for readlinkat */ }

TEST test_sync(void) { SKIP(); /* Generated stub for sync */ }

SUITE(suite_posix_core_io) {
  RUN_TEST(test_openat);
  RUN_TEST(test_sync_file_range);
  RUN_TEST(test_fdatasync);
  RUN_TEST(test_pipe);
  RUN_TEST(test_pipe2);
  RUN_TEST(test_pread);
  RUN_TEST(test_pwrite);
  RUN_TEST(test_readlink);
  RUN_TEST(test_readlinkat);
  RUN_TEST(test_sync);
}
