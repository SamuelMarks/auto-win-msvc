#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
#include <string.h>
/* clang-format on */

TEST test_shm_comprehensive(void) {
  int shmid;
  void *addr;
  struct shmid_ds ds;
  int res;

  /* Invalid shmget */
  ASSERT_EQ(-1, shmget(-999, 1024, 0));

  /* Invalid shmat */
  addr = shmat(-1, NULL, 0);
  ASSERT_EQ((void *)-1, addr);

  /* Invalid shmdt */
  res = shmdt(NULL);
  ASSERT_EQ(-1, res);

  /* Create shared memory segment */
  shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0666);
  if (shmid == -1) {
    PASS();
  }

  /* Attach */
  addr = shmat(shmid, NULL, 0);
  if (addr != (void *)-1) {
    memcpy(addr, "test_shm_data", 14);
    ASSERT_STR_EQ("test_shm_data", (char *)addr);

    /* Status */
    memset(&ds, 0, sizeof(ds));
    res = shmctl(shmid, IPC_STAT, &ds);
    (void)res;

    /* Detach */
    res = shmdt(addr);
    ASSERT_EQ(0, res);
  }

  /* Remove */
  res = shmctl(shmid, IPC_RMID, NULL);
  (void)res;

  PASS();
}

SUITE(suite_posix_ipc_shmat) { RUN_TEST(test_shm_comprehensive); }
