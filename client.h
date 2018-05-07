#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <inttypes.h>
#include <ctype.h>

#include <sys/socket.h>
#include <netinet/in.h>

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