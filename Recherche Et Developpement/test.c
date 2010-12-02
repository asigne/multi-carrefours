#include "fonctions.h"
int main(int argc, char **argv)
{
	int i;

	msgid[0]= msgget(666, IPC_CREAT | IPC_EXCL | 0666);
	msgid[1]= msgget(667, IPC_CREAT | IPC_EXCL | 0666);
	msgid[2]= msgget(668, IPC_CREAT | IPC_EXCL | 0666);
	msgid[3]= msgget(669, IPC_CREAT | IPC_EXCL | 0666);

	msgidServeurControleur=msgget(777, IPC_CREAT | IPC_EXCL | 0666);

	idMemPartagee[0]=shmget(886, 4*sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
	idMemPartagee[1]=shmget(887, 4*sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
	idMemPartagee[2]=shmget(888, 4*sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
	idMemPartagee[3]=shmget(889, 4*sizeof(int), IPC_CREAT | IPC_EXCL | 0666);

//	int* memoiresPartagees[4];
	for(i=0;i<4;i++){
		memoiresPartagees[i]=(int*) shmat(idMemPartagee[i], NULL, NULL);
	}

	//initialisation des compteurs de chaque voie
	int carrefour, numVoie;
	for(carrefour = 0; carrefour<4; carrefour++){
		for(numVoie = 0; numVoie<4; numVoie++){
			memoiresPartagees[carrefour][numVoie]=0;
		}
	}

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
	//	gestionCarrefour(2);
		//printf("Mort du fils 2...\n");		
		exit(0);		
	}
	else if(pidCarrefour[3] == 0){
		// processus gérant le carrefour 1 (NORD-EST)
	//	gestionCarrefour(3);
		//printf("Mort du fils 3...\n");		
		exit(0);		
	}
	else if(pidServeurControleur == 0){
		// processus gérantle serveur controleur
		char buf[5][15];
		sprintf(buf[0], "%d", msgidServeurControleur);

		sprintf(buf[1], "%d", idMemPartagee[0]);
		sprintf(buf[2], "%d", idMemPartagee[1]);
		sprintf(buf[3], "%d", idMemPartagee[2]);
		sprintf(buf[4], "%d", idMemPartagee[3]);


		execl("serveurControleur","serveurControleur", buf[0], buf[1], buf[2], buf[3], buf[4], NULL);
		printf("Mort du serveur controleur...\n");		
		exit(0);	
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
		m1.type = 1;
	
		mess m2;
		m2.car.id = 2;
		m2.car.entree = NORD;
		m2.car.sortie = SUD;
		m2.car.numCarrefour = 0;
		m2.type = 1;
		
		mess m3;
		m3.car.id = 3;
		m3.car.sortie = OUEST;
		m3.car.entree = NORD;
		m3.type = 1;

		mess m4;
		m4.car.id = 4;
		m4.car.sortie = EST;
		m4.car.entree = OUEST;
		m4.type = 1;
		
		mess m5;
		m5.car.id = 5;
		m5.car.sortie = NORD;
		m5.car.entree = OUEST;
		m5.type = OUEST;
		
		mess m6;
		m6.car.id = 6;
		m6.car.sortie = SUD;
		m6.car.entree = OUEST;
		m6.type = OUEST;


		
		
	//	msgsnd(msgid[0], &m2, sizeof(mess) - sizeof(long), 0);


		msgsnd(msgid[0], &m1, sizeof(mess) - sizeof(long), 0);	
		memoiresPartagees[m1.car.numCarrefour][m1.car.entree-1]++;



	//	msgsnd(msgid[0], &m3, sizeof(mess) - sizeof(long), 0);
	//	msgsnd(msgid[0], &m4, sizeof(mess) - sizeof(long), 0);	
	//	msgsnd(msgid[0], &m5, sizeof(mess) - sizeof(long), 0);
	//	msgsnd(msgid[0], &m6, sizeof(mess) - sizeof(long), 0);
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










