#include    <netdb.h>
#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    <sys/socket.h>
#include    <sys/types.h>
#include    <netinet/in.h>
#include	<arpa/inet.h>

#if !defined (_SOCKET_H_)
#define _SOCKET_H_

void initServer();

void initConnection();

#endif