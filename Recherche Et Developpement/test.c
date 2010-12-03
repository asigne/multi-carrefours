#include "fonctions.h"

void traitantSIGINT(int num){
	/*printf("Suppression Sémaphores\n");
	//destruction des semaphores
	int ligne,colonne,numCarrefour;
	for(numCarrefour = 0; numCarrefour<4; numCarrefour++){
		for(ligne = 0; ligne<2; ligne++){
			for(colonne = 0; colonne<2; colonne++){	
			//	printf("%d\n",semctl(sem_in_out[numCarrefour][ligne][colonne], 0, IPC_RMID, 0));	
				semctl(sem_in_out[numCarrefour][ligne][colonne], 0, IPC_RMID, 0);	
			}
		}
	}*/


	//suppression des memoires partagées et des files de messages
	//printf("suppression Memoires Partagées et Files de messages\n");
	int i;
	for(i=0; i<4; i++){
		//printf("%d\t%d\n",idMemPartagee[i], shmctl(idMemPartagee[i], 0, IPC_RMID, NULL));
		//printf("%d\t%d\n",msgid[i], msgctl(msgid[i],0, IPC_RMID));
	}
	raise(SIGTERM);
}

int main(int argc, char **argv)
{
	struct sigaction action;
	sigemptyset(&action.sa_mask);
	action.sa_handler=traitantSIGINT;
	sigaction(SIGINT,&action,NULL);

	int i;

	msgid[0]= msgget(666, IPC_CREAT | IPC_EXCL | 0666);
	msgid[1]= msgget(667, IPC_CREAT | IPC_EXCL | 0666);
	msgid[2]= msgget(668, IPC_CREAT | IPC_EXCL | 0666);
	msgid[3]= msgget(669, IPC_CREAT | IPC_EXCL | 0666);

	msgidServeurControleur=msgget(777, IPC_CREAT | IPC_EXCL | 0666);

	idMemPartagee[0]=shmget(886, 8*sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
	idMemPartagee[1]=shmget(887, 8*sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
	idMemPartagee[2]=shmget(888, 8*sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
	idMemPartagee[3]=shmget(889, 8*sizeof(int), IPC_CREAT | IPC_EXCL | 0666);

//	int* memoiresPartagees[4];
	for(i=0;i<4;i++){
		memoiresPartagees[i]=(int*) shmat(idMemPartagee[i], NULL, NULL);
	}

	//initialisation des compteurs de chaque voie
	int carrefour, numVoie;
	for(carrefour = 0; carrefour<4; carrefour++){
		for(numVoie = 0; numVoie<8; numVoie++){
			memoiresPartagees[carrefour][numVoie]=0;
		}
	}

	memoiresPartagees[0][0]=3;
	memoiresPartagees[0][1]=0;
	memoiresPartagees[0][2]=2;
	memoiresPartagees[0][3]=18;

	memoiresPartagees[0][4]=0;
	memoiresPartagees[0][7]=1;


	//affichage des compteurs de chaque voie
	/*for(carrefour = 0; carrefour<4; carrefour++){
		printf("Etat du carrefour %d\n",carrefour);		
		for(numVoie = 0; numVoie<4; numVoie++){	
			printf("Voie numero %d : %d\n",numVoie+1, memoiresPartagees[carrefour][numVoie]);		
		}
		printf("\n");
	}*/


	int numCarrefour,ligne,clef,colonne;
	int cpt=1;
	for(numCarrefour = 0; numCarrefour<2; numCarrefour++){
		for(ligne = 0; ligne<2; ligne++){
			for(colonne = 0; colonne<2; colonne++){	
				clef = 999+cpt*(numCarrefour+10); 			//verifier pour la clé
				sem_in_out[numCarrefour][ligne][colonne]=creerSem(clef, 4);
				initSem(sem_in_out[numCarrefour][ligne][colonne], 1);
				cpt++;		
			}
		}
		cpt=1;		
	}

	pid_t pidPere=getpid();


	for(i=0;i<4;i++){
		if(getpid()==pidPere){
			//printf("Creation du fils numero %d\n",i);
			pidCarrefour[i] = fork();
		}
	}
	//creation serveur controleur
	if(getpid()==pidPere){
		pidServeurControleur = fork();
	}	


	if(pidCarrefour[0] == 0){
		// processus gérant le carrefour 0 (NORD-OUEST)
		gestionCarrefour(0);
		printf("Mort du fils 0...\n");
		exit(0);
	}
	else if(pidCarrefour[1] == 0){
		// processus gérant le carrefour 1 (NORD-EST)
		gestionCarrefour(1);
		//printf("Mort du fils 1...\n");		
		exit(0);		
	}
	else if(pidCarrefour[2] == 0){
		// processus gérant le carrefour 1 (NORD-EST)
		gestionCarrefour(2);
		//printf("Mort du fils 2...\n");		
		exit(0);		
	}
	else if(pidCarrefour[3] == 0){
		// processus gérant le carrefour 1 (NORD-EST)
		gestionCarrefour(3);
		//printf("Mort du fils 3...\n");		
		exit(0);		
	}
	else if(pidServeurControleur == 0){
		// processus gérantle serveur controleur
	/*	char buf[5][15];
		sprintf(buf[0], "%d", msgidServeurControleur);

		sprintf(buf[1], "%d", idMemPartagee[0]);
		sprintf(buf[2], "%d", idMemPartagee[1]);
		sprintf(buf[3], "%d", idMemPartagee[2]);
		sprintf(buf[4], "%d", idMemPartagee[3]);


		execl("serveurControleur","serveurControleur", buf[0], buf[1], buf[2], buf[3], buf[4], NULL);
		printf("Mort du serveur controleur...\n");		
		exit(0);	*/
	}
	else{
		// pere
		mess m1;
		m1.car.id = 1;		 
		m1.car.numCarrefour = 0; 
		m1.car.entree = NORD;
		m1.car.numCarrefourFinal = 3;
		m1.car.sortieFinale = EST;  
  		m1.car.sortie = EST; 
		m1.car.prioritaire=FAUX;
		m1.type = NORD;
	
		mess m2;
		m2.car.id = 2	;		 
		m2.car.numCarrefour = 0; 
		m2.car.entree = OUEST;
		m2.car.numCarrefourFinal = 2;
		m2.car.sortieFinale = EST;  
  		m2.car.sortie = EST; 
		m2.car.prioritaire=VRAI;
		m2.type = OUEST;
			
		
		//faire une fonction pour l'envoi d'une voiture

		//entree de la voiture 1 dans le circuit
	//	msgsnd(msgid[m1.car.numCarrefour], &m1, sizeof(mess) - sizeof(long), 0);	
	//	memoiresPartagees[m1.car.numCarrefour][m1.car.entree-1]++;		//-1 car non prioritaire

		//entree de la voiture 2 dans le circuit
		msgsnd(msgid[m2.car.numCarrefour], &m2, sizeof(mess) - sizeof(long), 0);
		memoiresPartagees[m2.car.numCarrefour][m2.car.entree+3]++;		//+3 car prioritaire



		//printf("2 Nouvelle voiture dans la file...\n");


	/*	for(i=0;i<4;i++){
			printf("%d\n",pidCarrefour[i]);
		}*/

		//sleep(5);

		//afficheEtatSem();

		
		waitpid(pidCarrefour[0], NULL, NULL);
		waitpid(pidCarrefour[1], NULL, NULL);
		waitpid(pidCarrefour[2], NULL, NULL);
		waitpid(pidCarrefour[3], NULL, NULL);

		printf("Mort du père...\n");
	}
}










