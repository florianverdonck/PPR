#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "config.h"
#include "operation.h"
#include "resource.h"
#define BUFLEN  80
#define TRUE 1
#define FALSE 0

void my_handler(int signal);
int listening;

int main(int argc, char* argv[]) {
    if(argc != 2) {
        perror("Usage: ./server port");
        exit(1);
    }
    listening = TRUE;
    // Handling ^C
    struct sigaction sa;
    sa.sa_handler = &my_handler;
    sa.sa_flags = 0;
    SYS(sigfillset(&sa.sa_mask), "Error sigfillset");

    SYS(sigaction(SIGINT, &sa, NULL), "Error sigaction");
    


    // Socket
	int sck;
    struct sockaddr_in addr;

    if( (sck = socket(AF_INET,SOCK_STREAM,0)) < 0 )
    {   perror("server - socket");
        exit(1);
    }

    bzero((char*)&addr,sizeof(struct sockaddr_in));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(atoi(argv[1]));

    if( bind(sck, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
    {   perror("server - bind");
        exit(1);
    }

    fprintf(stderr,"Start listening for connections\n");
    listen(sck,1);
    int sck2;
    while(listening){
        struct sockaddr_in addr2;
        u_int len2 = sizeof(addr2);
        sck2 = accept(sck,(struct sockaddr *)&addr2,&len2);
        if(sck2 == -1) continue;
        fprintf(stderr,"Connection from %s %d\n",inet_ntoa(addr2.sin_addr), ntohs(addr2.sin_port));
        FILE *netFd = fdopen(sck2,"r");
        Transfer transfer;
        size_t res;
        while((res = fread(&transfer, sizeof(transfer), 1, netFd)) == 1){
            down();
            depositOrWithdrawalWithoutSem(transfer.source, -(transfer.amount));
            depositOrWithdrawalWithoutSem(transfer.destination, transfer.amount);
            up();
        }
        SYS(close(sck2), "Error server close");
    }
    printf("Exit\n");
    exit(0);
}

void my_handler(int signal){
    if(signal == SIGINT){
        listening = FALSE;
    }
}