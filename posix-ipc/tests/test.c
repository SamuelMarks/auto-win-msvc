#include "greatest.h"
#include "posix-ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning(push, 3)
#pragma warning(disable : 4127) /* conditional expression is constant */
#pragma warning(                                                               \
    disable : 4201) /* nonstandard extension used : nameless struct/union */
#pragma warning(disable : 4214) /* nonstandard extension used : bit field      \
                                   types other than int */
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#if defined(_M_AMD64) && !defined(_AMD64_)
#define _AMD64_
#elif defined(_M_IX86) && !defined(_X86_)
#define _X86_
#elif defined(_M_ARM64) && !defined(_ARM64_)
#define _ARM64_
#elif defined(_M_ARM) && !defined(_ARM_)
#define _ARM_
#endif
#include <windows.h>

#else
#include <unistd.h>
#endif

TEST test_ftok(void) {
  key_t k1, k2;
  FILE *f = NULL;
#ifdef _MSC_VER
  fopen_s(&f, "test_ftok.txt", "w");
#else
  f = fopen("test_ftok.txt", "w");
#endif
  if (f) {
    fputs("test", f);
    fclose(f);
  }

  k1 = ftok("test_ftok.txt", 1);
  k2 = ftok("test_ftok.txt", 1);
  ASSERT(k1 != (key_t)-1);
  ASSERT_EQ(k1, k2);

  k2 = ftok("test_ftok.txt", 2);
  ASSERT(k1 != k2);

  remove("test_ftok.txt");
  PASS();
}

TEST test_shm(void) {
  int shmid;
  void *addr;
  struct shmid_ds ds;
#ifdef __CYGWIN__
  SKIP();
#endif

  shmid = shmget(IPC_PRIVATE, 1024, IPC_CREAT | 0666);
  ASSERT(shmid >= 0);

  addr = shmat(shmid, NULL, 0);
  ASSERT(addr != (void *)-1);

#ifdef _MSC_VER
  strcpy_s((char *)addr, 1024, "Hello Shared Memory");
#else
  strcpy((char *)addr, "Hello Shared Memory");
#endif
  ASSERT_STR_EQ("Hello Shared Memory", (char *)addr);

  ASSERT_EQ(0, shmctl(shmid, IPC_STAT, &ds));
  ASSERT_EQ(1024, ds.shm_segsz);

  ASSERT_EQ(0, shmdt(addr));
  ASSERT_EQ(0, shmctl(shmid, IPC_RMID, NULL));

  PASS();
}

TEST test_sem(void) {
  int semid;
  struct sembuf sb;
  unsigned short vals[2];
#ifdef __CYGWIN__
  SKIP();
#endif

  semid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
  ASSERT(semid >= 0);

  ASSERT_EQ(0, semctl(semid, 0, SETVAL, 5));
  ASSERT_EQ(5, semctl(semid, 0, GETVAL));

  vals[0] = 10;
  vals[1] = 20;
  ASSERT_EQ(0, semctl(semid, 0, SETALL, vals));
  ASSERT_EQ(10, semctl(semid, 0, GETVAL));
  ASSERT_EQ(20, semctl(semid, 1, GETVAL));

  vals[0] = 0;
  vals[1] = 0;
  ASSERT_EQ(0, semctl(semid, 0, GETALL, vals));
  ASSERT_EQ(10, vals[0]);
  ASSERT_EQ(20, vals[1]);

  sb.sem_num = 0;
  sb.sem_op = -5;
  sb.sem_flg = 0;
  ASSERT_EQ(0, semop(semid, &sb, 1));
  ASSERT_EQ(5, semctl(semid, 0, GETVAL));

  ASSERT_EQ(0, semctl(semid, 0, IPC_RMID));

  PASS();
}

struct msgbuf {
  long mtype;
  char mtext[100];
};

TEST test_msg(void) {
  int msqid;
  struct msgbuf snd, rcv;
  struct msqid_ds ds;
  ssize_t rcv_sz;
#ifdef __CYGWIN__
  SKIP();
#endif

  msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
  ASSERT(msqid >= 0);

  snd.mtype = 1;
#ifdef _MSC_VER
  strcpy_s(snd.mtext, sizeof(snd.mtext), "Message 1");
#else
  strcpy(snd.mtext, "Message 1");
#endif
  ASSERT_EQ(0, msgsnd(msqid, &snd, strlen(snd.mtext) + 1, 0));

  snd.mtype = 2;
#ifdef _MSC_VER
  strcpy_s(snd.mtext, sizeof(snd.mtext), "Message 2");
#else
  strcpy(snd.mtext, "Message 2");
#endif
  ASSERT_EQ(0, msgsnd(msqid, &snd, strlen(snd.mtext) + 1, 0));

  ASSERT_EQ(0, msgctl(msqid, IPC_STAT, &ds));
  ASSERT(ds.msg_cbytes > 0);

  rcv_sz = msgrcv(msqid, &rcv, sizeof(rcv.mtext), 2, 0);
  ASSERT(rcv_sz > 0);
  ASSERT_EQ(2, rcv.mtype);
  ASSERT_STR_EQ("Message 2", rcv.mtext);

  rcv_sz = msgrcv(msqid, &rcv, sizeof(rcv.mtext), 1, 0);
  ASSERT(rcv_sz > 0);
  ASSERT_EQ(1, rcv.mtype);
  ASSERT_STR_EQ("Message 1", rcv.mtext);

  ASSERT_EQ(0, msgctl(msqid, IPC_RMID, NULL));

  PASS();
}

SUITE(ipc_suite) {
  RUN_TEST(test_ftok);
  RUN_TEST(test_shm);
  RUN_TEST(test_sem);
  RUN_TEST(test_msg);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(ipc_suite);
  GREATEST_MAIN_END();
}
