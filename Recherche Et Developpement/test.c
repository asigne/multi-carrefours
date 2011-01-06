#include "fonctions.h"

void traitantSIGINT(int num){
	printf("Interruption du programme !\n%d voitures ont été dirrigées vers des sorties non souhaitées pour faciliter la circulation des véhicules prioritaires.\n",cptExitFaux[0]);

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
	shmctl(idCptExitFaux, IPC_RMID, NULL);
	msgctl(msgidServeurControleur, IPC_RMID, NULL);
	//raise(SIGTERM);
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	
	int i;
	//creation des files de messages et des memoires partagees de chaque carrefour
	for(i=0;i<4;i++){
		msgid[i]= msgget(ftok(argv[0], ID_PROJET+cptIdentifiant), IPC_CREAT | IPC_EXCL | 0666);
		cptIdentifiant++;		
		idMemPartagee[i]=shmget(ftok(argv[0], ID_PROJET+cptIdentifiant), 8*sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
		cptIdentifiant++;
	}
	//creation file de message du serveur controleur
	msgidServeurControleur=msgget(ftok(argv[0], ID_PROJET+cptIdentifiant), IPC_CREAT | IPC_EXCL | 0666);
	cptIdentifiant++;

	for(i=0;i<4;i++){
		memoiresPartagees[i]=(int*) shmat(idMemPartagee[i], NULL, NULL);
	}
	
	
	idCptExitFaux=shmget(ftok(argv[0], ID_PROJET+cptIdentifiant), sizeof(int), IPC_CREAT | IPC_EXCL | 0777);
	cptIdentifiant++;
	cptExitFaux=(int*)shmat(idCptExitFaux, NULL, NULL);
	
	//initialisation des compteurs de chaque voie
	int carrefour, numVoie;
	pthread_mutex_lock(&memPart);
	for(carrefour = 0; carrefour<4; carrefour++){
		for(numVoie = 0; numVoie<8; numVoie++){
			memoiresPartagees[carrefour][numVoie]=0;
		}
	}
	pthread_mutex_unlock(&memPart);

	//creation et initialisation des semaphores gerant les croisements des voitures aux carrefours
	int numCarrefour,ligne,colonne;
	for(numCarrefour = 0; numCarrefour<4; numCarrefour++){
		for(ligne = 0; ligne<2; ligne++){
			for(colonne = 0; colonne<2; colonne++){	
				sem_in_out[numCarrefour][ligne][colonne]=creerSem(ftok(argv[0], ID_PROJET+cptIdentifiant), 4);
				initSem(sem_in_out[numCarrefour][ligne][colonne], 1);
				cptIdentifiant++;		
			}
		}
	}
	
	//creations des voitures 
	for(i=0;i<NbVoituresGlobal;i++){
		creerVoiture();
	}	
		
	//plateau au demarrage du programme	
	affichageCarrefours();
	sleep(1);
	
	pidPere=getpid();

	//creation des 4 carrefour	
	for(i=0;i<4;i++){
		if(getpid()==pidPere){
			pidCarrefour[i] = fork();
		}
	}
	//creation serveur controleur et du processus charge de l'affichage
	if(getpid()==pidPere){
		pidServeurControleur = fork();
	}
	if(getpid()==pidPere){	
		pidAffichage = fork();
	}
	if(getpid()==pidPere){	
		pidGenerateur = fork();
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
		// processus gérant le serveur controleur
		
		
		
		/*char buf[5][15];
		sprintf(buf[0], "%d", msgidServeurControleur);

		sprintf(buf[1], "%d", idMemPartagee[0]);
		sprintf(buf[2], "%d", idMemPartagee[1]);
		sprintf(buf[3], "%d", idMemPartagee[2]);
		sprintf(buf[4], "%d", idMemPartagee[3]);

		execl("../ServeurControleur/serveur","serveur", buf[0], buf[1], buf[2], buf[3], buf[4], NULL);
		printf("Mort du serveur controleur...\n");*/
		
		
		
		
		
		
		serveurControleur(idMemPartagee[0], idMemPartagee[1], idMemPartagee[2], idMemPartagee[3]);		
		exit(0);	
	}
	else if(pidAffichage == 0){
		while(1){
			affichageCarrefours();
			usleep(500000);
		}
	}
	else if(pidGenerateur == 0){
		while(1){
			creerVoiture();
			usleep(50);
		}
	}
	else{
		// pere
		
		//signal du Ctrl-C
		struct sigaction action;
		sigemptyset(&action.sa_mask);
		action.sa_handler=traitantSIGINT;
		sigaction(SIGINT,&action,NULL);		

		//attente de la mort des fils
		waitpid(pidCarrefour[0], NULL, NULL);
		waitpid(pidCarrefour[1], NULL, NULL);
		waitpid(pidCarrefour[2], NULL, NULL);
		waitpid(pidCarrefour[3], NULL, NULL);
		waitpid(pidGenerateur, NULL, NULL);
		waitpid(pidAffichage, NULL, NULL);
		waitpid(pidServeurControleur, NULL, NULL);
	}
}










