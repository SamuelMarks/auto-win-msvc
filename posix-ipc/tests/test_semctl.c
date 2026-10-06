#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
/* clang-format on */

TEST test_sem_set(void) {
  int semid;
  struct sembuf sop;
  int res;

  /* Invalid semget */
  ASSERT_EQ(-1, semget(-999, 1, 0));

  /* Create semaphore set */
  semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
  ASSERT_NEQ(-1, semid);

  /* Set value */
  res = semctl(semid, 0, SETVAL, 1);
  ASSERT(res >= 0 || res == -1);

  /* Get value */
  res = semctl(semid, 0, GETVAL);
  ASSERT(res >= 0 || res == -1);

  /* Semop */
  sop.sem_num = 0;
  sop.sem_op = -1;
  sop.sem_flg = IPC_NOWAIT;
  res = semop(semid, &sop, 1);
  ASSERT(res >= 0 || res == -1);

  sop.sem_num = 0;
  sop.sem_op = 1;
  sop.sem_flg = 0;
  res = semop(semid, &sop, 1);
  ASSERT(res >= 0 || res == -1);

  /* Remove semaphore */
  res = semctl(semid, 0, IPC_RMID);
  ASSERT(res >= 0 || res == -1);

  PASS();
}

SUITE(suite_posix_ipc_semctl) { RUN_TEST(test_sem_set); }
