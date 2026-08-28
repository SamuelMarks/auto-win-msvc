#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

SUITE_EXTERN(suite_posix_mman_mman);
SUITE_EXTERN(suite_posix_mman_mlockall);
SUITE_EXTERN(suite_posix_mman_munlockall);
SUITE_EXTERN(suite_posix_mman_madvise);
SUITE_EXTERN(suite_posix_mman_mlock);
SUITE_EXTERN(suite_posix_mman_mmap);
SUITE_EXTERN(suite_posix_mman_mprotect);
SUITE_EXTERN(suite_posix_mman_msync);
SUITE_EXTERN(suite_posix_mman_munlock);
SUITE_EXTERN(suite_posix_mman_munmap);
SUITE_EXTERN(suite_posix_mman_shm);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_posix_mman_mman);
  RUN_SUITE(suite_posix_mman_mlockall);
  RUN_SUITE(suite_posix_mman_munlockall);
  RUN_SUITE(suite_posix_mman_madvise);
  RUN_SUITE(suite_posix_mman_mlock);
  RUN_SUITE(suite_posix_mman_mmap);
  RUN_SUITE(suite_posix_mman_mprotect);
  RUN_SUITE(suite_posix_mman_msync);
  RUN_SUITE(suite_posix_mman_munlock);
  RUN_SUITE(suite_posix_mman_munmap);
  RUN_SUITE(suite_posix_mman_shm);
  GREATEST_MAIN_END();
}
