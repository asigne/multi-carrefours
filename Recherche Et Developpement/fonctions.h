#include "voiture.h"

void supprimerIPC(){
	printf("Suppression Sémaphores");
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
	printf("suppression Memoires Partagées et Files de messages");
	int i;
	for(i=0; i<4; i++){
		shmctl(idMemPartagee[i],0, IPC_RMID, NULL);
		shmctl(msgid[i],0, IPC_RMID, NULL);
	}
}

void nbMessageDansFile(int numCarrefour){
	struct msqid_ds msqid_ds, *buf;
    buf = & msqid_ds;
	msgctl(msgid[numCarrefour], IPC_STAT, buf);
	printf("Il y a %d véhicule(s) dans la file du carrefour %d\n",buf->msg_qnum, numCarrefour);
}


void affichageCarrefour(int carrefour){
	int numVoie;	
	printf("Etat du carrefour %d\n",carrefour);		
	for(numVoie = 0; numVoie<4; numVoie++){	
		printf("Voie numero %d : np:%d  p:%d\n",numVoie+1, memoiresPartagees[carrefour][numVoie], memoiresPartagees[carrefour][numVoie+4]);		
	}
	printf("\n");
}

void affichageCarrefours(){
	int carrefour, numVoie;	
	for(carrefour = 0; carrefour<4; carrefour++){
		affichageCarrefour(carrefour);
	}
}



int creerSem(int clef, int nombre){
	int semid = semget(clef, nombre, IPC_CREAT | IPC_EXCL | 0666);
	return semid;
}

void initSem(int id, int valeur){
	semctl(id, 0, SETVAL, valeur); 
}

int PSem(int id){
	struct sembuf op;
     
	op.sem_num = 0; //Numéro de notre sémaphore
	op.sem_op = -1; //Pour un P() on décrémente
	op.sem_flg = 0; //On ne s'en occupe pas
	usleep(50);
    return semop(id, &op, 1); //Entrée dans la section critique P()
}

int VSem(int id){
	struct sembuf op;
     
	op.sem_num = 0; //Numéro de notre sémaphore
	op.sem_op = 1; //Pour un V() on incrémente
	op.sem_flg = 0; //On ne s'en occupe pas

    semop(id, &op, 1); //Sortie de la section critique V()
}

void destructionSem(){
	int ligne,colonne,numCarrefour;
	 //Destruction du sémaphore
	for(numCarrefour = 0; numCarrefour<4; numCarrefour++){
		for(ligne = 0; ligne<2; ligne++){
			for(colonne = 0; colonne<2; colonne++){	
				semctl(sem_in_out[numCarrefour][ligne][colonne], 0, IPC_RMID, 0);		
			}
		}
	}
}

void afficheEtatSem(){
	int ligne,colonne,numCarrefour;
	printf("Etat des sémaphores\n");
	for(numCarrefour = 0; numCarrefour<4; numCarrefour++){
		for(ligne = 0; ligne<2; ligne++){
			for(colonne = 0; colonne<2; colonne++){	
				//printf("%lld\t",sem_in_out[numCarrefour][ligne][colonne]);
				printf("%d\t", semctl(sem_in_out[numCarrefour][ligne][colonne], 0, GETVAL, 0));		
			}
			printf("\n");
		}
		printf("\n\n");
	}
	printf("\n");
}


void tourneDroite(voiture v){
int ligne,colonne;
int numCarrefour=v.numCarrefour;
		switch (v.entree){
		case OUEST:	
		{				
			ligne=1;
			colonne=0;
			break;
		}
		case SUD:	
		{			
			ligne=1;
			colonne=1;
			break;
		}
		case EST:	
		{				
			ligne=0;
			colonne=1;
			break;
		}
		case NORD:	
		{				
			ligne=0;
			colonne=0;
			break;
		}
	}
	PSem(sem_in_out[numCarrefour][ligne][colonne]);
	printf("%d P %d, %d\n",v.id, ligne, colonne);
	//printf("envoie voiture dans le tube correspondant\n");		
	VSem(sem_in_out[numCarrefour][ligne][colonne]);
	printf("%d V %d, %d\n",v.id, ligne, colonne);	
}

void enFace(voiture v){
	int lDeb, lFin, cDeb, cFin, k;
	int numCarrefour=v.numCarrefour;
	switch (v.entree){
		case OUEST:	
		{				
			lDeb=1;
			lFin=1;
			cDeb=0;
			cFin=1;
			k=1;
			break;
		}
		case SUD:	
		{			
			lDeb=1;
			lFin=0;
			cDeb=1;
			cFin=1;
			k=-1;
			break;
		}
		case EST:	
		{				
			lDeb=0;
			lFin=0;
			cDeb=1;
			cFin=0;
			k=-1;
			break;
		}
		case NORD:	
		{				
			lDeb=0;
			lFin=1;
			cDeb=0;
			cFin=0;
			k=1;
			break;
		}
	}
	if(lDeb==lFin){
		int C, L;
		PSem(sem_in_out[numCarrefour][lDeb][cDeb]);
			printf("%d P %d, %d\n",v.id, lDeb, cDeb);
		
		for(C=cDeb+1*k;C*k<=cFin*k;C=C+k){
			PSem(sem_in_out[numCarrefour][lDeb][C]);
			printf("%d P %d, %d\n",v.id, lDeb, C);					
			VSem(sem_in_out[numCarrefour][lDeb][C-1*k]);
			printf("%d V %d, %d\n",v.id, lDeb, C-1*k);				
		}

		//printf("envoie voiture dans le tube correspondant\n");		

		VSem(sem_in_out[numCarrefour][lFin][cFin]);
		printf("%d V %d, %d\n",v.id, lFin, cFin);
	}
	else{	
		int C, L;
		PSem(sem_in_out[numCarrefour][lDeb][cDeb]);
			printf("%d P %d, %d\n",v.id, lDeb, cDeb);
		for(L=lDeb+1*k;L*k<=lFin*k;L=L+k){
			PSem(sem_in_out[numCarrefour][L][cFin]);
			printf("%d P %d, %d\n",v.id, L, cFin);
			VSem(sem_in_out[numCarrefour][L-1*k][cFin]);
			printf("%d V %d, %d\n",v.id, L-1*k, cFin);
		}
		//printf("envoie voiture dans le tube correspondant\n");		
		VSem(sem_in_out[numCarrefour][lFin][cFin]);
		printf("%d V %d, %d\n",v.id, lFin, cFin);
	}
}

void tourneGauche(voiture v){
	int lDeb, lFin, cDeb, cFin, k, m;
	int numCarrefour=v.numCarrefour;
	switch (v.entree){
		case OUEST:	
		{				
			lDeb=1;
			lFin=0;
			cDeb=0;
			cFin=1;
			k=1;
			m=-1;
			break;
		}
		case SUD:	
		{			
			lDeb=1;
			lFin=0;
			cDeb=1;
			cFin=0;
			k=-1;
			m=-1;
			break;
		}
		case EST:	
		{				
			lDeb=0;
			lFin=1;
			cDeb=1;
			cFin=0;
			k=-1;
			m=1;
			break;
		}
		case NORD:	
		{				
			lDeb=0;
			lFin=1;
			cDeb=0;
			cFin=1;
			k=1;
			m=1;
			break;
		}
	}

	if(k!=m){
		int C, L;
		PSem(sem_in_out[numCarrefour][lDeb][cDeb]);
			printf("%d P %d, %d\n",v.id, lDeb, cDeb); 
		for(C=cDeb+1*k;C*k<=cFin*k;C=C+k){
			PSem(sem_in_out[numCarrefour][lDeb][C]);
			printf("%d P %d, %d\n",v.id, lDeb, C);
			VSem(sem_in_out[numCarrefour][lDeb][C-1*k]);
			printf("%d V %d, %d\n",v.id, lDeb, C-1*k);				
		}
		
		for(L=lDeb+1*m;L*m<=lFin*m;L=L+m){
			PSem(sem_in_out[numCarrefour][L][cFin]);
			printf("%d P %d, %d\n",v.id, L, cFin);
			VSem(sem_in_out[numCarrefour][L-1*m][cFin]);
			printf("%d V %d, %d\n",v.id, L-1*m, cFin);
		}
		//envoie dans tube
		//printf("envoie voiture dans le tube correspondant\n");		
		VSem(sem_in_out[numCarrefour][lFin][cFin]);
		printf("%d V %d, %d\n",v.id, lFin, cFin);
	}
	else{	
		int C, L;
		PSem(sem_in_out[numCarrefour][lDeb][cDeb]);
		printf("%d P %d, %d\n",v.id, lDeb, cDeb);

		for(L=lDeb+1*m;L*m<=lFin*m;L=L+m){
			PSem(sem_in_out[numCarrefour][L][cDeb]);
			printf("%d P %d, %d\n",v.id, L, cDeb); 
			VSem(sem_in_out[numCarrefour][L-1*m][cDeb]);
			printf("%d V %d, %d\n",v.id, L-1*m, cDeb);
		}
		for(C=cDeb+1*k;C*k<=cFin*k;C=C+k){
			PSem(sem_in_out[numCarrefour][lFin][C]);
			printf("%d P %d, %d\n",v.id, lFin, C);
			VSem(sem_in_out[numCarrefour][lFin][C-1*k]);
			printf("%d V %d, %d\n",v.id, lFin, C-1*k);				
		}

		//envoie dans tube
		//printf("envoie voiture dans le tube correspondant\n");		
		VSem(sem_in_out[numCarrefour][lFin][cFin]);
		printf("%d V %d, %d\n",v.id, lFin, cFin);
	}
}

void traitement(mess* message)
{
	int numCarrefourSvt=-1;
	mess messageAEnvoyer;
	messageAEnvoyer.type=message->type;
	messageAEnvoyer.car=message->car;

	//modification sortie par le serveur-controleur

	//message->car.sortie=nouvelleSortie;

	printf("Entree dans le carrefour %d voiture %d par l'entree %d.\n", message->car.numCarrefour, message->car.id, message->car.entree);
	int destination = (message->car.entree)-(message->car.sortie);
	if(destination==-1 || destination==3){
		//printf("droite\n");
		tourneDroite(message->car);
	}
	else if(destination==-2 || destination==2){
		//printf("enface\n");
		enFace(message->car);
	}
	else if(destination==-3 || destination==1){
		//printf("gauche\n");
		tourneGauche(message->car);
	}
	else{
		printf("ERREUR: demi tour impossible\n");
	}
	printf("Voiture id:%d, sort du carrefour %d par la sortie %d!\n", message->car.id, message->car.numCarrefour, message->car.sortie);

		switch(message->car.numCarrefour){
		case 0:
		{
			switch(message->car.sortie){
			case OUEST:
				printf("Sortie de la voiture %d par la sortie OUEST du carrefour 0\n",message->car.id);
				messageAEnvoyer.car.entree=EST;				
				break;
			case SUD:
				numCarrefourSvt=2;
				messageAEnvoyer.car.entree=NORD;
				break;
			case EST:
				numCarrefourSvt=1;
				messageAEnvoyer.car.entree=OUEST;
				break;
			case NORD:
				printf("Sortie de la voiture %d par la sortie NORD du carrefour 0\n",message->car.id);
				messageAEnvoyer.car.entree=SUD;
				break;
			}
			break;
		}
		case 1:
		{
			switch(message->car.sortie){
			case OUEST:
				numCarrefourSvt=0;
				messageAEnvoyer.car.entree=EST;
				break;
			case SUD:
				numCarrefourSvt=3;
				messageAEnvoyer.car.entree=NORD;
				break;
			case EST:
				printf("Sortie de la voiture %d par la sortie EST du carrefour 1\n",message->car.id);
				messageAEnvoyer.car.entree=OUEST;
				break;
			case NORD:
				printf("Sortie de la voiture %d par la sortie NORD du carrefour 1\n",message->car.id);
				messageAEnvoyer.car.entree=SUD;
				break;
			}
			break;	
		}
		case 2:
		{
			switch(message->car.sortie){
			case OUEST:
				printf("Sortie de la voiture %d par la sortie OUEST du carrefour 2\n",message->car.id);
				messageAEnvoyer.car.entree=EST;
				break;
			case SUD:
				printf("Sortie de la voiture %d par la sortie SUD du carrefour 2\n",message->car.id);
				messageAEnvoyer.car.entree=NORD;
				break;
			case EST:
				numCarrefourSvt=3;
				messageAEnvoyer.car.entree=OUEST;				
				break;
			case NORD:
				numCarrefourSvt=0;
				messageAEnvoyer.car.entree=SUD;		
				break;
			}
			break;
		}
		case 3:
		{
	switch(message->car.sortie){
			case OUEST:
				numCarrefourSvt=2;		
				messageAEnvoyer.car.entree=EST;		
				break;
			case SUD:
				printf("Sortie de la voiture %d par la sortie SUD du carrefour 3\n",message->car.id);
				messageAEnvoyer.car.entree=NORD;		
				break;
			case EST:
				printf("Sortie de la voiture %d par la sortie EST du carrefour 3\n",message->car.id);	
				messageAEnvoyer.car.entree=OUEST;			
				break;
			case NORD:
				numCarrefourSvt=1;		
				messageAEnvoyer.car.entree=SUD;		
				break;
			}
			break;
		}
	}




	//ecriture file message carrefour correspondant
	if(numCarrefourSvt!=-1){
		messageAEnvoyer.car.numCarrefour=numCarrefourSvt;
		msgsnd(msgid[numCarrefourSvt], &messageAEnvoyer, sizeof(mess) - sizeof(long), 0);

		if(messageAEnvoyer.car.prioritaire==VRAI){
			memoiresPartagees[messageAEnvoyer.car.numCarrefour][messageAEnvoyer.car.entree+3]++;
		}
		else{
			memoiresPartagees[messageAEnvoyer.car.numCarrefour][messageAEnvoyer.car.entree-1]++;
		}
	}



	free(message);	
	pthread_exit(0);
}






void gestionCarrefour(int numCarrefour){
	int ligne,colonne;
	pthread_t thread_traitement[4];

	//int* memoiresPartagees[4];
	int i;
	for(i=0;i<4;i++){
		memoiresPartagees[i]=(int*) shmat(idMemPartagee[i], NULL, NULL);
	}

	while(1){
		//allocation mess
		mess* mTemp = (mess*) malloc(sizeof(mess));


		//sleep(3);


		//test si vehicule prioritaire dans le carrefour
		int i, nbVoituresFile=50000, numFile=-1;

		////////////////////////////////////gerer le fait qu'il puisse y avoir plus voiture prioritaire dans une file???
		for(i=4;i<8;i++){
			if(memoiresPartagees[numCarrefour][i]>0){
				if(memoiresPartagees[numCarrefour][i-4]+memoiresPartagees[numCarrefour][4]<nbVoituresFile){
					numFile=i-3;	//1 OUEST, 2 SUD, 3 EST, 4 NORD
					//nb de voitures présentes dans la file ou le véhicule prioritaire se trouve
					nbVoituresFile=memoiresPartagees[numCarrefour][i-4]+memoiresPartagees[numCarrefour][4]; 
				}				
			}
		}

		affichageCarrefour(numCarrefour);
		nbMessageDansFile(numCarrefour);

		// attente voiture
		//printf("Attente de voiture carrefour numero %d...\n",numCarrefour);
		//reception voiture
		if(numFile!=-1){
			printf("il existe un vehicule prioritaire\n");
			printf("numero de la file ayant le moins de voitures : %d\n",numFile);
			msgrcv(msgid[numCarrefour], mTemp, sizeof(mess), numFile, 0);
		}
		else{
			printf("pas de vehicule prioritaire\n");
			msgrcv(msgid[numCarrefour], mTemp, sizeof(mess), 0, 0);
		}

		printf("voiture %d recue carrefour numero %d, voie %d...\n",mTemp->car.id, numCarrefour, mTemp->car.entree);

		//decrementation de la voie en fonction du fait qu'il existe un vehicule prioritaire 
		if(mTemp->car.prioritaire==VRAI){
			printf("decrementation vehicule prioritaire\n");
			memoiresPartagees[mTemp->car.numCarrefour][mTemp->car.entree+3]--;
		}
		else{
			printf("decrementation vehicule non prioritaire\n");
			memoiresPartagees[mTemp->car.numCarrefour][mTemp->car.entree-1]--;
		}

		int indice=(mTemp->car.entree)-1;
		//printf("indice thread %d",indice);
		
		//thread traiter voiture.
		int retour=pthread_create(&thread_traitement[indice], NULL, (void * (*)(void *))traitement, mTemp);		
		//printf("retour thread %d",retour);
	}
	//destruction semaphores du carrefour
	destructionSem();
}


