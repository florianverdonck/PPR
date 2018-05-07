/*

	PROGRAMME DE MAINTENANCE
	------------------------
	
	Permet de faire la maintenance du livre des comptes

	Fonctionnement :

		Reçoit les paramètres

			Si type==1

				Créée le livre des comptes (sémaphores + mémoire partagées)

			Si type==2

				Détruit le livre des comptes

			Si type==3 && opt (durée du blocage)

				// permet de vérifier que durant le blocage aucun autre programme ne doit pouvoir accéder
				// au livre des comptes

				(X)	Réserve le livre des comptes
					Attend OPT secondes avant de libérer
				(X)	Libère le libre des comptes

	Usage :

		./maint [type] [opt]

*/
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
		printf("%d\n", atoi(args[2]));
		down();
		printf("%d\n", atoi(args[2]));
		sleep(atoi(args[2]));
		up();
	}
}