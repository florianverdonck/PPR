#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#define ACCOUNT_NUM 1000
#define SHM_KEY 1111
#define SEM_KEY 2222
#define SYS1(call) (call == -1 ? perror("SYSCALL ERROR"), exit(5) : 0)

#if !defined (_IPC_H_)
#define _IPC_H_

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

union semun sem;
struct sembuf sem_down;
struct sembuf sem_up;

int semid;
int shmid;

int* writer;
int* reader;

void iniiSharedMemory();
void initSemaphore();
void down();
void up();
int* getWriter();
int* getReader();

#endif