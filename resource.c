#include "resource.h"
#include <stdio.h>



void initSharedMemory()
{
    shmid = shmget(SHM_KEY, sizeof(int) * ACCOUNT_NUM, IPC_CREAT | 0660);
    accountBook = (int *)shmat(shmid, NULL, SHM_W);
    SYS1(*accountBook);
    SYS1(shmid);
}
void initSemaphore()
{
    sem.val = 1;
    semid = semget(SEM_KEY, 1, IPC_CREAT | 0660);
    SYS1(semid);
    SYS1(semctl(semid, 0, SETVAL, sem));
}
void destroy()
{
    initSharedMemory();
    initSemaphore();
    SYS1(shmctl(shmid, IPC_RMID, NULL));
    SYS1(shmdt(accountBook));
    SYS1(semctl(semid, 0, IPC_RMID, sem));
}
void down()
{
    initSemaphore();
    sem_down.sem_num = 0;
    sem_down.sem_flg = SEM_UNDO;
    sem_down.sem_op = -1;
    SYS1(semop(semid, &sem_down, 1));
}
void up()
{
    initSemaphore();
    sem_up.sem_num = 0;
    sem_up.sem_flg = SEM_UNDO;
    sem_up.sem_op = 1;
    SYS1(semop(semid, &sem_up, 1));
}

int *getAccountBook()
{
    initSharedMemory();
    return accountBook;
}

void displayAccountBook(int size){
    initSharedMemory();
    for(int* ptr = accountBook; ptr < accountBook+size; ptr++){
        printf("%d\n", *ptr);
    }
}

