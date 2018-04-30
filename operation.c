#include "ipc.h"
#include "operation.h"

void deposit(int account, int amount){
    down();
    int* ptr = writer+account;
    *ptr += amount; 
    up();
}

void withdrawal(int account, int amount){
    down();
    int* ptr = writer+account;
    *ptr -= amount; 
    up();
}