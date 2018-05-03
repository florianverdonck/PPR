/*

TYPE :	0	-> Virement classique (Symbole +)
		1	-> Virement récurrent (Symbole *)
		2	-> Tick d'horloge (attribut transfer = NULL)

*/

#include "transfer.h"

#if !defined (_MESSAGE_H_)
#define _MESSAGE_H_

typedef struct Message Message;

struct Message {
	int type;
	Transfer transfer;
};

#endif

