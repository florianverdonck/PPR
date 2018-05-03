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

/*


TO-DO :

- liste des virements (externalisée ? STRUCT SET avec méthode d'ajout ?)
- auto-double size struct

*/


#include "client.h"

// ouverture des deux pipes servant à communiquer
int pipe_transfer_fd[2];
int pipe_timer_fd[2];

char*	server_address;
int	 	server_port;
int  	source_account;
int		transfer_delay;

// Array containing all reccurent transfers
Transfer* transfers_list;
int transfers_list_size = 0;
int transfers_in_list = 0;

int main(int argc, char* argv[]) {

	if (argc != 5) {
		printf("Le nombre de paramètres est incorrect\n");
		print_usage();
		exit(1);
	}
	
	server_address = argv[1];
	server_port = atoi(argv[2]);
	source_account = atoi(argv[3]);
	transfer_delay = atoi(argv[4]);


	if (pipe(pipe_transfer_fd) < 0) {
		printf("Erreur création pipe transfer\n");
		exit(1);
	}

	if (pipe(pipe_timer_fd) < 0) {
		printf("Erreur création pipe timer\n");
		exit(1);
	}

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
			timer_child(transfer_delay);
		}

	} else {
		// Reccurent transfers child
		reccurent_transfers_child();
	}

	return 0;
}

void prompt() {
	printf("*** CONSOLE DE VIREMENTS ***\n");

	// On ferme la lecture sur celui là
	close(pipe_transfer_fd[0]);
	// On ferme la lecture et écriture sur l'autre
	close(pipe_timer_fd[0]);
	close(pipe_timer_fd[1]);

	while (1) {
		printf("> ");
		char command[30];

		if (fgets(command, 31, stdin) == NULL) {
			perror("Erreur lors du fgets");
		}

		strip_line_break(command);

		char params[3][20];

		get_command_params(command, params);
		Message msg = create_message(params[0], params[1], params[2]);

		if (msg.type == 0) {
			// Envoi du virement avec les params
		} else if (msg.type == 1) {
			write(pipe_transfer_fd[1], &msg, sizeof(msg));
		} else {
			printf("L'opération n'est pas reconnue, veuillez réessayer\n");
		}

	}
	// On ferme l'écriture sur celui là
	close(pipe_transfer_fd[1]);
}

int get_command_params(char *command, char params[3][20]) {
	int n=0, i, j=0;
	
	for(i=0; 1==1; i++)
	{
		if(command[i] != ' '){
			params[n][j++] = command[i];
		}
		else{
			params[n][j++] = '\0';
			n++;
			j = 0;
		}
		if(command[i] == '\0')
		    break;
	}
	return n;
	
}

Message create_message(char type[20], char destination[20], char amount[20]) {
	Message msg;

	if (strcmp(type, "+") == 0) {
		msg.type = 0;
	} else if (strcmp(type, "*") == 0) {
		msg.type = 1;
	} else {
		printf("L'opération n'est pas reconnue, veuillez réessayer\n");
		msg.type = -1;
		return msg;
	}

	Transfer transfer;
	transfer.source = source_account;
	transfer.destination = atoi(destination);
	transfer.amount = atoi(amount);

	msg.transfer = transfer;

	return msg;
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

void timer_child(int transfer_delay) {
	printf("*** TIMER CHILD (each %i seconds) *** \n", transfer_delay);

	// On ferme tout
	close(pipe_transfer_fd[0]);
	close(pipe_transfer_fd[1]);

	// On ferme la lecture sur l'autre
	close(pipe_timer_fd[0]);

	while(1) {
		// Every tick on transfer delay, send a message to the other child
		sleep(transfer_delay);
		send_tick_signal();
	}
}

void reccurent_transfers_child() {
	printf("*** RECCURENT TRANSFERS CHILD *** \n");

	// On ferme l'écriture sur celui là
	close(pipe_transfer_fd[1]);

	// On ferme l'écriture sur l'autre
	close(pipe_timer_fd[1]);

	Message transferMessage;
	Message timerMessage;

	while (1) {

		int transferMessageSize = read(pipe_transfer_fd[0], &transferMessage, sizeof(transferMessage));

		if (transferMessageSize == 0) {
			// Aucun message reçu
		} else if (transferMessageSize == -1) {
			// Erreur lors de la lecture
		} else {
			write(1, "R-VIRE\n", 7);
			// Enregistrement du virement dans le set

			if (transferMessage.type == 1) {
				// il s'agit d'un virement récurrent
				add_recurrent_transfer(&transferMessage.transfer);
			} else {
				// le type du message n'est pas reconnu
			}
		}

		// NE SEMBLE PAS RECEVOIR DE MESSAGE TICK ????
		int timerMessageSize = read(pipe_timer_fd[0], &timerMessage, sizeof(timerMessage));

		if (timerMessageSize == 0) {
			write(1, "NOMESS\n", 7);
			// Aucun message reçu
		} else if (timerMessageSize == -1) {
			write(1, "ERRORR\n", 7);
			// Erreur lors de la lecture
		} else {
			write(1, "R-TICK\n", 7);
			
			if (timerMessage.type == 2) {
				process_transfers_list();
			}
		}

	}

	// Stores reccurent transfers in an array
	// When it receives the tick signal, it loops over the array and send all transfers

}

void print_usage() {
	printf("./client [adr] [port] [num] [delay]\n");
}

void send_tick_signal() {
	
	Message msg_tick;
	msg_tick.type = 2;
	
	write(1, "TICK!\n", 6);

	write(pipe_timer_fd[1], &msg_tick, sizeof(msg_tick));

}

int add_recurrent_transfer(Transfer* toAdd) {

	write(1, "Ajout virement récurrent\n", strlen("Ajout virement récurrent\n"));

	if (transfers_list_size == transfers_in_list) {
		// Liste non initialisée ou trop courte
		if (transfers_list_size == 0) {
			// Si la liste n'a jamais été initialisée
			transfers_list = (Transfer*) malloc(sizeof(toAdd) * INIT_TRANSFERS_LIST_SIZE);
			if (transfers_list != NULL) {
				// Si le malloc a réussi
				transfers_list_size = INIT_TRANSFERS_LIST_SIZE;
			} else {
				perror("Erreur lors du malloc");
				exit(1);
			}
		} else {
			// Elle a déjà été initialisée mais est maintenant trop courte
			transfers_list = (Transfer*) realloc(transfers_list, sizeof(toAdd) * (transfers_list_size * 2));
			if (transfers_list != NULL) {
				// Si le malloc a réussi
				transfers_list_size = (transfers_list_size * 2);
			} else {
				perror("Erreur lors du malloc");
				exit(1);
			}
		}
	}

	*(transfers_list + transfers_in_list) = *toAdd;
	transfers_in_list++;

	return 1;

}

void print_message(Message* msg) {
	if (msg->type == 0) {
		printf("MESSAGE = Virement classique (%i -> %i : %i)", msg->transfer.source, msg->transfer.destination, msg->transfer.amount);
	} else if (msg->type == 1) {
		printf("MESSAGE = Virement récurrent (%i -> %i : %i)", msg->transfer.source, msg->transfer.destination, msg->transfer.amount);
	} else {
		printf("MESSAGE = Tick horloge");
	}
}

void process_transfers_list() {
	for (Transfer *transfer = transfers_list; transfer - transfers_list < transfers_in_list; transfer++)
	    // ENVOI DU VIREMENT
	    printf("Virement (%i -> %i : %i)\n", transfer->source, transfer->destination, transfer->amount);
}

void print_transfers_list() {
	printf("Liste des virement \n ----------------- \n");
	for (Transfer *transfer = transfers_list; transfer - transfers_list < transfers_in_list; transfer++)
	    printf("Virement (%i -> %i : %i)\n", transfer->source, transfer->destination, transfer->amount);
}