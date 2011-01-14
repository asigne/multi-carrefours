#include "fonctions.h"

void traitantSIGINT(int num){
	double pourcentageReussite;
	
	pthread_mutex_lock(&mCptExitFaux);
	pthread_mutex_lock(&mCptVoitures);
	pourcentageReussite=(1-(double)cptExitFaux[0]/((double)cptVoitures[VRAI]+(double)cptVoitures[FAUX]))*100;
	printf("\nLe pourcentage de réussite est de : %.2f\n",pourcentageReussite);
	pthread_mutex_unlock(&mCptVoitures);
	pthread_mutex_unlock(&mCptExitFaux);
	
	//destruction des semaphores de chaque carrefour
	int ligne,colonne,numCarrefour;
	for(numCarrefour = 0; numCarrefour<4; numCarrefour++){
		for(ligne = 0; ligne<2; ligne++){
			for(colonne = 0; colonne<2; colonne++){	
				semctl(sem_in_out[numCarrefour][ligne][colonne], 0, IPC_RMID, 0);	
			}
		}
	}
	
	//destruction des memoires partagées et des files de messages et du sémaphore de chaque carrefour
	int i;
	for(i=0; i<4; i++){
		shmctl(idMemPartagee[i], IPC_RMID, NULL);
		msgctl(msgid[i],IPC_RMID, NULL);
		semctl(semCptVoituresDansCarrefour[i], 0, IPC_RMID, 0);	
	}
	
	//desctruction mémoires partagées pour les compteurs
	shmctl(idCptExitFaux, IPC_RMID, NULL);
	shmctl(idCptVoitures, IPC_RMID, NULL);
	
	//destruction file de message serveur-controleur
	msgctl(msgidServeurControleur, IPC_RMID, NULL);

	//destruction des mutex
	pthread_mutex_destroy(&memPart);
	pthread_mutex_destroy(&mCptVoitures);
	pthread_mutex_destroy(&mCptExitFaux);
	//raise(SIGTERM);
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	remove("log.txt");
	int i;
	
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
	
	//creation des files de messages et des memoires partagees et semaphore de chaque carrefour
	for(i=0;i<4;i++){
		msgid[i]= msgget(ftok(argv[0], ID_PROJET+cptIdentifiant), IPC_CREAT | IPC_EXCL | 0666);
		cptIdentifiant++;		
		idMemPartagee[i]=shmget(ftok(argv[0], ID_PROJET+cptIdentifiant), 8*sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
		cptIdentifiant++;
		memoiresPartagees[i]=(int*) shmat(idMemPartagee[i], NULL, NULL);
		semCptVoituresDansCarrefour[i]=creerSem(ftok(argv[0], ID_PROJET+cptIdentifiant), 4);
		initSem(semCptVoituresDansCarrefour[i], 3);
		cptIdentifiant++;	
	}

	//creation memoire partagee pour compteur d'echecs
	idCptExitFaux=shmget(ftok(argv[0], ID_PROJET+cptIdentifiant), sizeof(int), IPC_CREAT | IPC_EXCL | 0777);
	cptIdentifiant++;
	cptExitFaux=(int*)shmat(idCptExitFaux, NULL, NULL);
	//creation memoire partagee pour compteur de voitures
	idCptVoitures=shmget(ftok(argv[0], ID_PROJET+cptIdentifiant), sizeof(int), IPC_CREAT | IPC_EXCL | 0777);
	cptIdentifiant++;
	cptVoitures=(int*)shmat(idCptVoitures, NULL, NULL);
	
	//creation file de message du serveur controleur
	msgidServeurControleur=msgget(ftok(argv[0], ID_PROJET+cptIdentifiant), IPC_CREAT | IPC_EXCL | 0666);
	cptIdentifiant++;
	
	
	//initialisation des compteurs de chaque voie
	int carrefour, numVoie;
	for(carrefour = 0; carrefour<4; carrefour++){
		for(numVoie = 0; numVoie<8; numVoie++){
			memoiresPartagees[carrefour][numVoie]=0;
		}
	}
	
	//initialisation du nombre de voitures à 0	
	cptVoitures[0]=0;


	//creations des voitures initiales
	for(i=0;i<NbVoituresGlobal;i++){
		creerVoiture();
	}
		
	//affichage du circuit au demarrage du programme	
	affichageCarrefours();
	
	//recuperation pid du pere
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
	
	//creation processus pour l'affichage
	if(getpid()==pidPere){	
		pidAffichage = fork();
	}
	
	//creation processus pour envoi des voitures dans le circuit
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
		serveurControleur(idMemPartagee[0], idMemPartagee[1], idMemPartagee[2], idMemPartagee[3]);		
		exit(0);	
	}
	else if(pidAffichage == 0){
		while(1){
			affichageCarrefours();
			usleep(raffraichissementAffichage);
		}
		exit(0);
	}
	else if(pidGenerateur == 0){
		while(1){
			creerVoiture();
			usleep(delaisNouvelleVoiture);
		}
		exit(0);
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










