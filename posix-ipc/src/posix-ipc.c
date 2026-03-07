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
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4201) /* nameless struct/union */
#pragma warning(disable: 4214) /* bit field types other than int */
#pragma warning(disable: 4244) /* possible loss of data */
#endif
#include <windef.h>
#include <winbase.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "posix-ipc.h"

/* Helper to generate a consistent hash from a file */
key_t ftok(const char *path, int id) {
    HANDLE hFile;
    BY_HANDLE_FILE_INFORMATION fileInfo;
    key_t key = (key_t)-1;

    if (!path) return (key_t)-1;

    hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return (key_t)-1;
    }

    if (GetFileInformationByHandle(hFile, &fileInfo)) {
        key = (key_t)id << 24;
        key |= (key_t)(fileInfo.nFileIndexLow & 0xFFFF);
        key |= (key_t)((fileInfo.nFileIndexHigh & 0xFF) << 16);
    }

    CloseHandle(hFile);
    return key;
}

#ifdef _MSC_VER
#define NUM_FORMAT "%d"
#define LU_FORMAT "%lu"
#else
#define NUM_FORMAT "%d"
#define LU_FORMAT "%lu"
#endif

/* --------------------------------------------------------------------------
 * Shared Memory
 * -------------------------------------------------------------------------- */
#define MAX_SHM 256
static struct {
    HANDLE hMap;
    key_t key;
    size_t size;
} g_shm[MAX_SHM];

static LONG g_shm_lock = 0;

static void lock_shm(void) {
    while (InterlockedCompareExchange(&g_shm_lock, 1, 0) != 0) {
        Sleep(0);
    }
}
static void unlock_shm(void) {
    InterlockedExchange(&g_shm_lock, 0);
}

int shmget(key_t key, size_t size, int shmflg) {
    char name[256];
    HANDLE hMap;
    int i, empty_idx = -1;
    
    lock_shm();
    if (key != IPC_PRIVATE) {
        for (i = 0; i < MAX_SHM; i++) {
            if (g_shm[i].hMap != NULL && g_shm[i].key == key) {
                if ((shmflg & IPC_CREAT) && (shmflg & IPC_EXCL)) {
                    unlock_shm();
                    return -1;
                }
                unlock_shm();
                return i;
            }
        }
    }
    
    for (i = 0; i < MAX_SHM; i++) {
        if (g_shm[i].hMap == NULL) {
            empty_idx = i;
            break;
        }
    }
    
    if (empty_idx == -1) {
        unlock_shm();
        return -1;
    }
    
    if (key == IPC_PRIVATE) {
        static int priv_count = 0;
        #ifdef _MSC_VER
        sprintf_s(name, sizeof(name), "Local\\SYSV_SHM_PRIV_" LU_FORMAT "_" NUM_FORMAT, (unsigned long)GetCurrentProcessId(), priv_count++);
#else
        sprintf(name, "Local\\SYSV_SHM_PRIV_" LU_FORMAT "_" NUM_FORMAT, (unsigned long)GetCurrentProcessId(), priv_count++);
#endif
    } else {
        #ifdef _MSC_VER
        sprintf_s(name, sizeof(name), "Local\\SYSV_SHM_" NUM_FORMAT, (int)key);
#else
        sprintf(name, "Local\\SYSV_SHM_" NUM_FORMAT, (int)key);
#endif
    }
    
    if (shmflg & IPC_CREAT) {
        hMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (DWORD)size, name);
    } else {
        hMap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name);
    }
    
    if (!hMap) {
        unlock_shm();
        return -1;
    }
    
    g_shm[empty_idx].hMap = hMap;
    g_shm[empty_idx].key = key;
    g_shm[empty_idx].size = size;
    unlock_shm();
    
    return empty_idx;
}

void *shmat(int shmid, const void *shmaddr, int shmflg) {
    HANDLE hMap;
    void *addr;
    DWORD access = FILE_MAP_ALL_ACCESS;
    
    (void)shmaddr; /* Suppress unused warning */
    
    if (shmid < 0 || shmid >= MAX_SHM) return (void *)-1;
    
    lock_shm();
    hMap = g_shm[shmid].hMap;
    unlock_shm();
    
    if (!hMap) return (void *)-1;
    
    if (shmflg & SHM_RDONLY) {
        access = FILE_MAP_READ;
    }
    
    addr = MapViewOfFile(hMap, access, 0, 0, 0);
    if (!addr) return (void *)-1;
    
    return addr;
}

int shmdt(const void *shmaddr) {
    if (UnmapViewOfFile(shmaddr)) {
        return 0;
    }
    return -1;
}

int shmctl(int shmid, int cmd, struct shmid_ds *buf) {
    if (shmid < 0 || shmid >= MAX_SHM) return -1;
    
    if (cmd == IPC_RMID) {
        lock_shm();
        if (g_shm[shmid].hMap) {
            CloseHandle(g_shm[shmid].hMap);
            g_shm[shmid].hMap = NULL;
            g_shm[shmid].key = 0;
            g_shm[shmid].size = 0;
        }
        unlock_shm();
        return 0;
    } else if (cmd == IPC_STAT) {
        if (buf) {
            memset(buf, 0, sizeof(struct shmid_ds));
            lock_shm();
            buf->shm_segsz = g_shm[shmid].size;
            unlock_shm();
            return 0;
        }
    }
    return -1;
}

/* --------------------------------------------------------------------------
 * Semaphores
 * -------------------------------------------------------------------------- */
#define MAX_SEM 256
struct sysv_sem {
    HANDLE hMutex;
    HANDLE hEvent;
    key_t key;
    int nsems;
    short *semvals;
};
static struct sysv_sem g_sems[MAX_SEM];
static LONG g_sem_lock = 0;

static void lock_sem(void) {
    while (InterlockedCompareExchange(&g_sem_lock, 1, 0) != 0) {
        Sleep(0);
    }
}
static void unlock_sem(void) {
    InterlockedExchange(&g_sem_lock, 0);
}

int semget(key_t key, int nsems, int semflg) {
    int i, empty_idx = -1;
    
    lock_sem();
    if (key != IPC_PRIVATE) {
        for (i = 0; i < MAX_SEM; i++) {
            if (g_sems[i].hMutex != NULL && g_sems[i].key == key) {
                if ((semflg & IPC_CREAT) && (semflg & IPC_EXCL)) {
                    unlock_sem();
                    return -1;
                }
                unlock_sem();
                return i;
            }
        }
    }
    
    for (i = 0; i < MAX_SEM; i++) {
        if (g_sems[i].hMutex == NULL) {
            empty_idx = i;
            break;
        }
    }
    
    if (empty_idx == -1) {
        unlock_sem();
        return -1;
    }
    
    g_sems[empty_idx].hMutex = CreateMutexA(NULL, FALSE, NULL);
    g_sems[empty_idx].hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
    g_sems[empty_idx].key = key;
    g_sems[empty_idx].nsems = nsems;
    g_sems[empty_idx].semvals = (short *)calloc((size_t)nsems, sizeof(short));
    
    unlock_sem();
    return empty_idx;
}

int semop(int semid, struct sembuf *sops, size_t nsops) {
    struct sysv_sem *sem;
    size_t i;
    int can_apply;

    if (semid < 0 || semid >= MAX_SEM) return -1;
    
    sem = &g_sems[semid];
    if (!sem->hMutex) return -1;

retry:
    WaitForSingleObject(sem->hMutex, INFINITE);
    
    can_apply = 1;
    for (i = 0; i < nsops; i++) {
        short val = sem->semvals[sops[i].sem_num];
        short op = sops[i].sem_op;
        
        if (op < 0) {
            if (val < -op) {
                can_apply = 0;
                break;
            }
        } else if (op == 0) {
            if (val != 0) {
                can_apply = 0;
                break;
            }
        }
    }
    
    if (can_apply) {
        for (i = 0; i < nsops; i++) {
            sem->semvals[sops[i].sem_num] = (short)(sem->semvals[sops[i].sem_num] + sops[i].sem_op);
        }
        SetEvent(sem->hEvent); /* wake up waiters */
        ReleaseMutex(sem->hMutex);
        return 0;
    } else {
        short has_nowait = 0;
        for (i = 0; i < nsops; i++) {
            if (sops[i].sem_flg & IPC_NOWAIT) {
                has_nowait = 1;
                break;
            }
        }
        
        if (has_nowait) {
            ReleaseMutex(sem->hMutex);
            return -1; /* would block */
        }
        
        ResetEvent(sem->hEvent);
        ReleaseMutex(sem->hMutex);
        
        WaitForSingleObject(sem->hEvent, INFINITE);
        goto retry;
    }
}

int semctl(int semid, int semnum, int cmd, ...) {
    struct sysv_sem *sem;
    va_list ap;
    int res = -1;
    union {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;
    
    if (semid < 0 || semid >= MAX_SEM) return -1;
    
    sem = &g_sems[semid];
    if (!sem->hMutex) return -1;
    
    va_start(ap, cmd);
    
    switch (cmd) {
        case SETVAL:
            arg.val = va_arg(ap, int);
            WaitForSingleObject(sem->hMutex, INFINITE);
            if (semnum >= 0 && semnum < sem->nsems) {
                sem->semvals[semnum] = (short)arg.val;
                SetEvent(sem->hEvent);
                res = 0;
            }
            ReleaseMutex(sem->hMutex);
            break;
            
        case GETVAL:
            WaitForSingleObject(sem->hMutex, INFINITE);
            if (semnum >= 0 && semnum < sem->nsems) {
                res = sem->semvals[semnum];
            }
            ReleaseMutex(sem->hMutex);
            break;
            
        case SETALL:
            arg.array = va_arg(ap, unsigned short *);
            WaitForSingleObject(sem->hMutex, INFINITE);
            {
                int i;
                for (i = 0; i < sem->nsems; i++) {
                    sem->semvals[i] = (short)arg.array[i];
                }
            }
            SetEvent(sem->hEvent);
            res = 0;
            ReleaseMutex(sem->hMutex);
            break;
            
        case GETALL:
            arg.array = va_arg(ap, unsigned short *);
            WaitForSingleObject(sem->hMutex, INFINITE);
            {
                int i;
                for (i = 0; i < sem->nsems; i++) {
                    arg.array[i] = (unsigned short)sem->semvals[i];
                }
            }
            res = 0;
            ReleaseMutex(sem->hMutex);
            break;
            
        case IPC_RMID:
            lock_sem();
            if (sem->hMutex) {
                CloseHandle(sem->hMutex);
                CloseHandle(sem->hEvent);
                free(sem->semvals);
                sem->hMutex = NULL;
                sem->hEvent = NULL;
                sem->semvals = NULL;
                sem->nsems = 0;
                sem->key = 0;
            }
            unlock_sem();
            res = 0;
            break;
            
        default:
            res = -1;
            break;
    }
    
    va_end(ap);
    return res;
}

/* --------------------------------------------------------------------------
 * Message Queues
 * -------------------------------------------------------------------------- */
#define MAX_MSG 256
struct msg_node {
    long msgtyp;
    void *data;
    size_t size;
    struct msg_node *next;
};

struct sysv_msg {
    HANDLE hMutex;
    HANDLE hEvent;
    key_t key;
    struct msg_node *head;
    struct msg_node *tail;
    size_t qbytes;
    size_t cbytes;
};

static struct sysv_msg g_msgs[MAX_MSG];
static LONG g_msg_lock = 0;

static void lock_msg(void) {
    while (InterlockedCompareExchange(&g_msg_lock, 1, 0) != 0) {
        Sleep(0);
    }
}
static void unlock_msg(void) {
    InterlockedExchange(&g_msg_lock, 0);
}

int msgget(key_t key, int msgflg) {
    int i, empty_idx = -1;
    
    lock_msg();
    if (key != IPC_PRIVATE) {
        for (i = 0; i < MAX_MSG; i++) {
            if (g_msgs[i].hMutex != NULL && g_msgs[i].key == key) {
                if ((msgflg & IPC_CREAT) && (msgflg & IPC_EXCL)) {
                    unlock_msg();
                    return -1;
                }
                unlock_msg();
                return i;
            }
        }
    }
    
    for (i = 0; i < MAX_MSG; i++) {
        if (g_msgs[i].hMutex == NULL) {
            empty_idx = i;
            break;
        }
    }
    
    if (empty_idx == -1) {
        unlock_msg();
        return -1;
    }
    
    g_msgs[empty_idx].hMutex = CreateMutexA(NULL, FALSE, NULL);
    g_msgs[empty_idx].hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
    g_msgs[empty_idx].key = key;
    g_msgs[empty_idx].head = NULL;
    g_msgs[empty_idx].tail = NULL;
    g_msgs[empty_idx].qbytes = 16384; /* default max bytes */
    g_msgs[empty_idx].cbytes = 0;
    
    unlock_msg();
    return empty_idx;
}

int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg) {
    struct sysv_msg *mq;
    struct msg_node *node;
    long mtype;
    
    if (msqid < 0 || msqid >= MAX_MSG) return -1;
    
    mq = &g_msgs[msqid];
    if (!mq->hMutex) return -1;
    
    mtype = *(const long *)msgp;
    if (mtype <= 0) return -1;
    
    node = (struct msg_node *)malloc(sizeof(struct msg_node));
    if (!node) return -1;
    
    node->msgtyp = mtype;
    node->size = msgsz;
    node->data = malloc(msgsz);
    if (!node->data && msgsz > 0) {
        free(node);
        return -1;
    }
    
    if (msgsz > 0) {
#ifdef _MSC_VER
        memcpy_s(node->data, msgsz, (const char *)msgp + sizeof(long), msgsz);
#else
        memcpy(node->data, (const char *)msgp + sizeof(long), msgsz);
#endif
    }
    node->next = NULL;

retry:
    WaitForSingleObject(mq->hMutex, INFINITE);
    
    if (mq->cbytes + msgsz > mq->qbytes) {
        if (msgflg & IPC_NOWAIT) {
            ReleaseMutex(mq->hMutex);
            free(node->data);
            free(node);
            return -1;
        }
        
        ResetEvent(mq->hEvent);
        ReleaseMutex(mq->hMutex);
        WaitForSingleObject(mq->hEvent, INFINITE);
        goto retry;
    }
    
    if (mq->tail) {
        mq->tail->next = node;
    } else {
        mq->head = node;
    }
    mq->tail = node;
    mq->cbytes += msgsz;
    
    SetEvent(mq->hEvent); /* wake up receivers */
    ReleaseMutex(mq->hMutex);
    
    return 0;
}

ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg) {
    struct sysv_msg *mq;
    struct msg_node *prev, *curr, *found_prev, *found;
    size_t copy_size;
    
    if (msqid < 0 || msqid >= MAX_MSG) return -1;
    
    mq = &g_msgs[msqid];
    if (!mq->hMutex) return -1;

retry:
    WaitForSingleObject(mq->hMutex, INFINITE);
    
    prev = NULL;
    curr = mq->head;
    found = NULL;
    found_prev = NULL;
    
    while (curr) {
        if (msgtyp == 0 || 
            (msgtyp > 0 && curr->msgtyp == msgtyp) || 
            (msgtyp < 0 && curr->msgtyp <= -msgtyp)) {
            
            if (msgtyp < 0 && found != NULL && curr->msgtyp >= found->msgtyp) {
                /* For negative msgtyp, find lowest type <= |msgtyp| */
                prev = curr;
                curr = curr->next;
                continue;
            }
            
            found = curr;
            found_prev = prev;
            
            if (msgtyp >= 0) break;
        }
        prev = curr;
        curr = curr->next;
    }
    
    if (!found) {
        if (msgflg & IPC_NOWAIT) {
            ReleaseMutex(mq->hMutex);
            return -1;
        }
        
        ResetEvent(mq->hEvent);
        ReleaseMutex(mq->hMutex);
        WaitForSingleObject(mq->hEvent, INFINITE);
        goto retry;
    }
    
    /* We found a message */
    if (found->size > msgsz && !(msgflg & MSG_NOERROR)) {
        ReleaseMutex(mq->hMutex);
        /* errno = E2BIG; */
        return -1;
    }
    
    copy_size = found->size > msgsz ? msgsz : found->size;
    
    *(long *)msgp = found->msgtyp;
    if (copy_size > 0) {
#ifdef _MSC_VER
        memcpy_s((char *)msgp + sizeof(long), msgsz, found->data, copy_size);
#else
        memcpy((char *)msgp + sizeof(long), found->data, copy_size);
#endif
    }
    
    /* Remove from queue */
    if (found_prev) {
        found_prev->next = found->next;
    } else {
        mq->head = found->next;
    }
    if (mq->tail == found) {
        mq->tail = found_prev;
    }
    
    mq->cbytes -= found->size;
    
    SetEvent(mq->hEvent); /* wake up senders */
    ReleaseMutex(mq->hMutex);
    
    free(found->data);
    free(found);
    
    return (ssize_t)copy_size;
}

int msgctl(int msqid, int cmd, struct msqid_ds *buf) {
    struct sysv_msg *mq;
    
    if (msqid < 0 || msqid >= MAX_MSG) return -1;
    
    mq = &g_msgs[msqid];
    if (!mq->hMutex) return -1;
    
    if (cmd == IPC_RMID) {
        struct msg_node *curr, *next;
        
        lock_msg();
        WaitForSingleObject(mq->hMutex, INFINITE);
        
        curr = mq->head;
        while (curr) {
            next = curr->next;
            free(curr->data);
            free(curr);
            curr = next;
        }
        
        CloseHandle(mq->hMutex);
        CloseHandle(mq->hEvent);
        mq->hMutex = NULL;
        mq->hEvent = NULL;
        mq->head = NULL;
        mq->tail = NULL;
        mq->key = 0;
        
        unlock_msg();
        return 0;
    } else if (cmd == IPC_STAT) {
        if (buf) {
            WaitForSingleObject(mq->hMutex, INFINITE);
            memset(buf, 0, sizeof(struct msqid_ds));
            buf->msg_qbytes = (unsigned long)mq->qbytes;
            buf->msg_cbytes = (unsigned long)mq->cbytes;
            ReleaseMutex(mq->hMutex);
            return 0;
        }
    } else if (cmd == IPC_SET) {
        if (buf) {
            WaitForSingleObject(mq->hMutex, INFINITE);
            mq->qbytes = (size_t)buf->msg_qbytes;
            ReleaseMutex(mq->hMutex);
            return 0;
        }
    }
    return -1;
}
#endif /* _WIN32 */
typedef int dummy_translation_unit;
