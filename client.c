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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <inttypes.h>
#include <ctype.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>

#include "config.h"

#define INIT_TRANSFERS_LIST_SIZE 2

void prompt();

void timer_child(int transfer_delay);

void reccurent_transfers_child();

int strip_line_break(char* command);

int get_command_params(char *base, char target[10][20]);

void print_usage();

void send_tick_signal();

Message create_message(char type[20], char destination[20], char amount[20]);

int add_recurrent_transfer(Transfer* toAdd);

void print_transfers_list();

void process_transfers_list();

int is_valid_number(char str[20]);



// ouverture des deux pipes servant à communiquer
int pipe_fd[2];

char*	server_address;
int	 	server_port;
int  	source_account;
int		transfer_delay;

// Array containing all reccurent transfers
Transfer* transfers_list;
int transfers_list_size = 0;
int transfers_in_list = 0;

// Variables related to socket connection
int sck;
FILE *socket_fd;
struct sockaddr_in addr;
struct hostent *host;

int main(int argc, char* argv[]) {

	if (argc != 5) {
		printf("Le nombre de paramètres est incorrect\n");
		print_usage();
		exit(1);
	}

	if( (sck = socket(AF_INET,SOCK_STREAM,0)) < 0 )
    {   perror("server - socket");
        exit(1);
    }

    host = gethostbyname(argv[1]);
	server_port = atoi(argv[2]);
	source_account = atoi(argv[3]);
	transfer_delay = atoi(argv[4]);

	if( host == NULL )
	{	fprintf(stderr,"Unknown host\n");
		exit(1);
	}

	bzero((char*)&addr,sizeof(struct sockaddr_in));
    addr.sin_family      = AF_INET;
	bcopy(host->h_addr,(char*)&addr.sin_addr.s_addr,host->h_length);
    addr.sin_port        = htons(server_port);

    if( connect(sck, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
    {   perror("client - connect");
        exit(1);
    }

    socket_fd = fdopen(sck,"w");
	setbuf(socket_fd,NULL);

    if( isatty(0) )
	    fprintf(stderr,"Connected\n> ");

	if (pipe(pipe_fd) < 0) {
		printf("Erreur création pipe transfer\n");
		exit(1);
	}

	pid_t pid_reccurent_child = fork();

	if (pid_reccurent_child < 0) {
		write(STDERR_FILENO, "Impossible de fork (fork n°1)\n", 19);
	} else if (pid_reccurent_child > 0) {
		// Father
		pid_t pid_timer_child = fork();

		if (pid_timer_child < 0) {
			write(STDERR_FILENO, "Impossible de fork (fork n°2)\n", 19);
		} else if (pid_timer_child > 0) {
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

	// On ferme la lecture sur celui là
	close(pipe_fd[0]);

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

		if (msg.type == -1) continue;

		if (msg.type == 0) {
			printf("Source: %d   Destination: %d   Amount: %d\n", msg.transfer.source, msg.transfer.destination, msg.transfer.amount);
			fwrite(&(msg.transfer), sizeof(msg.transfer), 1,socket_fd);
			printf("Envoyé\n");
		} else if (msg.type == 1) {
			write(pipe_fd[1], &msg, sizeof(msg));
		} else {
			printf("L'opération n'est pas reconnue, veuillez réessayer\n");
		}

	}
	// On ferme l'écriture sur celui là
	close(pipe_fd[1]);
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

	char *endptr;
	transfer.destination = strtol(destination, &endptr, 10);
	if (*endptr != '\0') {
		write(1, "La destination n'est pas un nombre valide\n", strlen("La destination n'est pas un nombre valide\n"));
		msg.type = -1;
		return msg;
	}

	/*


	CHECK


	*/ 
	transfer.amount = strtol(amount, &endptr, 10);
	if (*endptr != '\0') {
		write(1, "Le montant n'est pas valide\n", strlen("La destination n'est pas un nombre valide\n"));
		msg.type = -1;
		return msg;
	}

	msg.transfer = transfer;

	return msg;
}

int is_valid_number(char str[20]) {
	int i = 0;
	while(i<strlen(str))
	{
	  if(!isdigit(str[i]))
	  {
	    return 0;
	  }
	}
	return 1;
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

	// On ferme tout
	close(pipe_fd[0]);

	while(1) {
		// Every tick on transfer delay, send a message to the other child
		sleep(transfer_delay);
		send_tick_signal();
	}
}

void reccurent_transfers_child() {

	// On ferme l'écriture sur celui là
	close(pipe_fd[1]);

	Message message;

	while (1) {

		int messageSize = read(pipe_fd[0], &message, sizeof(message));

		if (messageSize == 0) {
			// Aucun message reçu
		} else if (messageSize == -1) {
			// Erreur lors de la lecture
		} else {
			// Enregistrement du virement dans le set
			if (message.type == 1) {
				// il s'agit d'un virement récurrent
				add_recurrent_transfer(&message.transfer);
			} else if (message.type == 2) {
				process_transfers_list();
			} else {
				// non reconnu
			}
		}

	}

	// Stores reccurent transfers in an array
	// When it receives the tick signal, it loops over the array and send all transfers

}

void print_usage() {
	write(1, "./client [adr] [port] [num] [delay]\n", strlen("./client [adr] [port] [num] [delay]\n"));
}

void send_tick_signal() {
	
	Message msg_tick;
	msg_tick.type = 2;

	write(pipe_fd[1], &msg_tick, sizeof(msg_tick));

}

int add_recurrent_transfer(Transfer* toAdd) {

	if (transfers_list_size == transfers_in_list) {

		// Liste non initialisée ou trop courte
		if (transfers_list_size == 0) {
			// Si la liste n'a jamais été initialisée
			transfers_list = (Transfer*) malloc(sizeof(*toAdd) * INIT_TRANSFERS_LIST_SIZE);
			if (transfers_list != NULL) {
				// Si le malloc a réussi
				transfers_list_size = INIT_TRANSFERS_LIST_SIZE;
			} else {
				perror("Erreur lors du malloc");
				exit(1);
			}
		} else {
			// Elle a déjà été initialisée mais est maintenant trop courte
			transfers_list = (Transfer*) realloc(transfers_list, sizeof(*toAdd) * (transfers_list_size * 2));
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

void process_transfers_list() {
	write(1, "PROCESS FOLLOWING TRANSFERS\n", strlen("PROCESS FOLLOWING TRANSFERS\n"));
	print_transfers_list();
}

void print_transfers_list() {
	printf(" ----------------- \n");
	for (Transfer *transfer = transfers_list; transfer - transfers_list < transfers_in_list; transfer++)
	    printf("Virement (%i -> %i : %i)\n", transfer->source, transfer->destination, transfer->amount);
	printf(" ----------------- \n");
}