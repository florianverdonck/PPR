#if !defined (_CONFIG_H_)
#define _CONFIG_H_

#define SYS(call,msg) (call == -1 ? perror("SYSCALL ERROR: " msg "\n"),exit(5) : 0)

typedef struct Transfer Transfer;

struct Transfer {
	int source;
	int destination;
	int amount;
};

typedef struct Message Message;

struct Message {
	int type;
	Transfer transfer;
};

#endif

