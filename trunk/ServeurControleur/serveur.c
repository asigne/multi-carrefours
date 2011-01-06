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

int chemin_plus_rapide(Traffic Origine, Traffic Destination);


int msgid;
int shmid[4];
int* shMem[4];


int main(int argc, char* argv[])
{
	//printf("LE SERVEUR CONTROLEUR EST FONCTIONNEL\n");

	if(argc != 6)
	{
		printf("Arguments manquants : ./Serveur msgid shmid0 shmid1 shmid2 shmid3\n");
		exit(1);
	}
	
	msgid = atoi(argv[1]);
	shmid[0] = atoi(argv[2]);
	shmid[1] = atoi(argv[3]);
	shmid[2] = atoi(argv[4]);
	shmid[3] = atoi(argv[5]);
	
	// on s'attache au 4 zones mémoires.
	int i;
	for(i = 0; i<4; i++)
		shMem[i] = (int*) shmat(shmid[i], 0, 0);
	
	
	//printf("valeurMemPartagee %d\n", shMem[1][0]);
	
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
	
	
	// on se détache des 4 zones mémoires.
	for(i = 0; i<4; i++)
		shmdt(shMem[i]);
	
	
	exit(0);
}


int AjouteTraffic(int TrafficCourant, int Carrefour, int Voie)
{
	if(TrafficCourant == -1 || shMem[Carrefour][Voie-1] > MAX_TRAFFIC)
	{
		return -1;
	}
	
	return TrafficCourant + shMem[Carrefour][Voie-1];
}


void* AnalyseTraffic(ReqEchgeur* req)
{
	
	Traffic tOrigine;
	Traffic tDest;
	
	tOrigine.idCarrefour = req->idOrigine;
	tOrigine.idVoie = req->voieOrigine;
	
	tDest.idCarrefour = req->idDest;
	tDest.idVoie = req->voieDest;
	
	
	// recherche de la voie a emprunter.
	int idVoieAPrendre = chemin_plus_rapide(tOrigine, tDest);
	
	
	// une fois qu'on a le chemin le plus court, on envoie un message à l'échangeur.
	
	RepCtrleur Reponse;
	
	Reponse.type = req->pidEchgeur;
	Reponse.voieDest = idVoieAPrendre;
	
	msgsnd(msgid, &Reponse, sizeof(RepCtrleur) - sizeof(long), 0);
	
	
	// liberation du message.
	free(req);
	
	
	// fin du thread.
	pthread_exit(0);
}


// /////////////////////////////////////////////////////////
// Retourne numéro de voie du carrefour en cours a prendre.
// Solution peux optimisée, sans récurrsivité.
// Elle teste simplement tout les cas possibles.

int chemin_plus_rapide(Traffic Origine, Traffic Destination)
{
	// bon carrefour dès le début ?!
	if(Origine.idCarrefour == Destination.idCarrefour)
	{
		// on est sur le bon carrefour.
		
	//	printf("> Bon carrefour : N°%d, traffic derniere voie :%d\n", 
			//			Destination.idCarrefour, shMem[Origine.idCarrefour][Origine.idVoie]);
		
		// on retourne Dest qui contient le bon numéro de voie (Exit)
		return Destination.idVoie;
	}
	
	
	switch(Origine.idCarrefour)
	{
		case CARREFOUR_NO:
		{
			// 2 possibilités a tester.
			
			Traffic CheminA, CheminB;
			
			CheminA.Traffic = 0; // shMem[Origine.idCarrefour][Origine.idVoie];
			CheminB.Traffic = 0; // shMem[Origine.idCarrefour][Origine.idVoie];
			
			///////////////////////////////
			// A -> on prend vers l'est.
			
			if(Origine.idVoie == EST)
			{
				CheminA.Traffic = -1;
			} else {
				// carrefour Nord-Est entrée Ouest
				CheminA.Traffic = AjouteTraffic(CheminA.Traffic, CARREFOUR_NE, OUEST);
				// si ce n'est pas le carrefour d'arrivée, on continue.
				if(Destination.idCarrefour != CARREFOUR_NE)
				{
					// Carrefour Sud-Est arrivée Nord.
					CheminA.Traffic = AjouteTraffic(CheminA.Traffic, CARREFOUR_SE, NORD);
				
					if(Destination.idCarrefour != CARREFOUR_SE)
					{
						// Carrefour Sud-Ouest arrivée Est.
						CheminA.Traffic = AjouteTraffic(CheminA.Traffic, CARREFOUR_SO, EST);
					
						if(Destination.idCarrefour != CARREFOUR_SO)
						{
							// erreur !
							printf("Erreur ! Numéro de carrefour erronné. Code 0A\n");
						}
					}
				}
			}
			
			///////////////////////////////
			// B -> on prend vers le sud.
			
			if(Origine.idVoie == SUD)
			{
				CheminB.Traffic = -1;
			} else {
				// carrefour Sud-Ouest entrée Nord
				CheminB.Traffic = AjouteTraffic(CheminB.Traffic, CARREFOUR_SO, NORD);
			
				// si ce n'est pas le carrefour d'arrivée, on continue.
				if(Destination.idCarrefour != CARREFOUR_SO)
				{
					// Carrefour Sud-Est arrivée Ouest.
					CheminB.Traffic = AjouteTraffic(CheminB.Traffic, CARREFOUR_SE, OUEST);
				
					if(Destination.idCarrefour != CARREFOUR_SE)
					{
						// Carrefour Nord-Est arrivée Sud.
						CheminB.Traffic = AjouteTraffic(CheminB.Traffic, CARREFOUR_NE, SUD);
					
						if(Destination.idCarrefour != CARREFOUR_NE)
						{
							// erreur !
							printf("Erreur ! Numéro de carrefour erronné. Code 0B\n");
						}
					}
				}
				
			}
			
			////////////////////////////////
			// finalement on compare.
			
			//printf("PathFinder Traffics : Chemin A %d  Chemin B  %d\n", CheminA.Traffic, CheminB.Traffic);
			
			// si les sorties sont saturées ou interdites (demitour) on prend une autre sortie.
			if(CheminA.Traffic == -1 && CheminB.Traffic == -1)
			{
				return NORD;
			}
			
			// si le chemin A est bloqué, on prend le B et inversement.
			if(CheminA.Traffic == -1)
				return SUD;
			
			if(CheminB.Traffic == -1)
				return EST;
			
			// sinon finalement on compare et on prend au plus court.
			if(CheminA.Traffic <= CheminB.Traffic)
			{
				// on prend le chemin A, vers l'est
				return EST;
				
			} else {
			
				return SUD;
			}
			
			
			break;
		}
		case 1:
		{
			// 2 possibilités a tester.
			
			Traffic CheminA, CheminB;
			
			CheminA.Traffic = 0; // shMem[Origine.idCarrefour, Origine.idVoie];
			CheminB.Traffic = 0; // shMem[Origine.idCarrefour, Origine.idVoie];
			
			///////////////////////////////
			// A -> on prend vers l'ouest.
			
			if(Origine.idVoie == OUEST)
			{
				CheminA.Traffic = -1;
			} else {
				// carrefour Nord-Ouest entrée Est
				CheminA.Traffic = AjouteTraffic(CheminA.Traffic, CARREFOUR_NO, EST);
			
				// si ce n'est pas le carrefour d'arrivée, on continue.
				if(Destination.idCarrefour != CARREFOUR_NO)
				{
					// Carrefour Sud-Ouest arrivée Nord.
					CheminA.Traffic = AjouteTraffic(CheminA.Traffic, CARREFOUR_SO, NORD);
				
					if(Destination.idCarrefour != CARREFOUR_SO)
					{
						// Carrefour Sud-Est arrivée Ouest.
						CheminA.Traffic = AjouteTraffic(CheminA.Traffic, CARREFOUR_SE, OUEST);
					
						if(Destination.idCarrefour != CARREFOUR_SE)
						{
							// erreur !
							printf("Erreur ! Numéro de carrefour erronné. Code 1A\n");
						}
					}
				}
			}
			
			///////////////////////////////
			// B -> on prend vers le sud.
			
			if(Origine.idVoie == SUD)
			{
				CheminB.Traffic = -1;
			} else {
			
				// carrefour Sud-Est entrée Nord
				CheminB.Traffic = AjouteTraffic(CheminB.Traffic, CARREFOUR_SE, NORD);
			
				// si ce n'est pas le carrefour d'arrivée, on continue.
				if(Destination.idCarrefour != CARREFOUR_SE)
				{
					// Carrefour Sud-Ouest arrivée Est.
					CheminB.Traffic = AjouteTraffic(CheminB.Traffic, CARREFOUR_SO, EST);
				
					if(Destination.idCarrefour != CARREFOUR_SO)
					{
						// Carrefour Nord-Ouest arrivée Sud.
						CheminB.Traffic = AjouteTraffic(CheminB.Traffic, CARREFOUR_NO, SUD);
					
						if(Destination.idCarrefour != CARREFOUR_NO)
						{
							// erreur !
							printf("Erreur ! Numéro de carrefour erronné. Code 1B\n");
						}
					}
				}
			}
			
			////////////////////////////////
			// finalement on compare.
			
		//	printf("PathFinder Traffics : Chemin A %d  Chemin B  %d\n", CheminA.Traffic, CheminB.Traffic);
			
			// si les sorties sont saturées ou interdites (demitour) on prend une autre sortie.
			if(CheminA.Traffic == -1 && CheminB.Traffic == -1)
			{
				return NORD;
			}
			
			// si le chemin A est bloqué, on prend le B et inversement.
			if(CheminA.Traffic == -1)
				return SUD;
			
			if(CheminB.Traffic == -1)
				return OUEST;
			
			// sinon finalement on compare et on prend au plus court.
			if(CheminA.Traffic <= CheminB.Traffic)
			{
				// on prend le chemin A, vers l'ouest
				return OUEST;
				
			} else {
			
				return SUD;
			}
			
			break;
		}
		case 2:
		{
			// 2 possibilités a tester.
			
			Traffic CheminA, CheminB;
			
			CheminA.Traffic = 0; // shMem[Origine.idCarrefour, Origine.idVoie];
			CheminB.Traffic = 0; // shMem[Origine.idCarrefour, Origine.idVoie];
			
			///////////////////////////////
			// A -> on prend vers le nord.
			
			if(Origine.idVoie == NORD)
			{
				CheminA.Traffic = -1;
			} else {
				// carrefour Nord-Ouest entrée Sud
				CheminA.Traffic = AjouteTraffic(CheminA.Traffic, CARREFOUR_NO, SUD);
			
				// si ce n'est pas le carrefour d'arrivée, on continue.
				if(Destination.idCarrefour != CARREFOUR_NO)
				{
					// Carrefour Nord-Est arrivée Ouest.
					CheminA.Traffic = AjouteTraffic(CheminA.Traffic, CARREFOUR_NE, OUEST);
				
					if(Destination.idCarrefour != CARREFOUR_NE)
					{
						// Carrefour Sud-Est arrivée Nord.
						CheminA.Traffic = AjouteTraffic(CheminA.Traffic, CARREFOUR_SE, NORD);
					
						if(Destination.idCarrefour != CARREFOUR_SE)
						{
							// erreur !
							printf("Erreur ! Numéro de carrefour erronné. Code 2A\n");
						}
					}
				}
			}
			
			///////////////////////////////
			// B -> on prend vers le est.
			
			if(Origine.idVoie == EST)
			{
				CheminB.Traffic = -1;
			} else {
			
				// carrefour Sud-Est entrée Ouest
				CheminB.Traffic = AjouteTraffic(CheminB.Traffic, CARREFOUR_SE, OUEST);
			
				// si ce n'est pas le carrefour d'arrivée, on continue.
				if(Destination.idCarrefour != CARREFOUR_SE)
				{
					// Carrefour Nord-Est arrivée Sud.
					CheminB.Traffic = AjouteTraffic(CheminB.Traffic, CARREFOUR_NE, SUD);
				
					if(Destination.idCarrefour != CARREFOUR_NE)
					{
						// Carrefour Nord-Ouest arrivée Est.
						CheminB.Traffic = AjouteTraffic(CheminB.Traffic, CARREFOUR_NO, EST);
					
						if(Destination.idCarrefour != CARREFOUR_NO)
						{
							// erreur !
							printf("Erreur ! Numéro de carrefour erronné. Code 2B\n");
						}
					}
				}
			}
			
			////////////////////////////////
			// finalement on compare.
			
			//printf("PathFinder Traffics : Chemin A %d  Chemin B  %d\n", CheminA.Traffic, CheminB.Traffic);
			
			// si les sorties sont saturées ou interdites (demitour) on prend une autre sortie.
			if(CheminA.Traffic == -1 && CheminB.Traffic == -1)
			{
				return SUD;
			}
			
			// si le chemin A est bloqué, on prend le B et inversement.
			if(CheminA.Traffic == -1)
				return EST;
			
			if(CheminB.Traffic == -1)
				return NORD;
			
			// sinon finalement on compare et on prend au plus court.
			if(CheminA.Traffic <= CheminB.Traffic)
			{
				// on prend le chemin A, vers le Nord
				return NORD;
				
			} else {
			
				return EST;
			}
			
			break;
		}
		case 3:
		{
			// 2 possibilités a tester.
			
			Traffic CheminA, CheminB;
			
			CheminA.Traffic = 0; // shMem[Origine.idCarrefour, Origine.idVoie];
			CheminB.Traffic = 0; // shMem[Origine.idCarrefour, Origine.idVoie];
			
			///////////////////////////////
			// A -> on prend vers le nord.
			
			if(Origine.idVoie == NORD)
			{
				CheminA.Traffic = -1;
			} else {
				// carrefour Nord-Est entrée Sud
				CheminA.Traffic = AjouteTraffic(CheminA.Traffic, CARREFOUR_NE, SUD);
			
				// si ce n'est pas le carrefour d'arrivée, on continue.
				if(Destination.idCarrefour != CARREFOUR_NE)
				{
					// Carrefour Nord-Ouest arrivée Est.
					CheminA.Traffic = AjouteTraffic(CheminA.Traffic, CARREFOUR_NO, EST);
				
					if(Destination.idCarrefour != CARREFOUR_NO)
					{
						// Carrefour Sud-Ouest arrivée Nord.
						CheminA.Traffic = AjouteTraffic(CheminA.Traffic, CARREFOUR_SO, NORD);
					
						if(Destination.idCarrefour != CARREFOUR_SO)
						{
							// erreur !
							printf("Erreur ! Numéro de carrefour erronné. Code 3A\n");
						}
					}
				}
			}
			
			///////////////////////////////
			// B -> on prend vers le ouest.
			
			if(Origine.idVoie == OUEST)
			{
				CheminB.Traffic = -1;
			} else {
				// carrefour Sud-Ouest entrée Est
				CheminB.Traffic = AjouteTraffic(CheminB.Traffic, CARREFOUR_SO, EST);
			
				// si ce n'est pas le carrefour d'arrivée, on continue.
				if(Destination.idCarrefour != CARREFOUR_SO)
				{
					// Carrefour Nord-Ouest arrivée Sud.
					CheminB.Traffic = AjouteTraffic(CheminB.Traffic, CARREFOUR_NO, SUD);
				
					if(Destination.idCarrefour != CARREFOUR_NO)
					{
						// Carrefour Nord-Est arrivée Ouest.
						CheminB.Traffic = AjouteTraffic(CheminB.Traffic, CARREFOUR_NE, OUEST);
					
						if(Destination.idCarrefour != CARREFOUR_NE)
						{
							// erreur !
							printf("Erreur ! Numéro de carrefour erronné. Code 3B\n");
						}
					}
				}
			}
			
			////////////////////////////////
			// finalement on compare.
			
			//printf("PathFinder Traffics : Chemin A %d  Chemin B  %d\n", CheminA.Traffic, CheminB.Traffic);
			
			// si les sorties sont saturées ou interdites (demitour) on prend une autre sortie.
			if(CheminA.Traffic == -1 && CheminB.Traffic == -1)
			{
				return SUD;
			}
			
			// si le chemin A est bloqué, on prend le B et inversement.
			if(CheminA.Traffic == -1)
				return OUEST;
			
			if(CheminB.Traffic == -1)
				return NORD;
			
			// sinon finalement on compare et on prend au plus court.
			if(CheminA.Traffic <= CheminB.Traffic)
			{
				// on prend le chemin A, vers le Nord
				return NORD;
				
			} else {
			
				return OUEST;
			}
			
			break;
		}
	}
	
	return -1;
}

