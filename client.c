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
#include <signal.h>

#include "config.h"

#define INIT_TRANSFERS_LIST_SIZE 2
#define READ 0
#define WRITE 1

void prompt(pid_t pid_timer_child);
void timer_child(int transfer_delay);
void reccurent_transfers_child();
void strip_line_break(char* command);
int get_command_params(char *base, char target[3][80]);
void print_usage();
void send_tick_signal();
Message create_message(char*, Transfer);
Transfer create_transfer(int destination, int amount);
int add_recurrent_transfer(Transfer* toAdd);
void print_transfers_list();
void process_transfers_list();
int is_valid_number(char str[20]);
void connectToServer();
void disconnectFromServer();


// ouverture des deux pipes servant à communiquer
int pipe_fd[2];

char* server_address;
int server_port;
int source_account;
int	transfer_delay;

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
		perror("Usage: ./client adr port num delay");
		exit(1);
	}

	host = gethostbyname(argv[1]);
	server_port = atoi(argv[2]);
	source_account = atoi(argv[3]);
	transfer_delay = atoi(argv[4]);

	SYS(pipe(pipe_fd), "Error pipe");

	pid_t pid_reccurent_child = fork();
	SYS(pid_reccurent_child, "Error fork reccurent child");

	if (pid_reccurent_child > 0) {
		// Father
		pid_t pid_timer_child = fork();
		SYS(pid_timer_child, "Error fork timer child");

		if (pid_timer_child > 0) {
			// Father
			SYS(close(pipe_fd[READ]), "Error close pipe on READ");
			prompt(pid_timer_child);
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

void prompt(pid_t pid_timer_child) {

	while (1) {
		printf("> ");
		char command[255];

		if (fgets(command, 256, stdin) == NULL) {
			perror("Erreur lors du fgets");
		}

		strip_line_break(command);

		char params[3][80];

		get_command_params(command, params);

		if (strcmp(params[0], "q") == 0) {
			printf("Fin du programme\n");
			SYS(close(pipe_fd[WRITE]), "Error close pipe on WRITE");
			SYS(kill(pid_timer_child, SIGTERM), "Error kill timer child");
			exit(0);
		}

		Transfer transfer = create_transfer(atoi(params[1]), atoi(params[2]));

		if (strcmp(params[0], "+") == 0) {
			connectToServer();
			fwrite(&(transfer), sizeof(transfer), 1,socket_fd);
			disconnectFromServer();
		} else if (strcmp(params[0], "*") == 0) {
			Message msg = create_message(params[0], transfer);
			write(pipe_fd[WRITE], &msg, sizeof(msg));
		} else {
			printf("L'opération n'est pas reconnue, veuillez réessayer\n");
		}

	}
}

int get_command_params(char *command, char params[3][80]) {
	
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

Message create_message(char* type, Transfer transfer) {
	Message msg;

	if (strcmp(type, "QUIT") == 0) {
		msg.type = 0;
	} else if (strcmp(type, "*") == 0) {
		msg.type = 1;
	} else {
		printf("L'opération n'est pas reconnue, veuillez réessayer\n");
		msg.type = -1;
		return msg;
	}

	/*char *endptr1;
	transfer.destination = strtol(destination, &endptr1, 10);
	if (*endptr1 != '\0') {
		write(1, "La destination n'est pas un nombre valide\n", strlen("La destination n'est pas un nombre valide\n"));
		msg.type = -1;
		return msg;
	}

	char *endptr2;
	transfer.amount = strtol(amount, &endptr2, 10);
	if (*endptr2 != '\0') {
		write(1, "Le montant n'est pas valide\n", strlen("La destination n'est pas un nombre valide\n"));
		msg.type = -1;
		return msg;
	}*/

	msg.transfer = transfer;

	return msg;
}

Transfer create_transfer(int destination, int amount){
	Transfer transfer;
	transfer.source = source_account;
	transfer.destination = destination;
	transfer.amount = amount;
	return transfer;
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

void strip_line_break(char* command) {
	command[strlen(command)-1] = '\0';
	/*char *pos_found;
	if ((pos_found=strchr(command, '\n')) != NULL) {
		// Si on trouve \n dans la chaine, on 
	    *pos_found = '\0';
	    return 1;
	} else {
		return 0;
	}*/
}

void timer_child(int transfer_delay) {

	SYS(close(pipe_fd[READ]), "Error close pipe on READ");

	while(1) {
		// Every tick on transfer delay, send a message to the other child
		sleep(transfer_delay);
		send_tick_signal();
	}
}

void reccurent_transfers_child() {

	// On ferme l'écriture sur celui là
	SYS(close(pipe_fd[WRITE]), "Error close pipe on WRITE");

	Message message;

	while(1) {

		int messageSize = read(pipe_fd[READ], &message, sizeof(message));
		SYS(messageSize, "Error read on pipe");

		if(messageSize != 0){
			if(message.type == 0){
				SYS(close(pipe_fd[READ]), "Error close pipe on WRITE");
				exit(0);
			}
			// Enregistrement du virement dans le set
			else if (message.type == 1) {
				// il s'agit d'un virement récurrent
				add_recurrent_transfer(&message.transfer);
			} else if (message.type == 2) {
				process_transfers_list();
			} else {
				// non reconnu
			}
		} else exit(0); // EOF

	}

	// Stores reccurent transfers in an array
	// When it receives the tick signal, it loops over the array and send all transfers

}

void print_usage() {
	
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
	if(transfers_in_list == 0) return;
	connectToServer();
	for (Transfer *transfer = transfers_list; transfer - transfers_list < transfers_in_list; transfer++) {
		fwrite(transfer, sizeof(*transfer), 1,socket_fd);
	}
	disconnectFromServer();
}

void print_transfers_list() {
	printf(" ----------------- \n");
	for (Transfer *transfer = transfers_list; transfer - transfers_list < transfers_in_list; transfer++)
	    printf("Transfer (%d -> %d : %d)\n", transfer->source, transfer->destination, transfer->amount);
	printf(" ----------------- \n");
}

void connectToServer(){
	SYS((sck = socket(AF_INET,SOCK_STREAM,0)), "Error client socket");

	if( host == NULL )
	{	perror("Unknown host");
		exit(1);
	}

	bzero((char*)&addr,sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
	bcopy(host->h_addr,(char*)&addr.sin_addr.s_addr,host->h_length);
    addr.sin_port = htons(server_port);

    SYS(connect(sck, (struct sockaddr *)&addr, sizeof(addr)), "Error client connect");
    

    socket_fd = fdopen(sck,"w");
	setbuf(socket_fd,NULL);
}

void disconnectFromServer(){
	SYS(close(sck), "Error client close");
}