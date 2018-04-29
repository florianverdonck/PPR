/*

	CLIENT DE VIREMENTS
	--------------------
	
	Permet aux clients de se connecter via TCP et de demander d'exécuter des virements.
	Ces virements peuvent être uniques ou récurrents

	Fonctionnement :

		Processus PERE :
			Se connecte au serveur de virement
			-> PROMPT
				Attend les commandes de l'utilisateur
				Si + alors virement unique 
					Transmet les demandes de virement
				Si * alors virement récurrent
					Ajoute le virement récurrent au set de virements du processus FILS
				Si q alors quitte l'application
					Ferme la connexion

			Processus FILS :
			-> Minuterie envoie signal à l'autre processus FILS

			Processus FILS :
			-> Attend le signal de la minuterie et envoie tous les virements du set

	Usage :

		./client [adr] [port] [num] [delay]

	Usage du prompt :

		+ [num_cpt_dest] [montant]		-> Demande le virement unique
		* [num_cpt_dest] [montant]		-> Demande le virement récurrent
		q 								-> Quitte l'application

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "client.h"

int main(int argc, char* argv[]) {

	char*	server_address = argv[0];
	int	 	server_port = atoi(argv[1]);
	char* 	source_account = argv[2];
	int		transfer_delay = atoi(argv[3]);

	pid_t pid = fork();

	if (pid < 0) {
		write(STDERR_FILENO, "Impossible de fork (fork n°1)\n", 19);
	} else if (pid > 0) {
		// Father
		pid_t pid_fork = fork();

		if (pid_fork < 0) {
			write(STDERR_FILENO, "Impossible de fork (fork n°2)\n", 19);
		} else if (pid_fork > 0) {
			// Father
			/*	THIS CAN BE ENHANCED, sleep for 2 seconds to avoid
				printing the prompt before other are done */
			sleep(1);
			prompt();
		} else {
			// Child 2
			timer_child();
		}

	} else {
		// Reccurent transfers child
		reccurent_transfers_child();
	}

	return 0;
}


void prompt() {
	printf("*** CONSOLE DE VIREMENTS ***\n");

	while (1) {
		printf("> ");
		char command[30];

		if (fgets(command, 31, stdin) == NULL) {
			perror("Erreur lors du fgets");
		}

		strip_line_break(&command);

		char params[3][20];

		getCommandParams(command, params);

		if (strcmp(params[0], "*")) {
			// Envoi du virement avec les params
		} else if (strcmp(params[0], "+")) {
			// Envoi du virement avec les params
		} else {
			printf("L'opération n'est pas reconnue, veuillez réessayer\n");
		}

	}
}

int getCommandParams(char *command, char params[3][20])
{
	int n=0,i,j=0;
	
	for(i=0;1==1;i++)
	{
		if(command[i]!=' '){
			params[n][j++]=command[i];
		}
		else{
			params[n][j++]='\0';
			n++;
			j=0;
		}
		if(command[i]=='\0')
		    break;
	}
	return n;
	
}

int strip_line_break(char* command) {
	char *pos_found;
	if ((pos_found=strchr(command, '\n')) != NULL) {
		// Si on trouve \n dans la chaine, on 
	    *pos_found = '\0';
	    return 1;
	} else {
		return 0;
	}
}

void timer_child() {
	printf("*** TIMER CHILD *** \n");
}

void reccurent_transfers_child() {
	printf("*** RECCURENT TRANSFERS CHILD *** \n");
}