#include "ipc.h"

void initSharedMemory(){
    shmid = shmget(SHM_KEY, sizeof(int)*ACCOUNT_NUM, IPC_CREAT | 0660);
    SYS1(semid);

    writer = (int *)shmat(shmid, NULL, SHM_W);
    SYS1(*writer);

    reader = (int *)shmat(shmid, NULL, SHM_R);
    SYS1(*reader);
}
void initSemaphore(){
    sem.val = 1;

    sem_down.sem_num = 0;
    sem_down.sem_flg = SEM_UNDO;
    sem_down.sem_op = -1;

    sem_up.sem_num = 0;
    sem_up.sem_flg = SEM_UNDO;
    sem_up.sem_op = 1;

    semid = semget(SEM_KEY, 1, IPC_CREAT | 0660);
    SYS1(semid);
    SYS1(semctl(semid, 0, SETVAL, sem));
    
}
void down(){
    SYS1(semop(semid, &sem_down, 1));
}
void up(){
    SYS1(semop(semid, &sem_up, 1));
}

int* getWriter(){
    return writer;
}

int* getReader(){
    return reader;
}