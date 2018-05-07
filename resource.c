#include "resource.h"
#include <stdio.h>

void initSharedMemory(){
    shmid = shmget(SHM_KEY, sizeof(int)*ACCOUNT_NUM, IPC_CREAT | 0660);
    SYS1(shmid);
}
void initSemaphore(){
    sem.val = 1;
    semid = semget(SEM_KEY, 1, IPC_CREAT | 0660);
    SYS1(semid);
    SYS1(semctl(semid, 0, SETVAL, sem));
    
}
void destroy(){
    shmctl(shmid, IPC_RMID, NULL);
    shmdt(writer);
    shmdt(reader);
}
void down(){
    sem_down.sem_num = 0;
    sem_down.sem_flg = SEM_UNDO;
    sem_down.sem_op = -1;
    SYS1(semop(semid, &sem_down, 1));
    fprintf(stdout, "finish down\n");
}
void up(){
    sem_up.sem_num = 0;
    sem_up.sem_flg = SEM_UNDO;
    sem_up.sem_op = 1;
    SYS1(semop(semid, &sem_up, 1));
    fprintf(stdout, "finish up\n");
}

int* getWriter(){
    return writer;
}

int* getReader(){
    return reader;
}