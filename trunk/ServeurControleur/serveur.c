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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
int main2()
{
	int i;
	for(i = 0; i<4; i++)
		shmid[i] = shmget(777+i, 4*sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
	
	
	for(i = 0; i<4; i++)
		shMem[i] = (int*) shmat(shmid[i], 0, 0);
	
	printf("Attachement OK\n");
	
	// traffic sur les entrées.
	shMem[0][OUEST] = 5;
	shMem[0][SUD] = 8;
	shMem[0][EST] = 3;
	shMem[0][NORD] = 3;
	
	shMem[1][OUEST] = 9;
	shMem[1][SUD] = 6;
	shMem[1][EST] = 3;
	shMem[1][NORD] = 8;
	
	shMem[2][OUEST] = 4;
	shMem[2][SUD] = 9;
	shMem[2][EST] = 7;
	shMem[2][NORD] = 2;
	
	shMem[3][OUEST] = 1;
	shMem[3][SUD] = 1;
	shMem[3][EST] = 4;
	shMem[3][NORD] = 10;
	
	
	Traffic tOrigine;
	Traffic tDest;
	
	// on entre a l'ouest du carrefour 0.
	tOrigine.idCarrefour = 2;
	tOrigine.idVoie = 1;
	tOrigine.Traffic = 0;
	
	// on sort au sud du carrefour 3.
	tDest.idCarrefour = 0;
	tDest.idVoie = 3;
	tDest.Traffic = 0;
	
	
	
	int voie = chemin_plus_rapide2(tOrigine, tDest);
	
	printf("\nvoie a prendre : %d\n", voie);
	
	
	// delete les mem partagées.
	
	for(i = 0; i<4; i++)
	{
		shmdt(shMem[i]);
		shmctl(shmid[i], IPC_RMID);
	}
}
*/


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
		
		printf("> Bon carrefour : N°%d, traffic derniere voie :%d\n", 
						Destination.idCarrefour, shMem[Origine.idCarrefour][Origine.idVoie]);
		
		// on retourne Dest qui contient le bon numéro de voie (Exit)
		return Destination.idVoie;
	}
	
	
	switch(Origine.idCarrefour)
	{
		case CARREFOUR_NO:
		{
			// 2 possibilités a tester.
			
			Traffic CheminA, CheminB;
			
			CheminA.Traffic = shMem[Origine.idCarrefour][Origine.idVoie];
			CheminB.Traffic = shMem[Origine.idCarrefour][Origine.idVoie];
			
			///////////////////////////////
			// A -> on prend vers l'est.
			
			// carrefour Nord-Est entrée Ouest
			CheminA.Traffic += shMem[CARREFOUR_NE][OUEST];
			
			// si ce n'est pas le carrefour d'arrivée, on continue.
			if(Destination.idCarrefour != CARREFOUR_NE)
			{
				// Carrefour Sud-Est arrivée Nord.
				CheminA.Traffic += shMem[CARREFOUR_SE][NORD];
				
				if(Destination.idCarrefour != CARREFOUR_SE)
				{
					// Carrefour Sud-Ouest arrivée Est.
					CheminA.Traffic += shMem[CARREFOUR_SO][EST];
					
					if(Destination.idCarrefour != CARREFOUR_SO)
					{
						// erreur !
						printf("Erreur ! Numéro de carrefour erronné. Code 0A\n");
					}
				}
			}
			
			
			///////////////////////////////
			// B -> on prend vers le sud.
			
			// carrefour Sud-Ouest entrée Nord
			CheminB.Traffic += shMem[CARREFOUR_SO][NORD];
			
			// si ce n'est pas le carrefour d'arrivée, on continue.
			if(Destination.idCarrefour != CARREFOUR_SO)
			{
				// Carrefour Sud-Est arrivée Ouest.
				CheminB.Traffic += shMem[CARREFOUR_SE][OUEST];
				
				if(Destination.idCarrefour != CARREFOUR_SE)
				{
					// Carrefour Nord-Est arrivée Sud.
					CheminB.Traffic += shMem[CARREFOUR_NE][SUD];
					
					if(Destination.idCarrefour != CARREFOUR_NE)
					{
						// erreur !
						printf("Erreur ! Numéro de carrefour erronné. Code 0B\n");
					}
				}
			}
			
			////////////////////////////////
			// finalement on compare.
			
			printf("PathFinder Traffics : Chemin A %d  Chemin B  %d\n", CheminA.Traffic, CheminB.Traffic);
			
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
			
			CheminA.Traffic = shMem[Origine.idCarrefour][Origine.idVoie];
			CheminB.Traffic = shMem[Origine.idCarrefour][Origine.idVoie];
			
			///////////////////////////////
			// A -> on prend vers l'ouest.
			
			// carrefour Nord-Ouest entrée Est
			CheminA.Traffic += shMem[CARREFOUR_NO][EST];
			
			// si ce n'est pas le carrefour d'arrivée, on continue.
			if(Destination.idCarrefour != CARREFOUR_NO)
			{
				// Carrefour Sud-Ouest arrivée Nord.
				CheminA.Traffic += shMem[CARREFOUR_SO][NORD];
				
				if(Destination.idCarrefour != CARREFOUR_SO)
				{
					// Carrefour Sud-Est arrivée Ouest.
					CheminA.Traffic += shMem[CARREFOUR_SE][OUEST];
					
					if(Destination.idCarrefour != CARREFOUR_SE)
					{
						// erreur !
						printf("Erreur ! Numéro de carrefour erronné. Code 1A\n");
					}
				}
			}
			
			
			///////////////////////////////
			// B -> on prend vers le sud.
			
			// carrefour Sud-Est entrée Nord
			CheminB.Traffic += shMem[CARREFOUR_SE][NORD];
			
			// si ce n'est pas le carrefour d'arrivée, on continue.
			if(Destination.idCarrefour != CARREFOUR_SE)
			{
				// Carrefour Sud-Ouest arrivée Est.
				CheminB.Traffic += shMem[CARREFOUR_SO][EST];
				
				if(Destination.idCarrefour != CARREFOUR_SO)
				{
					// Carrefour Nord-Ouest arrivée Sud.
					CheminB.Traffic += shMem[CARREFOUR_NO][SUD];
					
					if(Destination.idCarrefour != CARREFOUR_NO)
					{
						// erreur !
						printf("Erreur ! Numéro de carrefour erronné. Code 1B\n");
					}
				}
			}
			
			////////////////////////////////
			// finalement on compare.
			
			printf("PathFinder Traffics : Chemin A %d  Chemin B  %d\n", CheminA.Traffic, CheminB.Traffic);
			
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
			
			CheminA.Traffic = shMem[Origine.idCarrefour][Origine.idVoie];
			CheminB.Traffic = shMem[Origine.idCarrefour][Origine.idVoie];
			
			///////////////////////////////
			// A -> on prend vers le nord.
			
			// carrefour Nord-Ouest entrée Sud
			CheminA.Traffic += shMem[CARREFOUR_NO][SUD];
			
			// si ce n'est pas le carrefour d'arrivée, on continue.
			if(Destination.idCarrefour != CARREFOUR_NO)
			{
				// Carrefour Nord-Est arrivée Ouest.
				CheminA.Traffic += shMem[CARREFOUR_NE][OUEST];
				
				if(Destination.idCarrefour != CARREFOUR_NE)
				{
					// Carrefour Sud-Est arrivée Nord.
					CheminA.Traffic += shMem[CARREFOUR_SE][NORD];
					
					if(Destination.idCarrefour != CARREFOUR_SE)
					{
						// erreur !
						printf("Erreur ! Numéro de carrefour erronné. Code 2A\n");
					}
				}
			}
			
			
			///////////////////////////////
			// B -> on prend vers le est.
			
			// carrefour Sud-Est entrée Ouest
			CheminB.Traffic += shMem[CARREFOUR_SE][OUEST];
			
			// si ce n'est pas le carrefour d'arrivée, on continue.
			if(Destination.idCarrefour != CARREFOUR_SE)
			{
				// Carrefour Nord-Est arrivée Sud.
				CheminB.Traffic += shMem[CARREFOUR_NE][SUD];
				
				if(Destination.idCarrefour != CARREFOUR_NE)
				{
					// Carrefour Nord-Ouest arrivée Est.
					CheminB.Traffic += shMem[CARREFOUR_NO][EST];
					
					if(Destination.idCarrefour != CARREFOUR_NO)
					{
						// erreur !
						printf("Erreur ! Numéro de carrefour erronné. Code 2B\n");
					}
				}
			}
			
			////////////////////////////////
			// finalement on compare.
			
			printf("PathFinder Traffics : Chemin A %d  Chemin B  %d\n", CheminA.Traffic, CheminB.Traffic);
			
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
			
			CheminA.Traffic = shMem[Origine.idCarrefour][Origine.idVoie];
			CheminB.Traffic = shMem[Origine.idCarrefour][Origine.idVoie];
			
			///////////////////////////////
			// A -> on prend vers le nord.
			
			// carrefour Nord-Est entrée Sud
			CheminA.Traffic += shMem[CARREFOUR_NE][SUD];
			
			// si ce n'est pas le carrefour d'arrivée, on continue.
			if(Destination.idCarrefour != CARREFOUR_NE)
			{
				// Carrefour Nord-Ouest arrivée Est.
				CheminA.Traffic += shMem[CARREFOUR_NO][EST];
				
				if(Destination.idCarrefour != CARREFOUR_NO)
				{
					// Carrefour Sud-Ouest arrivée Nord.
					CheminA.Traffic += shMem[CARREFOUR_SO][NORD];
					
					if(Destination.idCarrefour != CARREFOUR_SO)
					{
						// erreur !
						printf("Erreur ! Numéro de carrefour erronné. Code 3A\n");
					}
				}
			}
			
			
			///////////////////////////////
			// B -> on prend vers le ouest.
			
			// carrefour Sud-Ouest entrée Est
			CheminB.Traffic += shMem[CARREFOUR_SO][EST];
			
			// si ce n'est pas le carrefour d'arrivée, on continue.
			if(Destination.idCarrefour != CARREFOUR_SO)
			{
				// Carrefour Nord-Ouest arrivée Sud.
				CheminB.Traffic += shMem[CARREFOUR_NO][SUD];
				
				if(Destination.idCarrefour != CARREFOUR_NO)
				{
					// Carrefour Nord-Est arrivée Ouest.
					CheminB.Traffic += shMem[CARREFOUR_NE][OUEST];
					
					if(Destination.idCarrefour != CARREFOUR_NE)
					{
						// erreur !
						printf("Erreur ! Numéro de carrefour erronné. Code 3B\n");
					}
				}
			}
			
			////////////////////////////////
			// finalement on compare.
			
			printf("PathFinder Traffics : Chemin A %d  Chemin B  %d\n", CheminA.Traffic, CheminB.Traffic);
			
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

