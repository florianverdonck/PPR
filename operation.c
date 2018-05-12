#include "resource.h"
#include "operation.h"

void depositOrWithdrawal(int account, int amount){
    down();
    printf("%d -> %d\n", account, amount);
    int* ptr = getAccountBook()+account;
    *ptr += (amount); 
    up();
}

void depositOrWithdrawalWithoutSem(int account, int amount){
    printf("%d -> %d\n", account, amount);
    int* ptr = getAccountBook()+account;
    *ptr += (amount);
}

void displayAccountBook(int size)
{
    retrieveSharedMemory();
    for (int *ptr = accountBook; ptr < accountBook + size; ptr++)
    {
        printf("%ld : %d\n", ptr - accountBook, *ptr);
    }
}
