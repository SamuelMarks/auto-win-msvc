#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
/* clang-format on */

SUITE_EXTERN(suite_posix_ipc_ipc);
SUITE_EXTERN(suite_posix_ipc_ftok);
SUITE_EXTERN(suite_posix_ipc_msgctl);
SUITE_EXTERN(suite_posix_ipc_msgget);
SUITE_EXTERN(suite_posix_ipc_msgrcv);
SUITE_EXTERN(suite_posix_ipc_msgsnd);
SUITE_EXTERN(suite_posix_ipc_semctl);
SUITE_EXTERN(suite_posix_ipc_semget);
SUITE_EXTERN(suite_posix_ipc_semop);
SUITE_EXTERN(suite_posix_ipc_shmat);
SUITE_EXTERN(suite_posix_ipc_shmctl);
SUITE_EXTERN(suite_posix_ipc_shmdt);
SUITE_EXTERN(suite_posix_ipc_shmget);

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_posix_ipc_ipc);
  RUN_SUITE(suite_posix_ipc_ftok);
  RUN_SUITE(suite_posix_ipc_msgctl);
  RUN_SUITE(suite_posix_ipc_msgget);
  RUN_SUITE(suite_posix_ipc_msgrcv);
  RUN_SUITE(suite_posix_ipc_msgsnd);
  RUN_SUITE(suite_posix_ipc_semctl);
  RUN_SUITE(suite_posix_ipc_semget);
  RUN_SUITE(suite_posix_ipc_semop);
  RUN_SUITE(suite_posix_ipc_shmat);
  RUN_SUITE(suite_posix_ipc_shmctl);
  RUN_SUITE(suite_posix_ipc_shmdt);
  RUN_SUITE(suite_posix_ipc_shmget);
  GREATEST_MAIN_END();
  return 0;
}
