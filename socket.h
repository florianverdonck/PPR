#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFLEN  80


#if !defined (_SOCKET_H_)
#define _SOCKET_H_

static char buffer[BUFLEN];

void initServer(int);

void initConnection();

#endif