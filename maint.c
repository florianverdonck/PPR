#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "resource.h"

int main(int argc, char* args[]) {
	if(argc < 2) {
		perror("maint type [opt]");
		exit(1);
	}
	int type = atoi(args[1]);
	if(argc < 3 && type == 3) {
		perror("maint 3 opt");
		exit(1);
	}
	
	if(type == 1){
		initSharedMemory();
		initSemaphore();
	} else if(type == 2){
		destroy();
	} else {
		down();
		sleep(atoi(args[2]));
		up();
	}
}
