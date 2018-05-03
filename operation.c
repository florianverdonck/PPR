#include "ipc.h"
#include "operation.h"

void depositOrWithdrawal(int account, int amount){
    down();
    int* ptr = writer+account;
    *ptr += (amount); 
    up();
}