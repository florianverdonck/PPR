#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#define ACCOUNT_NUM 1000
#define SHM_KEY 1111
#define SEM_KEY 2222

#if !defined (_RESOURCE_H_)
#define _RESOURCE_H_

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

int* accountBook;

void initSharedMemory();
void initSemaphore();
void retrieveSharedMemory();
void retrieveSemaphore();
void destroy();
void down();
void up();
int* getAccountBook();
void displayAccountBook(int size);

#endif
