#include "fonctions.h"

void traitantSIGINT(int num){
	//destruction des semaphores
	int ligne,colonne,numCarrefour;
	for(numCarrefour = 0; numCarrefour<4; numCarrefour++){
		for(ligne = 0; ligne<2; ligne++){
			for(colonne = 0; colonne<2; colonne++){	
				semctl(sem_in_out[numCarrefour][ligne][colonne], 0, IPC_RMID, 0);	
			}
		}
	}
	//suppression des memoires partagées et des files de messages
	int i;
	for(i=0; i<4; i++){
		shmctl(idMemPartagee[i], IPC_RMID, NULL);
		msgctl(msgid[i],IPC_RMID, NULL);
	}
	msgctl(msgidServeurControleur, IPC_RMID, NULL);

	raise(SIGTERM);
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	
	int i;
	for(i=0;i<4;i++){
		msgid[i]= msgget(ftok(argv[0], ID_PROJET+cptIdentifiant), IPC_CREAT | IPC_EXCL | 0666);
		cptIdentifiant++;		
		idMemPartagee[i]=shmget(ftok(argv[0], ID_PROJET+cptIdentifiant), 8*sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
		cptIdentifiant++;
	}
	msgidServeurControleur=msgget(ftok(argv[0], ID_PROJET+cptIdentifiant), IPC_CREAT | IPC_EXCL | 0666);
	cptIdentifiant++;

	for(i=0;i<4;i++){
		memoiresPartagees[i]=(int*) shmat(idMemPartagee[i], NULL, NULL);
	}

	//initialisation des compteurs de chaque voie
	int carrefour, numVoie;
	pthread_mutex_lock(&memPart);
	for(carrefour = 0; carrefour<4; carrefour++){
		for(numVoie = 0; numVoie<8; numVoie++){
			memoiresPartagees[carrefour][numVoie]=0;
		}
	}
	

/*
	//vehicules non-prioritaires
	memoiresPartagees[0][0]=3;
	memoiresPartagees[0][1]=0;
	memoiresPartagees[0][2]=2;
*/
	//memoiresPartagees[1][0]=1;
/*	
	//vehicules prioritaires
	memoiresPartagees[0][4]=0;
	//memoiresPartagees[0][7]=1;
	memoiresPartagees[0][7]=0;
*/
	pthread_mutex_unlock(&memPart);



	//affichage des compteurs de chaque voie
	/*for(carrefour = 0; carrefour<4; carrefour++){
		printf("Etat du carrefour %d\n",carrefour);		
		for(numVoie = 0; numVoie<4; numVoie++){	
			printf("Voie numero %d : %d\n",numVoie+1, memoiresPartagees[carrefour][numVoie]);		
		}
		printf("\n");
	}*/

	

	int numCarrefour,ligne,colonne;
	for(numCarrefour = 0; numCarrefour<2; numCarrefour++){
		for(ligne = 0; ligne<2; ligne++){
			for(colonne = 0; colonne<2; colonne++){	
				sem_in_out[numCarrefour][ligne][colonne]=creerSem(ftok(argv[0], ID_PROJET+cptIdentifiant), 4);
				initSem(sem_in_out[numCarrefour][ligne][colonne], 1);
				cptIdentifiant++;		
			}
		}
	}
	
	
	for(i=0;i<10000;i++){
			creerVoiture();
		}	
	
	pid_t pidPere=getpid();

	//creation des 4 carrefour	
	for(i=0;i<4;i++){
		if(getpid()==pidPere){
			pidCarrefour[i] = fork();
		}
	}
	//creation serveur controleur
	if(getpid()==pidPere){
		pidServeurControleur = fork();
		pidAffichage = fork();
	}	

	if(pidCarrefour[0] == 0){
		// processus gérant le carrefour 0 (NORD-OUEST)
		gestionCarrefour(0);
		exit(0);
	}
	else if(pidCarrefour[1] == 0){
		// processus gérant le carrefour 1 (NORD-EST)
		gestionCarrefour(1);
		exit(0);		
	}
	else if(pidCarrefour[2] == 0){
		// processus gérant le carrefour 1 (NORD-EST)
		gestionCarrefour(2);
		exit(0);		
	}
	else if(pidCarrefour[3] == 0){
		// processus gérant le carrefour 1 (NORD-EST)
		gestionCarrefour(3);
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


		execl("../ServeurControleur/serveur","serveur", buf[0], buf[1], buf[2], buf[3], buf[4], NULL);
		printf("Mort du serveur controleur...\n");		
		exit(0);	
	}
	else if(pidAffichage == 0){
		while(1){
			affichageCarrefours();
			sleep(1);
		}
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
	
		/*mess m2;
		m2.car.id = 2	;		 
		m2.car.numCarrefour = 0; 
		m2.car.entree = OUEST;
		m2.car.numCarrefourFinal = 2;
		m2.car.sortieFinale = SUD;  
  		//m2.car.sortie = EST; 
		m2.car.prioritaire=FAUX;
		m2.type = OUEST;
			*/
		
		//faire une fonction pour l'envoi d'une voiture

		//entree de la voiture 1 dans le circuit
	//	msgsnd(msgid[m1.car.numCarrefour], &m1, sizeof(mess) - sizeof(long), 0);	
	//	memoiresPartagees[m1.car.numCarrefour][m1.car.entree-1]++;		//-1 car non prioritaire

//affichageCarrefours();
		
		
		//entree de la voiture 2 dans le circuit
		
		/*pthread_mutex_lock(&memPart);
		memoiresPartagees[m2.car.numCarrefour][m2.car.entree-1]++;		//m2.car.entree+3 dans le cas d'un vehicule  prioritaire
		pthread_mutex_unlock(&memPart);
		
		msgsnd(msgid[m2.car.numCarrefour], &m2, sizeof(mess) - sizeof(long), 0);*/
		
		/*for(i=0;i<1000;i++){
			creerVoiture();
		}*/		
		//printf("affichage MAIN\n");
		//affichageCarrefours();


		struct sigaction action;
		sigemptyset(&action.sa_mask);
		action.sa_handler=traitantSIGINT;
		sigaction(SIGINT,&action,NULL);		

		waitpid(pidCarrefour[0], NULL, NULL);
		waitpid(pidCarrefour[1], NULL, NULL);
		waitpid(pidCarrefour[2], NULL, NULL);
		waitpid(pidCarrefour[3], NULL, NULL);
		printf("Mort du père...\n");
	}
}










