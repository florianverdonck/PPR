#include "resource.h"
#include <stdio.h>

void initSharedMemory()
{
    shmid = shmget(SHM_KEY, sizeof(int) * ACCOUNT_NUM, IPC_CREAT | 0660);
    SYS(shmid, "Error shmget");
    accountBook = (int *)shmat(shmid, NULL, SHM_W);
    SYS(*accountBook, "Error shmat");
}
void initSemaphore()
{
    sem.val = 1;
    semid = semget(SEM_KEY, 1, IPC_CREAT | 0660);
    SYS(semid, "Error semget");
    SYS(semctl(semid, 0, SETVAL, sem), "Error semctl");
}
void retrieveSharedMemory()
{
    shmid = shmget(SHM_KEY, sizeof(int) * ACCOUNT_NUM, 0660);
    SYS(shmid, "Error shmid");
    accountBook = (int *)shmat(shmid, NULL, SHM_W);
    SYS(*accountBook, "Error shmat");
}
void retrieveSemaphore()
{
    semid = semget(SEM_KEY, 1, 0660);
    SYS(semid, "Error semget");
    SYS(semctl(semid, 0, GETVAL, sem), "Error semctl");
}
void destroy()
{
    retrieveSharedMemory();
    retrieveSemaphore();
    SYS(shmctl(shmid, IPC_RMID, NULL), "Error semctl");
    SYS(shmdt(accountBook), "Error shmdt");
    SYS(semctl(semid, 0, IPC_RMID, sem), "Error semctl");
}
void down()
{
    retrieveSemaphore();
    sem_down.sem_num = 0;
    sem_down.sem_flg = SEM_UNDO;
    sem_down.sem_op = -1;
    SYS(semop(semid, &sem_down, 1), "Error semop");
}
void up()
{
    retrieveSemaphore();
    sem_up.sem_num = 0;
    sem_up.sem_flg = SEM_UNDO;
    sem_up.sem_op = 1;
    SYS(semop(semid, &sem_up, 1), "Error semop");
}

int *getAccountBook()
{
    retrieveSharedMemory();
    return accountBook;
}
