/*

TYPE :	0	-> Virement classique (Symbole +)
		1	-> Virement récurrent (Symbole *)
		2	-> Tick d'horloge (attribut transfer = NULL)

*/

#if !defined (_CONFIG_H_)
#define _CONFIG_H_

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

