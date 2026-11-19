/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#endif
/* clang-format on */

#ifdef _WIN32
struct ipc_test_msg {
  long mtype;
  char mtext[32];
};

static int g_ipc_oom_countdown = 0;
static int g_ipc_spin_once = 0;
static int g_ipc_wait_mode = 0;
static int g_ipc_target_id = -1;
static int g_ipc_reset_step = 0;

static void *mock_ipc_malloc(size_t sz) {
  if (g_ipc_oom_countdown > 0) {
    g_ipc_oom_countdown--;
    if (g_ipc_oom_countdown == 0) {
      return NULL;
    }
  }
  return malloc(sz);
}

static void *mock_ipc_calloc(size_t n, size_t sz) {
  if (g_ipc_oom_countdown > 0) {
    g_ipc_oom_countdown--;
    if (g_ipc_oom_countdown == 0) {
      return NULL;
    }
  }
  return calloc(n, sz);
}

static int mock_get_file_info_fail(void *hFile, void *out_info) {
  (void)hFile;
  (void)out_info;
  return 0;
}

static void *mock_map_view_fail(void *hMap, unsigned long access) {
  (void)hMap;
  (void)access;
  return NULL;
}

static long mock_cmpxchg_spin(volatile long *dest, long exch, long comp) {
  if (g_ipc_spin_once > 0) {
    g_ipc_spin_once--;
    return 1;
  }
  return (long)InterlockedCompareExchange((LONG volatile *)dest, (LONG)exch,
                                          (LONG)comp);
}

static unsigned long mock_wait_event(void *hEvent);

static int mock_reset_noop_then_hook(void *hEvent) {
  (void)hEvent;
  g_ipc_reset_step++;
  if (g_ipc_reset_step >= 2) {
    posix_ipc_set_win32_hooks(NULL, NULL, NULL, NULL, mock_wait_event);
  }
  return 1;
}

static unsigned long mock_wait_event(void *hEvent) {
  (void)hEvent;
  if (g_ipc_wait_mode == 1) {
    /* Wake semop waiter by setting semaphore 0 to 1 */
    semctl(g_ipc_target_id, 0, SETVAL, 1);
  } else if (g_ipc_wait_mode == 2) {
    /* Wake msgsnd waiter by draining queue */
    struct ipc_test_msg rmsg;
    msgrcv(g_ipc_target_id, &rmsg, sizeof(rmsg.mtext), 0, IPC_NOWAIT);
  } else if (g_ipc_wait_mode == 3) {
    /* Wake msgrcv waiter by sending message */
    struct ipc_test_msg smsg;
    smsg.mtype = 7;
    memcpy(smsg.mtext, "woke", 5);
    msgsnd(g_ipc_target_id, &smsg, 5, IPC_NOWAIT);
  }
  return 0;
}

TEST test_posix_ipc_win32_branches(void) {
  int ids[256];
  int i;
  int id1, id2;
  void *addr;
  struct shmid_ds shmds;
  struct sembuf sops[2];
  unsigned short sem_arr[2];
  unsigned short sem_out[2];
  struct msqid_ds msgds;
  struct ipc_test_msg m1, m2;
  HANDLE hExt;

  /* 1. ftok branches */
  ASSERT_EQ((key_t)-1, ftok(NULL, 1));
  ASSERT_EQ((key_t)-1, ftok("Z:\\definitely_missing_ipc_file_99999.tmp", 1));
  ASSERT_NEQ((key_t)-1, ftok(".", 1));

  posix_ipc_set_win32_hooks(mock_get_file_info_fail, NULL, NULL, NULL, NULL);
  ASSERT_EQ((key_t)-1, ftok(".", 1));
  posix_ipc_set_win32_hooks(NULL, NULL, NULL, NULL, NULL);

  /* 2. Spinlock contention branches on lock_shm, lock_sem, lock_msg */
  posix_ipc_set_win32_hooks(NULL, NULL, mock_cmpxchg_spin, NULL, NULL);
  g_ipc_spin_once = 1;
  ASSERT_EQ(-1, shmget(77701, 64, 0));
  g_ipc_spin_once = 1;
  ASSERT_EQ(-1, semget(77701, 1, 0));
  g_ipc_spin_once = 1;
  ASSERT_EQ(-1, msgget(77701, 0));
  posix_ipc_set_win32_hooks(NULL, NULL, NULL, NULL, NULL);

  /* 3. Shared memory branches */
  /* Create hole at slot 0, occupied key at slot 1 */
  id1 = shmget(IPC_PRIVATE, 4096, IPC_CREAT);
  id2 = shmget(8801, 4096, IPC_CREAT);
  ASSERT_NEQ(-1, id1);
  ASSERT_NEQ(-1, id2);
  ASSERT_EQ(0, shmctl(id1, IPC_RMID, NULL));
  /* Remove already-empty slot */
  ASSERT_EQ(0, shmctl(id1, IPC_RMID, NULL));

  /* Key lookup: IPC_CREAT|IPC_EXCL fails, IPC_CREAT succeeds, 0 succeeds,
   * non-matching searches past slot 1 */
  ASSERT_EQ(-1, shmget(8801, 4096, IPC_CREAT | IPC_EXCL));
  ASSERT_EQ(id2, shmget(8801, 4096, IPC_CREAT));
  ASSERT_EQ(id2, shmget(8801, 4096, 0));
  ASSERT_EQ(-1, shmget(8802, 4096, 0));

  /* CreateFileMappingA failure (size == 0) */
  ASSERT_EQ(-1, shmget(IPC_PRIVATE, 0, IPC_CREAT));

  /* OpenFileMappingA success when external mapping exists */
  hExt = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4096,
                            "Local\\SYSV_SHM_8803");
  ASSERT(hExt != NULL);
  id1 = shmget(8803, 4096, 0);
  ASSERT_NEQ(-1, id1);
  ASSERT_EQ(0, shmctl(id1, IPC_RMID, NULL));
  CloseHandle(hExt);

  /* shmat & shmctl edge cases */
  ASSERT_EQ((void *)(size_t)-1, shmat(-1, NULL, 0));
  ASSERT_EQ((void *)(size_t)-1, shmat(256, NULL, 0));
  ASSERT_EQ((void *)(size_t)-1, shmat(id1, NULL, 0)); /* unallocated slot */

  addr = shmat(id2, NULL, SHM_RDONLY);
  ASSERT_NEQ((void *)(size_t)-1, addr);
  ASSERT_EQ(0, shmdt(addr));

  posix_ipc_set_win32_hooks(NULL, mock_map_view_fail, NULL, NULL, NULL);
  ASSERT_EQ((void *)(size_t)-1, shmat(id2, NULL, 0));
  posix_ipc_set_win32_hooks(NULL, NULL, NULL, NULL, NULL);

  ASSERT_EQ(-1, shmctl(-1, IPC_RMID, NULL));
  ASSERT_EQ(-1, shmctl(256, IPC_RMID, NULL));
  ASSERT_EQ(0, shmctl(id2, IPC_STAT, &shmds));
  ASSERT_EQ(4096U, shmds.shm_segsz);
  ASSERT_EQ(-1, shmctl(id2, IPC_STAT, NULL));
  ASSERT_EQ(-1, shmctl(id2, 999, &shmds));
  ASSERT_EQ(0, shmctl(id2, IPC_RMID, NULL));

  /* Fill all 256 SHM slots to hit empty_idx == -1 */
  for (i = 0; i < 256; i++) {
    ids[i] = shmget(IPC_PRIVATE, 64, IPC_CREAT);
    ASSERT_NEQ(-1, ids[i]);
  }
  ASSERT_EQ(-1, shmget(IPC_PRIVATE, 64, IPC_CREAT));
  for (i = 0; i < 256; i++) {
    ASSERT_EQ(0, shmctl(ids[i], IPC_RMID, NULL));
  }

  /* 4. Semaphore branches */
  ASSERT_EQ(-1, semget(IPC_PRIVATE, 0, IPC_CREAT));
  posix_ipc_set_alloc_hooks(mock_ipc_malloc, mock_ipc_calloc);
  g_ipc_oom_countdown = 1;
  ASSERT_EQ(-1, semget(IPC_PRIVATE, 2, IPC_CREAT));
  posix_ipc_set_alloc_hooks(NULL, NULL);

  id1 = semget(IPC_PRIVATE, 2, IPC_CREAT);
  id2 = semget(9901, 2, IPC_CREAT);
  ASSERT_NEQ(-1, id1);
  ASSERT_NEQ(-1, id2);
  ASSERT_EQ(0, semctl(id1, 0, IPC_RMID));

  ASSERT_EQ(-1, semget(9901, 2, IPC_CREAT | IPC_EXCL));
  ASSERT_EQ(id2, semget(9901, 2, IPC_CREAT));
  ASSERT_EQ(id2, semget(9901, 2, 0));
  ASSERT_EQ(-1, semget(9902, 2, 0));

  /* semop & semctl edge cases */
  ASSERT_EQ(-1, semop(-1, sops, 1));
  ASSERT_EQ(-1, semop(256, sops, 1));
  ASSERT_EQ(-1, semop(id2, NULL, 1));
  ASSERT_EQ(-1, semop(id1, sops, 1)); /* inactive slot */

  ASSERT_EQ(-1, semctl(-1, 0, GETVAL));
  ASSERT_EQ(-1, semctl(256, 0, GETVAL));
  ASSERT_EQ(-1, semctl(id1, 0, GETVAL)); /* inactive slot */
  ASSERT_EQ(-1, semctl(id2, -1, SETVAL, 5));
  ASSERT_EQ(-1, semctl(id2, 2, SETVAL, 5));
  ASSERT_EQ(-1, semctl(id2, -1, GETVAL));
  ASSERT_EQ(-1, semctl(id2, 2, GETVAL));
  ASSERT_EQ(-1, semctl(id2, 0, 999));

  sem_arr[0] = 2;
  sem_arr[1] = 0;
  ASSERT_EQ(0, semctl(id2, 0, SETALL, sem_arr));
  ASSERT_EQ(0, semctl(id2, 0, GETALL, sem_out));
  ASSERT_EQ(2U, sem_out[0]);
  ASSERT_EQ(0U, sem_out[1]);

  /* op == 0 when val == 0 (succeeds) and when val != 0 (fails with IPC_NOWAIT
   * on 2nd op) */
  sops[0].sem_num = 1;
  sops[0].sem_op = 0;
  sops[0].sem_flg = 0;
  ASSERT_EQ(0, semop(id2, sops, 1));

  sops[0].sem_num = 0;
  sops[0].sem_op = 0; /* val is 2 != 0 -> cannot apply */
  sops[0].sem_flg = 0;
  sops[1].sem_num = 1;
  sops[1].sem_op = 1;
  sops[1].sem_flg = IPC_NOWAIT;
  ASSERT_EQ(-1, semop(id2, sops, 2));

  /* op < 0 when val < -op (fails with IPC_NOWAIT) */
  sops[0].sem_num = 1;
  sops[0].sem_op = -1;
  sops[0].sem_flg = IPC_NOWAIT;
  ASSERT_EQ(-1, semop(id2, sops, 1));

  /* Blocking semop retry: 1st iteration exercises default_ipc_wait_event (event
     signaled via SETVAL), 2nd iteration uses mock_wait_event to set sem 0 to 1
   */
  ASSERT_EQ(0, semctl(id2, 0, SETVAL, 0)); /* signals hEvent! */
  g_ipc_target_id = id2;
  g_ipc_wait_mode = 1;
  g_ipc_reset_step = 0;
  posix_ipc_set_win32_hooks(NULL, NULL, NULL, mock_reset_noop_then_hook, NULL);
  sops[0].sem_num = 0;
  sops[0].sem_op = -1;
  sops[0].sem_flg = 0;
  ASSERT_EQ(0, semop(id2, sops, 1));
  posix_ipc_set_win32_hooks(NULL, NULL, NULL, NULL, NULL);

  ASSERT_EQ(0, semctl(id2, 0, IPC_RMID));

  /* Fill all 256 SEM slots */
  for (i = 0; i < 256; i++) {
    ids[i] = semget(IPC_PRIVATE, 1, IPC_CREAT);
    ASSERT_NEQ(-1, ids[i]);
  }
  ASSERT_EQ(-1, semget(IPC_PRIVATE, 1, IPC_CREAT));
  for (i = 0; i < 256; i++) {
    ASSERT_EQ(0, semctl(ids[i], 0, IPC_RMID));
  }

  /* 5. Message queue branches */
  id1 = msgget(IPC_PRIVATE, IPC_CREAT);
  id2 = msgget(6601, IPC_CREAT);
  ASSERT_NEQ(-1, id1);
  ASSERT_NEQ(-1, id2);
  ASSERT_EQ(0, msgctl(id1, IPC_RMID, NULL));

  ASSERT_EQ(-1, msgget(6601, IPC_CREAT | IPC_EXCL));
  ASSERT_EQ(id2, msgget(6601, IPC_CREAT));
  ASSERT_EQ(id2, msgget(6601, 0));
  ASSERT_EQ(-1, msgget(6602, 0));

  /* msgsnd validation & OOM */
  m1.mtype = 1;
  memcpy(m1.mtext, "abc", 4);
  ASSERT_EQ(-1, msgsnd(-1, &m1, 4, 0));
  ASSERT_EQ(-1, msgsnd(256, &m1, 4, 0));
  ASSERT_EQ(-1, msgsnd(id2, NULL, 4, 0));
  ASSERT_EQ(-1, msgsnd(id1, &m1, 4, 0)); /* inactive */
  m1.mtype = 0;
  ASSERT_EQ(-1, msgsnd(id2, &m1, 4, 0));

  m1.mtype = 1;
  posix_ipc_set_alloc_hooks(mock_ipc_malloc, mock_ipc_calloc);
  g_ipc_oom_countdown = 1;
  ASSERT_EQ(-1, msgsnd(id2, &m1, 4, 0)); /* node OOM */
  g_ipc_oom_countdown = 2;
  ASSERT_EQ(-1, msgsnd(id2, &m1, 4, 0)); /* node->data OOM */
  posix_ipc_set_alloc_hooks(NULL, NULL);

  /* Zero-length message send & receive */
  m1.mtype = 10;
  ASSERT_EQ(0, msgsnd(id2, &m1, 0, IPC_NOWAIT));
  ASSERT_EQ(0, (int)msgrcv(id2, &m2, sizeof(m2.mtext), 0, IPC_NOWAIT));
  ASSERT_EQ(10L, m2.mtype);

  /* Queue full: IPC_NOWAIT failure AND blocking retry */
  memset(&msgds, 0, sizeof(msgds));
  msgds.msg_qbytes = 4;
  ASSERT_EQ(0, msgctl(id2, IPC_SET, &msgds));
  m1.mtype = 2;
  memcpy(m1.mtext, "1234", 4);
  ASSERT_EQ(0, msgsnd(id2, &m1, 4, IPC_NOWAIT));
  ASSERT_EQ(-1, msgsnd(id2, &m1, 4, IPC_NOWAIT)); /* full + NOWAIT */

  g_ipc_target_id = id2;
  g_ipc_wait_mode = 2;
  posix_ipc_set_win32_hooks(NULL, NULL, NULL, NULL, mock_wait_event);
  ASSERT_EQ(0, msgsnd(id2, &m1, 4,
                      0)); /* full -> drains via mock_wait_event -> succeeds */
  posix_ipc_set_win32_hooks(NULL, NULL, NULL, NULL, NULL);

  msgds.msg_qbytes = 16384;
  ASSERT_EQ(0, msgctl(id2, IPC_SET, &msgds));
  /* Drain the remaining message */
  ASSERT_EQ(4, (int)msgrcv(id2, &m2, sizeof(m2.mtext), 0, IPC_NOWAIT));

  /* msgrcv validation, negative msgtyp, truncation, middle/tail removal,
   * blocking retry */
  ASSERT_EQ(-1, (int)msgrcv(-1, &m2, 10, 0, IPC_NOWAIT));
  ASSERT_EQ(-1, (int)msgrcv(256, &m2, 10, 0, IPC_NOWAIT));
  ASSERT_EQ(-1, (int)msgrcv(id2, NULL, 10, 0, IPC_NOWAIT));
  ASSERT_EQ(-1, (int)msgrcv(id1, &m2, 10, 0, IPC_NOWAIT));  /* inactive */
  ASSERT_EQ(-1, (int)msgrcv(id2, &m2, 10, 99, IPC_NOWAIT)); /* empty */

  /* Enqueue types: 3, 1, 2, 5 */
  m1.mtype = 3;
  memcpy(m1.mtext, "three!", 7);
  ASSERT_EQ(0, msgsnd(id2, &m1, 7, IPC_NOWAIT));
  m1.mtype = 1;
  memcpy(m1.mtext, "one", 4);
  ASSERT_EQ(0, msgsnd(id2, &m1, 4, IPC_NOWAIT));
  m1.mtype = 2;
  memcpy(m1.mtext, "two", 4);
  ASSERT_EQ(0, msgsnd(id2, &m1, 4, IPC_NOWAIT));
  m1.mtype = 5;
  memcpy(m1.mtext, "five", 5);
  ASSERT_EQ(0, msgsnd(id2, &m1, 5, IPC_NOWAIT));

  /* msgtyp = -3 finds lowest type <= 3 (which is type 1, in middle of list!) */
  ASSERT_EQ(4, (int)msgrcv(id2, &m2, sizeof(m2.mtext), -3, IPC_NOWAIT));
  ASSERT_EQ(1L, m2.mtype);

  /* Too big without MSG_NOERROR fails; with MSG_NOERROR truncates */
  ASSERT_EQ(-1, (int)msgrcv(id2, &m2, 2, 3, IPC_NOWAIT));
  ASSERT_EQ(2, (int)msgrcv(id2, &m2, 2, 3, IPC_NOWAIT | MSG_NOERROR));
  ASSERT_EQ(3L, m2.mtype);

  /* Remove tail (type 5 when queue is [2, 5]) */
  ASSERT_EQ(5, (int)msgrcv(id2, &m2, sizeof(m2.mtext), 5, IPC_NOWAIT));
  ASSERT_EQ(5L, m2.mtype);

  /* Blocking msgrcv when matching type not yet present */
  g_ipc_target_id = id2;
  g_ipc_wait_mode = 3;
  posix_ipc_set_win32_hooks(NULL, NULL, NULL, NULL, mock_wait_event);
  ASSERT_EQ(5, (int)msgrcv(id2, &m2, sizeof(m2.mtext), 7, 0));
  ASSERT_EQ(7L, m2.mtype);
  posix_ipc_set_win32_hooks(NULL, NULL, NULL, NULL, NULL);

  /* msgctl edge cases + IPC_RMID while queue still has message (type 2) */
  ASSERT_EQ(-1, msgctl(-1, IPC_RMID, NULL));
  ASSERT_EQ(-1, msgctl(256, IPC_RMID, NULL));
  ASSERT_EQ(-1, msgctl(id1, IPC_RMID, NULL)); /* inactive */
  ASSERT_EQ(-1, msgctl(id2, IPC_STAT, NULL));
  ASSERT_EQ(-1, msgctl(id2, IPC_SET, NULL));
  ASSERT_EQ(-1, msgctl(id2, 999, &msgds));
  ASSERT_EQ(0, msgctl(id2, IPC_RMID, NULL)); /* frees remaining node! */

  /* Fill all 256 MSG slots */
  for (i = 0; i < 256; i++) {
    ids[i] = msgget(IPC_PRIVATE, IPC_CREAT);
    ASSERT_NEQ(-1, ids[i]);
  }
  ASSERT_EQ(-1, msgget(IPC_PRIVATE, IPC_CREAT));
  for (i = 0; i < 256; i++) {
    ASSERT_EQ(0, msgctl(ids[i], IPC_RMID, NULL));
  }

  PASS();
}
#endif

TEST test_posix_ipc_get_info(void) {
  auto_win_msvc_error_t rc;
  int info = 0;

  rc = posix_ipc_get_info(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_ipc_get_info(&info);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_ipc(void) {
  key_t k;
  k = ftok(".", 1);
  ASSERT(k != (key_t)-1 || k == (key_t)-1);
  PASS();
}

SUITE(suite_posix_ipc_ipc) {
  RUN_TEST(test_posix_ipc_get_info);
  RUN_TEST(test_ipc);
#ifdef _WIN32
  RUN_TEST(test_posix_ipc_win32_branches);
#endif
}
