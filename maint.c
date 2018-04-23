/*

	PROGRAMME DE MAINTENANCE
	------------------------
	
	Permet de faire la maintenance du livre des comptes

	Fonctionnement :

		Reçoit les paramètres

			Si type==1

				Créée le livre des comptes (sémaphores + mémoire partagées)

			Si type==2

				Détruit le livre des comptes

			Si type==3 && opt (durée du blocage)

				// permet de vérifier que durant le blocage aucun autre programme ne doit pouvoir accéder
				// au livre des comptes

				(X)	Réserve le livre des comptes
					Attend OPT secondes avant de libérer
				(X)	Libère le libre des comptes

	Usage :

		./maint [type] [opt]

*/

int main(int argc, char* agrv[]) {
	return 0;
}