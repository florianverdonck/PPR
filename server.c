/*

	SERVEUR DE VIREMENTS
	--------------------
	
	Permet aux clients de se connecter via TCP et de demander d'exécuter des virements.
	Ces virements, coté serveur sont uniques. C'est le client qui gère l'éventuelle récurrence.

	Fonctionnement :

		Démarre un socket sur le port demandé
		Attend et accepte les connexions de clients

		FOREACH CLIENT
			Reçoit le(s) virement(s) à traiter
		(X)	Réserve le livre des comptes
			Ajoute le(s) opération(s)
		(X)	Libère le libre des comptes
			Retourne le montant total des virements au client
			Ferme la connexion TCP
		END FOREACH
		

	Usage :

		./server [port]

*/

int main(int argc, char* agrv[]) {
	return 0;
}