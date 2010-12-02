/* ******************************
 *          Projet LO41 
 *             UTBM
 *   Code du Serveur-Controleur
 * ****************************** */

#include "serveur.h"

/*
Fonctionnement :
=================

Le serveur controleur est un processus en fonctionnement "serveur"

Lorsqu'il se créer, il créer une file de message.
Il se met alors en attente de message.

Un message est constitué de plusieurs infos :
- pid expediteur
- origine
	- numéro du carrefour
	- numéro de la voie
- destination
	- numéro du carrefour
	- numéro de la voie

Le serveur lance alors un thread de traitement qui analyse 
le traffic et fourni une réponse du type :

- destination
	- numero du carrefour (adjacent au carrefour actuel)
	- numéro de la voie


Analyse du traffic :
=====================

Chaque Carrefour (Echangeur) créer lors de sa création 
une zone de mémoire partagée (shared memory)
Il stock dedans un tableau de X entiers. (X le nombre de voies du carrefour)

Pour chaque case du tableau, le nombre de voiture dans la voie correspondante est enregistré.
L'accès est protégé par des sémaphores afin d'éviter les accès concurentiels.

*/

// fonction "Thread" de calcul
void* AnalyseTraffic(ReqEchgeur*);

Traffic chemin_plus_rapide(Traffic Origine, Traffic Destination, Liste OpenList);

int msgid;

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("Arguments manquants : ./Serveur cle\n");
		exit(1);
	}
	
	int cle = atoi(argv[1]);
	
	
	// on créer la file de message dont la cle est passée en arg.
	msgid = msgget(cle, IPC_CREAT | IPC_EXCL | 0666);
	
	
	pthread_t thread;
	
	while(1)
	{
		ReqEchgeur* msg = (ReqEchgeur*) malloc(sizeof(ReqEchgeur));
		
		// type = 1, messages pour le serveur.
		msgrcv(msgid, msg, sizeof(ReqEchgeur), 1, 0);
		
		// message de fermeture, pid=0
		if(msg->pidEchgeur == 0)
		{
			free(msg);
			break;
		}
		
		pthread_create(&thread, NULL, (void * (*)(void *))AnalyseTraffic, msg);
	}
	
	// destruction de la file.
	msgctl(msgid, IPC_RMID, 0);
	
	exit(0);
}


void* AnalyseTraffic(ReqEchgeur* req)
{
	// fonctionnement de l'algo de recherche.
	// recurrsif.
	
	// chemin le plus rapide uniquement pour le moment.
	// le plus court sera utilisé pour les véhicules prioritaires.
	
	Traffic tOrigine;
	Traffic tDest;
	
	tOrigine.idCarrefour = req->idOrigine;
	tOrigine.idVoie = req->voieOrigine;
	
	tDest.idCarrefour = req->idDest;
	tDest.idVoie = req->voieDest;
	
	Liste OpenL;
	OpenL.Open[0] = 1;
	OpenL.Open[1] = 1;
	OpenL.Open[2] = 1;
	OpenL.Open[3] = 1;
	
	Traffic CheminAPrendre;
	CheminAPrendre = chemin_plus_rapide(tOrigine, tDest, OpenL);
}


Traffic chemin_plus_rapide(Traffic Origine, Traffic Destination, Liste OpenList)
{
	// on retire le carrefour de l'open-liste.
	OpenList.Open[Origine.idCarrefour] = 0;
	
	if(Origine.idCarrefour == Destination.idCarrefour)
	{
		// on est sur le bon carrefour.
		
		// recherche avec sharedmem le nb de voiture du carrefour.
		int nb_voiture = 10;
		
		Origine.Traffic = nb_voiture;
		
		return Origine;
	}
	else
	{
		// on test les autres sorties
		switch(Origine.idCarrefour)
		{
		case 0:
		{
			// depuis le carrefour 0 on test l'est et le sud !
			Origine.idCarrefour = 1;
			Traffic EST = chemin_plus_rapide(Origine, Destination, OpenList);
			
			Origine.idCarrefour = 2;
			Traffic SUD = chemin_plus_rapide(Origine, Destination, OpenList);
			
			if(EST.Traffic < SUD.Traffic)
			{
				Origine.idCarrefour = 1;
				Origine.Traffic += EST.Traffic;
				return Origine;
			}
			
			break;
		}
		case 1:
		{
			// depuis le carrefour 1 on test l'ouest et le sud
			break;
		}
		case 2:
		{
			// depuis le carrefour 2 on test le nord et l'est
			break;
		}
		case 3:
		{
			// depuis le carrefour 3 on test le nord et l'ouest
			break;
		}
		} // -switch
		
		return Origine;
		
		/*
		int ouest = chemin_plus_rapide(origine.carrefour+1, destination);
		int est = chemin_plus_rapide(origine.carrefour+2, destination);
		int sud = chemin_plus_rapide(origine.carrefour+3, destination);
		int nord = chemin_plus_rapide(origine.carrefour+4, destination);
		
		on prend le plus court.
		Traffic t;
		t.idCarrefour = plus_court.carrefour;
		t.idVoie = plus_court.voie;
		t.Traffic = plus_court.nb_voiture
		
		return t;*/
	}
	
}


