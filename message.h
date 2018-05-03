/*

TYPE :	0	-> Virement classique (Symbole +)
		1	-> Virement récurrent (Symbole *)
		2	-> Tick d'horloge (attribut transfer = NULL)

*/

typedef struct Message {
	int type;
	Transfer transfer;
} Message;