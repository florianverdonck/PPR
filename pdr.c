/*

	PROGRAMME DE DEPOTS ET DE RETRAITS
	----------------------------------
	
	Permet de déposer et de retirer de l'argent facilement

	Fonctionnement :

		Reçoit les paramètres
	(X)	Réserve le livre des comptes
		Ajoute ou retire l'argent sur le compte
	(X)	Libère le libre des comptes

	Usage :

		./pdr [num_cpt_destinataire] {-}[montant]

*/
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "operation.h"
#include "resource.h"

int main(int argc, char* args[]) {
	/*if(argc != 3) {
		perror("pdr account amount");
		exit(1);
	}*/
	if(argc == 1){
		reader = (int *)shmat(shmid, NULL, SHM_R);
    	SYS1(*reader);
		int* ptr = reader;
		for(ptr; ptr < reader+20; ptr++){
			printf("%d\n", *ptr);
		}
	}else{
	depositOrWithdrawal(atoi(args[1]), atoi(args[2]));
	}

}