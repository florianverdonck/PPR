#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "operation.h"

int main(int argc, char* args[]) {
	if(argc < 2 || argc > 3) {
		perror("pdr account amount");
		exit(1);
	}
	if(argc == 2){
		displayAccountBook(atoi(args[1]));
	}else{
	depositOrWithdrawal(atoi(args[1]), atoi(args[2]));
	}

}
