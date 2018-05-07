#include "resource.h"
#include "operation.h"

void depositOrWithdrawal(int account, int amount){
    down();
    writer = (int *)shmat(shmid, NULL, SHM_W);
    SYS1(*writer);
    int* ptr = writer+account;
    *ptr += (amount); 
    fprintf(stdout, "finish deposit\n");
    up();
}