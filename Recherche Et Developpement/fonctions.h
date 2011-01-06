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
	pthread_mutex_lock(&memPart);		
	for(numVoie = 0; numVoie<4; numVoie++){	
		printf("Voie numero %d : np:%d  p:%d\n",numVoie+1, memoiresPartagees[carrefour][numVoie], memoiresPartagees[carrefour][numVoie+4]);		
	}
	pthread_mutex_unlock(&memPart);
	printf("\n");
}

/*void affichageCarrefours(){
	int carrefour, numVoie;	
	for(carrefour = 0; carrefour<4; carrefour++){
		affichageCarrefour(carrefour);
	}
}*/


void affichageCarrefours(){
char* tabChar[4][8];
//tabChar = new char[4];//[8];
//char* tabChar = (mess*) malloc(sizeof(mess));
char buffer[3];
char* maValeur;

maValeur="aa";

int i,j;
for(i=0;i<4;i++){
	for(j=0;j<8;j++){
		if(memoiresPartagees[i][j]<10){
			sprintf(buffer, "%d ", memoiresPartagees[i][j]	);
		}	
		else{
			sprintf(buffer, "%d", memoiresPartagees[i][j]);
		}	
		tabChar[i][j]=buffer;
				
		maValeur=buffer;
		//printf("i%d, j%d, int %d, char %s\n", i,j,memoiresPartagees[i][j],tabChar[i][j]);
	}	
}




/*
printf("##%s##\n",tabChar[0][0]);
printf("##  ##\n");
*/
/*printf("\n                  #   |   #                    #   |   #          \n	          #   |   #                    #   |   #          \n	          #   |   #                    #   |   #          \n	          # %s|   #                    # %s|   #          \n	########### %s|   ###################### %s|   ###########\n	              |   %s %s                    |   %s %s      \n	----------------------------------------------------------\n	      %s %s   |                    %s %s   |              \n	###########   |%s ######################   |%s ###########\n	          #   |%s #                    #   |%s #          \n	          #   |   #                    #   |   #          \n	          #   |   #                    #   |   #          \n	          #   |   #                    #   |   #          \n	          #   |   #                    #   |   #          \n	          # %s|   #                    # %s|   #          \n	########### %s|   ###################### %s|   ###########\n	              |   %s %s                    |   %s %s      \n	----------------------------------------------------------\n	      %s %s   |                    %s %s   |              \n	###########   |%s ######################   |%s ###########\n	          #   |%s #                    #   |%s #          \n	          #   |   #                    #   |   #          \n	          #   |   #                    #   |   #          \n	          #   |   #                    #   |   #          \n",tabChar[0][3+4],tabChar[1][3+4],tabChar[0][3],tabChar[1][3],tabChar[0][2],tabChar[0][2+4],tabChar[1][2],tabChar[1][2+4],tabChar[0][0+4],tabChar[0][0],tabChar[1][0+4],tabChar[1][0],tabChar[0][1],tabChar[1][1],tabChar[0][1+4],tabChar[1][1+4],tabChar[2][3+4],tabChar[3][3+4],tabChar[2][3],tabChar[3][3],tabChar[2][2],tabChar[2][2+4],tabChar[3][2],tabChar[3][2+4],tabChar[2][0+4],tabChar[2][0],tabChar[3][0+4],tabChar[3][0],tabChar[2][1],tabChar[3][1],tabChar[2][1+4],tabChar[3][1+4]);*/

pthread_mutex_lock(&memPart);

printf("\n                  #   |   #                    #   |   #          \n	          #   |   #                    #   |   #          \n	          #   |   #                    #   |   #          \n	          # %2d|   #                    # %2d|   #          \n	########### %2d|   ###################### %2d|   ###########\n	              |   %2d %2d                    |   %2d %2d      \n	----------------------------------------------------------\n	      %2d %2d   |                    %2d %2d   |              \n	###########   |%2d ######################   |%2d ###########\n	          #   |%2d #                    #   |%2d #          \n	          #   |   #                    #   |   #          \n	          #   |   #                    #   |   #          \n	          #   |   #                    #   |   #          \n	          #   |   #                    #   |   #          \n	          # %2d|   #                    # %2d|   #          \n	########### %2d|   ###################### %2d|   ###########\n	              |   %2d %2d                    |   %2d %2d      \n	----------------------------------------------------------\n	      %2d %2d   |                    %2d %2d   |              \n	###########   |%2d ######################   |%2d ###########\n	          #   |%2d #                    #   |%2d #          \n	          #   |   #                    #   |   #          \n	          #   |   #                    #   |   #          \n	          #   |   #                    #   |   #          \n",memoiresPartagees[0][3+4],memoiresPartagees[1][3+4],memoiresPartagees[0][3],memoiresPartagees[1][3],memoiresPartagees[0][2],memoiresPartagees[0][2+4],memoiresPartagees[1][2],memoiresPartagees[1][2+4],memoiresPartagees[0][0+4],memoiresPartagees[0][0],memoiresPartagees[1][0+4],memoiresPartagees[1][0],memoiresPartagees[0][1],memoiresPartagees[1][1],memoiresPartagees[0][1+4],memoiresPartagees[1][1+4],memoiresPartagees[2][3+4],memoiresPartagees[3][3+4],memoiresPartagees[2][3],memoiresPartagees[3][3],memoiresPartagees[2][2],memoiresPartagees[2][2+4],memoiresPartagees[3][2],memoiresPartagees[3][2+4],memoiresPartagees[2][0+4],memoiresPartagees[2][0],memoiresPartagees[3][0+4],memoiresPartagees[3][0],memoiresPartagees[2][1],memoiresPartagees[3][1],memoiresPartagees[2][1+4],memoiresPartagees[3][1+4]);
pthread_mutex_unlock(&memPart);
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
	//printf("%d P %d, %d\n",v.id, ligne, colonne);
	//printf("envoie voiture dans le tube correspondant\n");		
	VSem(sem_in_out[numCarrefour][ligne][colonne]);
	//printf("%d V %d, %d\n",v.id, ligne, colonne);	
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
			//printf("%d P %d, %d\n",v.id, lDeb, cDeb);
		
		for(C=cDeb+1*k;C*k<=cFin*k;C=C+k){
			PSem(sem_in_out[numCarrefour][lDeb][C]);
			//printf("%d P %d, %d\n",v.id, lDeb, C);					
			VSem(sem_in_out[numCarrefour][lDeb][C-1*k]);
			//printf("%d V %d, %d\n",v.id, lDeb, C-1*k);				
		}

		//printf("envoie voiture dans le tube correspondant\n");		

		VSem(sem_in_out[numCarrefour][lFin][cFin]);
		//printf("%d V %d, %d\n",v.id, lFin, cFin);
	}
	else{	
		int C, L;
		PSem(sem_in_out[numCarrefour][lDeb][cDeb]);
			//printf("%d P %d, %d\n",v.id, lDeb, cDeb);
		for(L=lDeb+1*k;L*k<=lFin*k;L=L+k){
			PSem(sem_in_out[numCarrefour][L][cFin]);
			//printf("%d P %d, %d\n",v.id, L, cFin);
			VSem(sem_in_out[numCarrefour][L-1*k][cFin]);
			//printf("%d V %d, %d\n",v.id, L-1*k, cFin);
		}
		//printf("envoie voiture dans le tube correspondant\n");		
		VSem(sem_in_out[numCarrefour][lFin][cFin]);
		//printf("%d V %d, %d\n",v.id, lFin, cFin);
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
			//printf("%d P %d, %d\n",v.id, lDeb, cDeb); 
		for(C=cDeb+1*k;C*k<=cFin*k;C=C+k){
			PSem(sem_in_out[numCarrefour][lDeb][C]);
			//printf("%d P %d, %d\n",v.id, lDeb, C);
			VSem(sem_in_out[numCarrefour][lDeb][C-1*k]);
			//printf("%d V %d, %d\n",v.id, lDeb, C-1*k);				
		}
		
		for(L=lDeb+1*m;L*m<=lFin*m;L=L+m){
			PSem(sem_in_out[numCarrefour][L][cFin]);
			//printf("%d P %d, %d\n",v.id, L, cFin);
			VSem(sem_in_out[numCarrefour][L-1*m][cFin]);
			//printf("%d V %d, %d\n",v.id, L-1*m, cFin);
		}
		//envoie dans tube
		//printf("envoie voiture dans le tube correspondant\n");		
		VSem(sem_in_out[numCarrefour][lFin][cFin]);
		//printf("%d V %d, %d\n",v.id, lFin, cFin);
	}
	else{	
		int C, L;
		PSem(sem_in_out[numCarrefour][lDeb][cDeb]);
		//printf("%d P %d, %d\n",v.id, lDeb, cDeb);

		for(L=lDeb+1*m;L*m<=lFin*m;L=L+m){
			PSem(sem_in_out[numCarrefour][L][cDeb]);
			//printf("%d P %d, %d\n",v.id, L, cDeb); 
			VSem(sem_in_out[numCarrefour][L-1*m][cDeb]);
			//printf("%d V %d, %d\n",v.id, L-1*m, cDeb);
		}
		for(C=cDeb+1*k;C*k<=cFin*k;C=C+k){
			PSem(sem_in_out[numCarrefour][lFin][C]);
			//printf("%d P %d, %d\n",v.id, lFin, C);
			VSem(sem_in_out[numCarrefour][lFin][C-1*k]);
			//printf("%d V %d, %d\n",v.id, lFin, C-1*k);				
		}

		//envoie dans tube
		//printf("envoie voiture dans le tube correspondant\n");		
		VSem(sem_in_out[numCarrefour][lFin][cFin]);
		//printf("%d V %d, %d\n",v.id, lFin, cFin);
	}
}

void traitement(mess* message)
{
	int numCarrefourSvt=-1;
	mess messageAEnvoyer;
	messageAEnvoyer.type=message->type;
	messageAEnvoyer.car=message->car;

	//modification sortie par le serveur-controleur
	ReqEchgeur msg;

	msg.type=1;
	// numero du thread 
	msg.pidEchgeur=message->car.id;
	msg.idOrigine=message->car.numCarrefour;
	msg.voieOrigine=message->car.entree;
	msg.idDest=message->car.numCarrefourFinal;
	msg.voieDest=message->car.sortieFinale;

	msgsnd(msgidServeurControleur, &msg, sizeof(ReqEchgeur)-sizeof(long), 0);
	//reception par la SC 				msgrcv(msgid, msg, sizeof(ReqEchgeur), 1, 0);
	//message envoye par le SC			msgsnd(msgid, &Reponse, sizeof(RepCtrleur) - sizeof(long), 0);

	RepCtrleur msgRep;

	msgrcv(msgidServeurControleur, &msgRep, sizeof(RepCtrleur), message->car.id, 0);

	message->car.sortie=msgRep.voieDest;

	printf("voieDest%d\n",msgRep.voieDest);


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


		//proteger par mutex

		/*if(messageAEnvoyer.car.prioritaire==VRAI){
			pthread_mutex_lock(&memPart);
			printf("incrementation vehicule prioritaire carr%d file%d\n", messageAEnvoyer.car.numCarrefour, messageAEnvoyer.car.entree);
			memoiresPartagees[messageAEnvoyer.car.numCarrefour][messageAEnvoyer.car.entree+3]++;
			pthread_mutex_unlock(&memPart);
		}
		else{
			pthread_mutex_lock(&memPart);
			printf("incrementation vehicule non prioritaire carr%d file%d\n", messageAEnvoyer.car.numCarrefour, messageAEnvoyer.car.entree);
			memoiresPartagees[messageAEnvoyer.car.numCarrefour][messageAEnvoyer.car.entree-1]++;
			pthread_mutex_unlock(&memPart);
		}
		
		
		msgsnd(msgid[numCarrefourSvt], &messageAEnvoyer, sizeof(mess) - sizeof(long), 0);*/
		printf("envoi voiture\n");
		envoiVoiture(messageAEnvoyer);
	}


	free(message);	
	pthread_exit(0);
}






void gestionCarrefour(int numCarrefour){
	//sleep(5);
	int ligne,colonne;
	pthread_t thread_traitement[4];

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


		//affichageCarrefours();


		////////////////////////////////////gerer le fait qu'il puisse y avoir plus voiture prioritaire dans une file???
		for(i=4;i<8;i++){
			pthread_mutex_lock(&memPart);
			// si il existe un véhicule prioritaire attendant au carrefour
			if(memoiresPartagees[numCarrefour][i]>0){
				// choix de la file ayant le moins de voiture dans le cas ou il y a plusieurs vehicules prioritaires a un carrefour
				if(memoiresPartagees[numCarrefour][i-4]+memoiresPartagees[numCarrefour][4]<nbVoituresFile){
					numFile=i-3;	//1 OUEST, 2 SUD, 3 EST, 4 NORD
					//nb de voitures présentes dans la file ou le véhicule prioritaire se trouve
					nbVoituresFile=memoiresPartagees[numCarrefour][i-4]+memoiresPartagees[numCarrefour][4]; 
				}				
			}
			pthread_mutex_unlock(&memPart);
		}

		//affichageCarrefours(numCarrefour);
		//nbMessageDansFile(numCarrefour);

		// attente voiture
		//printf("Attente de voiture carrefour numero %d...\n",numCarrefour);
		//reception voiture
		if(numFile!=-1){
			printf("numFile :%d\n",numFile);
			printf("il existe un vehicule prioritaire\n");
			printf("numero de la file ayant le moins de voitures : %d\n",numFile);
			//traite le vehicule se trouvant dans la file contenant un vehicule prioritaire 
			msgrcv(msgid[numCarrefour], mTemp, sizeof(mess), numFile, 0);
		}
		else{
			printf("pas de vehicule prioritaire\n");
			//traite le vehicule qui est arrivé en premier
			msgrcv(msgid[numCarrefour], mTemp, sizeof(mess), 0, 0);
		}

		printf("voiture %d recue carrefour numero %d, voie %d...\n",mTemp->car.id, numCarrefour, mTemp->car.entree);

		//decrementation de la voie en fonction du fait qu'il existe un vehicule prioritaire 
		if(mTemp->car.prioritaire==VRAI){
			printf("decrementation vehicule prioritaire\n");
			pthread_mutex_lock(&memPart);
			memoiresPartagees[mTemp->car.numCarrefour][mTemp->car.entree+3]--;
			pthread_mutex_unlock(&memPart);
		}
		else{
			printf("decrementation vehicule non prioritaire\n");
			pthread_mutex_lock(&memPart);
			memoiresPartagees[mTemp->car.numCarrefour][mTemp->car.entree-1]--;
			pthread_mutex_unlock(&memPart);
		}

		int indice=(mTemp->car.entree)-1;
		//printf("indice thread %d",indice);
		
		//thread traiter voiture.
		int retour=pthread_create(&thread_traitement[indice], NULL, (void * (*)(void *))traitement, mTemp);		
		//printf("retour thread %d",retour);

		usleep(5000);
		//affichageCarrefours();

	}
	//destruction semaphores du carrefour
	destructionSem();
}
void envoiVoiture(mess messageAEnvoyer){
	if(messageAEnvoyer.car.prioritaire==VRAI){
		pthread_mutex_lock(&memPart);
		printf("incrementation vehicule prioritaire carr%d file%d\n", messageAEnvoyer.car.numCarrefour, messageAEnvoyer.car.entree);
		memoiresPartagees[messageAEnvoyer.car.numCarrefour][messageAEnvoyer.car.entree+3]++;
		pthread_mutex_unlock(&memPart);
	}
	else{
		pthread_mutex_lock(&memPart);
		printf("incrementation vehicule non prioritaire carr%d file%d\n", messageAEnvoyer.car.numCarrefour, messageAEnvoyer.car.entree);
		memoiresPartagees[messageAEnvoyer.car.numCarrefour][messageAEnvoyer.car.entree-1]++;
		pthread_mutex_unlock(&memPart);
	}

	msgsnd(msgid[messageAEnvoyer.car.numCarrefour], &messageAEnvoyer, sizeof(mess) - sizeof(long), 0);
}


void creerVoiture(){
	mess messageAEnvoyer;
	cptVoitures++;
	messageAEnvoyer.car.id = cptVoitures;
			 
	messageAEnvoyer.car.numCarrefour = rand()%4; 
	
	int numEntree = rand()%2;
	switch(messageAEnvoyer.car.numCarrefour)
	{
		case 0:
			messageAEnvoyer.car.entree = (numEntree+3)%4+1;
		break;
		case 1:
			messageAEnvoyer.car.entree = numEntree+3;
		break;
		case 2:
			messageAEnvoyer.car.entree = numEntree+1;
		break;
		case 3:
			messageAEnvoyer.car.entree = numEntree+2;
		break;
	}
	messageAEnvoyer.car.numCarrefourFinal = rand()%4;
	int numSortie;
	switch(messageAEnvoyer.car.numCarrefourFinal)
	{
		case 0:
			do{
				numSortie = rand()%2;
				messageAEnvoyer.car.sortieFinale = (numSortie+3)%4+1;
			}while(messageAEnvoyer.car.numCarrefour == messageAEnvoyer.car.numCarrefourFinal 
				&& messageAEnvoyer.car.entree == messageAEnvoyer.car.sortieFinale);
		break;
		case 1:
			do{
				numSortie = rand()%2;
				messageAEnvoyer.car.sortieFinale = numSortie+3;
			}while(messageAEnvoyer.car.numCarrefour == messageAEnvoyer.car.numCarrefourFinal 
				&& messageAEnvoyer.car.entree == messageAEnvoyer.car.sortieFinale);
		break;
		case 2:
			do{
				numSortie = rand()%2;
				messageAEnvoyer.car.sortieFinale = numSortie+1;
			}while(messageAEnvoyer.car.numCarrefour == messageAEnvoyer.car.numCarrefourFinal 
				&& messageAEnvoyer.car.entree == messageAEnvoyer.car.sortieFinale);
		break;
		case 3:
			do{
				numSortie = rand()%2;
				messageAEnvoyer.car.sortieFinale = numSortie+2;
			}while(messageAEnvoyer.car.numCarrefour == messageAEnvoyer.car.numCarrefourFinal 
				&& messageAEnvoyer.car.entree == messageAEnvoyer.car.sortieFinale);
		break;
	}  
	messageAEnvoyer.car.prioritaire=FAUX;//(int)rand()%2;
	messageAEnvoyer.type = messageAEnvoyer.car.entree;
	//sleep(1);
	envoiVoiture(messageAEnvoyer);
}






