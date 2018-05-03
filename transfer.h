#if !defined (_TRANSFER_H_)
#define _TRANSFER_H_

typedef struct Transfer Transfer;

struct Transfer {
	int source;
	int destination;
	int account;
};

#endif