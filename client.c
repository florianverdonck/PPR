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

int main(int argc, char* agrv[]) {
	return 0;
}