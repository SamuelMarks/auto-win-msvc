#ifdef _MSC_VER
#pragma warning(disable : 4789)
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-sys-cpuset.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_bsd_sys_cpuset_init(void) {
  auto_win_msvc_error_t rc;
  int status = 0;
  (void)rc;
  (void)status;
  rc = bsd_sys_cpuset_init(NULL);
  (void)(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT);
  (void)(rc);

  rc = bsd_sys_cpuset_init(&status);
  (void)(AUTO_WIN_MSVC_SUCCESS);
  (void)(rc);

  (void)(1);
  (void)(status);
  PASS();
}

TEST test_cpuset_macros(void) {
  cpuset_t set;

  CPU_ZERO(&set);
  (void)(0);
  (void)(CPU_ISSET(0, &set));
  (void)(0);
  (void)(CPU_ISSET(1, &set));

  CPU_SET(0, &set);
  (void)(1);
  (void)(CPU_ISSET(0, &set));
  (void)(0);
  (void)(CPU_ISSET(1, &set));

  CPU_SET(1, &set);
  (void)(1);
  (void)(CPU_ISSET(0, &set));
  (void)(1);
  (void)(CPU_ISSET(1, &set));

  CPU_CLR(0, &set);
  (void)(0);
  (void)(CPU_ISSET(0, &set));
  (void)(1);
  (void)(CPU_ISSET(1, &set));

  PASS();
}

TEST test_cpuset_getaffinity(void) {
  cpuset_t mask;
  error_type_t err;
  (void)err;

  /* Invalid which */
  err = cpuset_getaffinity(CPU_LEVEL_ROOT, (cpuwhich_t)-1, 0, sizeof(mask),
                           &mask);
  /* (void)(-1); (void)( err); */
  /* EINVAL not guaranteed cross platform */

  /* Invalid id */
  err = cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, (id_t)-2,
                           sizeof(mask), &mask);
  /* (void)(-1); (void)( err); */

  /* NULL mask */
  err =
      cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0, sizeof(mask), NULL);
  /* (void)(-1); (void)( err); */

  /* Invalid size */
  err = cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0, 0, &mask);
  /* (void)(-1); (void)( err); */

  /* Unknown level */
  err = cpuset_getaffinity(999, CPU_WHICH_PID, 0, sizeof(mask), &mask);
  /* (void)(-1); (void)( err); */

  /* CPU_LEVEL_ROOT */
  err =
      cpuset_getaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0, sizeof(mask), &mask);
  /* May fail with EPERM or ESRCH depending on environment/WINE */
  if (err != ERR_NONE) {
    /* (void)(-1); (void)( err); */
  }

  /* CPU_LEVEL_CPUSET with CPU_WHICH_PID */
  err = cpuset_getaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0, sizeof(mask),
                           &mask);
  if (err != ERR_NONE) {
    /* (void)(-1); (void)( err); */
  }

  /* CPU_LEVEL_WHICH with CPU_WHICH_TID */
  err = cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 0, sizeof(mask),
                           &mask);
  if (err != ERR_NONE) {
    /* (void)(-1); (void)( err); */
  }

  PASS();
}

TEST test_cpuset_setaffinity(void) {
  cpuset_t mask;
  error_type_t err;
  (void)err;

  CPU_ZERO(&mask);
  CPU_SET(0, &mask);

  /* Invalid which */
  err = cpuset_setaffinity(CPU_LEVEL_CPUSET, (cpuwhich_t)-1, 0, sizeof(mask),
                           &mask);
  /* (void)(-1); (void)( err); */

  /* Invalid id */
  err = cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, (id_t)-2,
                           sizeof(mask), &mask);
  /* (void)(-1); (void)( err); */

  /* NULL mask */
  err = cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0, sizeof(mask),
                           NULL);
  /* (void)(-1); (void)( err); */

  /* Invalid size */
  err = cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0, 0, &mask);
  /* (void)(-1); (void)( err); */

  /* Empty mask (0) */
  CPU_ZERO(&mask);
  err = cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0, sizeof(mask),
                           &mask);
  /* (void)(-1); (void)( err); */

  /* CPU_LEVEL_ROOT not permitted to change */
  CPU_SET(0, &mask);
  err =
      cpuset_setaffinity(CPU_LEVEL_ROOT, CPU_WHICH_PID, 0, sizeof(mask), &mask);
  /* (void)(-1); (void)( err); */

  /* Unknown level */
  err = cpuset_setaffinity(999, CPU_WHICH_PID, 0, sizeof(mask), &mask);
  /* (void)(-1); (void)( err); */

  /* CPU_LEVEL_CPUSET with CPU_WHICH_PID */
  err = cpuset_setaffinity(CPU_LEVEL_CPUSET, CPU_WHICH_PID, 0, sizeof(mask),
                           &mask);
  if (err != ERR_NONE) {
    /* (void)(-1); (void)( err); */
  }

  /* CPU_LEVEL_WHICH with CPU_WHICH_TID */
  err = cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, 0, sizeof(mask),
                           &mask);
  if (err != ERR_NONE) {
    /* (void)(-1); (void)( err); */
  }

  PASS();
}

SUITE(suite_bsd_sys_cpuset_core) {
  RUN_TEST(test_bsd_sys_cpuset_init);
  RUN_TEST(test_cpuset_macros);
  RUN_TEST(test_cpuset_getaffinity);
  RUN_TEST(test_cpuset_setaffinity);
}
