#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-ipc.h"
#include <string.h>
/* clang-format on */

struct my_msgbuf {
  long mtype;
  char mtext[32];
};

TEST test_msg_queue(void) {
  int msqid;
  struct my_msgbuf smsg;
  struct my_msgbuf rmsg;
  struct msqid_ds ds;
  int res;
  ssize_t ssz;

  /* Invalid msgget */
  ASSERT_EQ(-1, msgget(-999, 0));

  /* Create private queue */
  msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
  ASSERT_NEQ(-1, msqid);

  /* Status */
  memset(&ds, 0, sizeof(ds));
  res = msgctl(msqid, IPC_STAT, &ds);
  ASSERT(res == 0 || res == -1);

  /* Set */
  res = msgctl(msqid, IPC_SET, &ds);
  ASSERT(res == 0 || res == -1);

  /* Send message */
  smsg.mtype = 1;
  memcpy(smsg.mtext, "hello_msg", 10);
  res = msgsnd(msqid, &smsg, 10, IPC_NOWAIT);
  ASSERT(res == 0 || res == -1);

  /* Receive message */
  memset(&rmsg, 0, sizeof(rmsg));
  ssz = msgrcv(msqid, &rmsg, sizeof(rmsg.mtext), 1, IPC_NOWAIT);
  ASSERT(ssz >= 0 || ssz == -1);

  /* Remove queue */
  res = msgctl(msqid, IPC_RMID, NULL);
  ASSERT(res == 0 || res == -1);

  PASS();
}

SUITE(suite_posix_ipc_msgctl) { RUN_TEST(test_msg_queue); }
