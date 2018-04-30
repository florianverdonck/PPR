#include "transfer.h"

#if !defined (_MESSAGE_H_)
#define _MESSAGE_H_

typedef struct Message Message;

struct Message {
	int type;
	Transfer transfer;
};

#endif
