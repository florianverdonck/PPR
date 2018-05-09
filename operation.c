#include "resource.h"
#include "operation.h"

void depositOrWithdrawal(int account, int amount){
    down();
fprintf(stdout, "debut deposit\n");
    int* ptr = getAccountBook()+account;
	printf("%p\n", ptr);
    *ptr += (amount); 
    fprintf(stdout, "finish deposit\n");
    up();
}
