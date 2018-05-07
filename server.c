/*

	SERVEUR DE VIREMENTS
	--------------------
	
	Permet aux clients de se connecter via TCP et de demander d'exécuter des virements.
	Ces virements, coté serveur sont uniques. C'est le client qui gère l'éventuelle récurrence.

	Fonctionnement :

		Démarre un socket sur le port demandé
		Attend et accepte les connexions de clients

		FOREACH CLIENT
			Reçoit le(s) virement(s) à traiter
		(X)	Réserve le livre des comptes
			Ajoute le(s) opération(s)
		(X)	Libère le libre des comptes
			Retourne le montant total des virements au client
			Ferme la connexion TCP
		END FOREACH
		

	Usage :

		./server [port]

*/
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "socket.h"
#include "config.h"
#include "operation.h"
#define PORT 8080
#define BUFLEN  80

static char buffer[BUFLEN];

int main() {
	int sck;
    struct sockaddr_in addr;

    if( (sck = socket(AF_INET,SOCK_STREAM,0)) < 0 )
    {   perror("server - socket");
        exit(1);
    }

    bzero((char*)&addr,sizeof(struct sockaddr_in));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(PORT);

    if( bind(sck, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
    {   perror("server - bind");
        exit(1);
    }

    fprintf(stderr,"Start listening for connections\n");
    listen(sck,1);

    while( 1 )
    {   struct sockaddr_in addr2;
        u_int len2 = sizeof(addr2);
        int   sck2 = accept(sck,(struct sockaddr *)&addr2,&len2);
        //fprintf(stderr,"Connection from %s %d\n",inet_ntoa(addr2.sin_addr.s_addr), ntohs(addr2.sin_port));
        fprintf(stderr,"Connection from %s %d\n",inet_ntoa(addr2.sin_addr), ntohs(addr2.sin_port));
        FILE *netFd = fdopen(sck2,"r");
        Transfer* transfer;
        size_t res;
        while(res = fread(transfer, sizeof(*transfer), 1, netFd)){
            fprintf(stderr,"%s",buffer);
			if(res == sizeof(*transfer)){
                depositOrWithdrawal(transfer->source, -(transfer->amount));
                depositOrWithdrawal(transfer->destination, transfer->amount);
                printf("Source: %d   Destination: %d   Amount: %d", transfer->source, transfer->destination, transfer->amount);
			} else {
                fprintf(stderr,"Closing connection\n");
				close(sck2);
				break;
            }
        }
    }

    exit(0);
}