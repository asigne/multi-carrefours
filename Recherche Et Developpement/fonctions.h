#include "voiture.h"

int creerSem(int clef){
	int semid = semget(clef, 1, IPC_CREAT | IPC_EXCL | 0666);
	return semid;
}

void initSem(int id, int valeur){
	semctl(id, 0, SETVAL, valeur); 
}

int PSem(int id){
	sembuf op;
     
	op.sem_num = 0; //Numéro de notre sémaphore
	op.sem_op = -1; //Pour un P() on décrémente
	op.sem_flg = 0; //On ne s'en occupe pas
    return semop(id, &op, 1); //Entrée dans la section critique P()
}

int VSem(int id){
	sembuf op;
     
	op.sem_num = 0; //Numéro de notre sémaphore
	op.sem_op = 1; //Pour un V() on incrémente
	op.sem_flg = 0; //On ne s'en occupe pas

    semop(id, &op, 1); //Sortie de la section critique V()
}

void destructionSem(){
	int ligne,colonne;
	 //Destruction du sémaphore
	for(ligne = 0; ligne<6; ligne++){
		for(colonne = 0; colonne<6; colonne++){	
			semctl(sem_in_out[ligne][colonne], 0, IPC_RMID, 0);		
		}
	}
}

void afficheEtatSem(){
	int ligne,colonne;
	printf("Etat des sémaphores\n");
	for(ligne = 0; ligne<6; ligne++){
		for(colonne = 0; colonne<6; colonne++){	
			printf("%lld\t",sem_in_out[ligne][colonne]);
		}
		printf("\n");
	}
	printf("\n");
}


void tourneDroite(voiture v){
int ligne,colonne;
		switch (v.entree){
		case OUEST:	
		{				
			ligne=5;
			colonne=0;
			break;
		}
		case SUD:	
		{			
			ligne=5;
			colonne=5;
			break;
		}
		case EST:	
		{				
			ligne=0;
			colonne=5;
			break;
		}
		case NORD:	
		{				
			ligne=0;
			colonne=0;
			break;
		}
	}
	PSem(sem_in_out[ligne][colonne]);
	printf("%d P %d, %d\n",v.id, ligne, colonne);
	//printf("envoie voiture dans le tube correspondant\n");		
	VSem(sem_in_out[ligne][colonne]);
	printf("%d V %d, %d\n",v.id, ligne, colonne);	
}

void enFace(voiture v){
	int lDeb, lFin, cDeb, cFin, k;

	//a voir 
	mess m;
	m.car.id = v.id;
	m.car.sortie = v.sortie; //sortie donnee par le serveur-controleur (numero msgid)
	m.car.entree = v.entree;
	m.type = 1;	

	switch (v.entree){
		case OUEST:	
		{				
			lDeb=4;
			lFin=4;
			cDeb=0;
			cFin=5;
			k=1;
			break;
		}
		case SUD:	
		{			
			lDeb=5;
			lFin=0;
			cDeb=4;
			cFin=4;
			k=-1;
			break;
		}
		case EST:	
		{				
			lDeb=1;
			lFin=1;
			cDeb=5;
			cFin=0;
			k=-1;
			break;
		}
		case NORD:	
		{				
			lDeb=0;
			lFin=5;
			cDeb=1;
			cFin=1;
			k=1;
			break;
		}
	}
	if(lDeb==lFin){
		int C, L;
		PSem(sem_in_out[lDeb][cDeb]);
			printf("%d P %d, %d\n",v.id, lDeb, cDeb);
		for(C=cDeb+1*k;C*k<=cFin*k;C=C+k){
			PSem(sem_in_out[lDeb][C]);
			printf("%d P %d, %d\n",v.id, lDeb, C);
			VSem(sem_in_out[lDeb][C-1*k]);
			printf("%d V %d, %d\n",v.id, lDeb, C-1*k);				
		}

		//printf("envoie voiture dans le tube correspondant\n");		

		VSem(sem_in_out[lFin][cFin]);
		printf("%d V %d, %d\n",v.id, lFin, cFin);
	}
	else{	
		int C, L;
		PSem(sem_in_out[lDeb][cDeb]);
			printf("%d P %d, %d\n",v.id, lDeb, cDeb);
		for(L=lDeb+1*k;L*k<=lFin*k;L=L+k){
			PSem(sem_in_out[L][cFin]);
			printf("%d P %d, %d\n",v.id, L, cFin);
			VSem(sem_in_out[L-1*k][cFin]);
			printf("%d V %d, %d\n",v.id, L-1*k, cFin);
		}
		//printf("envoie voiture dans le tube correspondant\n");		
		VSem(sem_in_out[lFin][cFin]);
		printf("%d V %d, %d\n",v.id, lFin, cFin);
	}
}

void tourneGauche(voiture v){
	int lDeb, lFin, cDeb, cFin, k, m;
	int noFileMessage=-1;
	switch (v.entree){
		case OUEST:	
		{				
			lDeb=3;
			lFin=0;
			cDeb=0;
			cFin=3;
			k=1;
			m=-1;
			break;
		}
		case SUD:	
		{			
			lDeb=5;
			lFin=2;
			cDeb=3;
			cFin=0;
			k=-1;
			m=-1;
			break;
		}
		case EST:	
		{				
			lDeb=2;
			lFin=5;
			cDeb=5;
			cFin=2;
			k=-1;
			m=1;
			break;
		}
		case NORD:	
		{				
			lDeb=0;
			lFin=3;
			cDeb=2;
			cFin=5;
			k=1;
			m=1;
			break;
		}
	}

	if(k!=m){
		int C, L;
		PSem(sem_in_out[lDeb][cDeb]);
			printf("%d P %d, %d\n",v.id, lDeb, cDeb);
		for(C=cDeb+1*k;C*k<=cFin*k;C=C+k){
			PSem(sem_in_out[lDeb][C]);
			printf("%d P %d, %d\n",v.id, lDeb, C);
			VSem(sem_in_out[lDeb][C-1*k]);
			printf("%d V %d, %d\n",v.id, lDeb, C-1*k);				
		}
		
		for(L=lDeb+1*m;L*m<=lFin*m;L=L+m){
			PSem(sem_in_out[L][cFin]);
			printf("%d P %d, %d\n",v.id, L, cFin);
			VSem(sem_in_out[L-1*m][cFin]);
			printf("%d V %d, %d\n",v.id, L-1*m, cFin);
		}
		//envoie dans tube
		//printf("envoie voiture dans le tube correspondant\n");		
		VSem(sem_in_out[lFin][cFin]);
		printf("%d V %d, %d\n",v.id, lFin, cFin);
	}
	else{	
		int C, L;
		PSem(sem_in_out[lDeb][cDeb]);
		printf("%d P %d, %d\n",v.id, lDeb, cDeb);

		for(L=lDeb+1*m;L*m<=lFin*m;L=L+m){
			PSem(sem_in_out[L][cDeb]);
			printf("%d P %d, %d\n",v.id, L, cDeb);
			VSem(sem_in_out[L-1*m][cDeb]);
			printf("%d V %d, %d\n",v.id, L-1*m, cDeb);
		}
		for(C=cDeb+1*k;C*k<=cFin*k;C=C+k){
			PSem(sem_in_out[lFin][C]);
			printf("%d P %d, %d\n",v.id, lFin, C);
			VSem(sem_in_out[lFin][C-1*k]);
			printf("%d V %d, %d\n",v.id, lFin, C-1*k);				
		}

		//envoie dans tube
		//printf("envoie voiture dans le tube correspondant\n");		
		VSem(sem_in_out[lFin][cFin]);
		printf("%d V %d, %d\n",v.id, lFin, cFin);
	}
}

void traitement(mess* message)
{
	voiture* v = &(message->car);

	//modification sortie par le serveur-controleur

	switch(v->sortie-v->entree) //difference entre le numero de la sortie de celui de l'entrée
	{
		// v->entree numéro de l'entrée
		case 1: //tourne a droite
		{
		//	printf("Entree dans le carrefour voiture %d par l'entree %d.\n", v->id, v->entree);			
			tourneDroite(*v);
			printf("Voiture id:%d, sort  carrefour !\n", v->id);
			break;
		}
		case 2: //va en face
		{
		//	printf("Entree dans le carrefour voiture %d par l'entree %d.\n", v->id, v->entree);
			enFace(*v);
			printf("Voiture id:%d, sort  carrefour !\n", v->id);
			break;
		}
		case 3: //tourne a gauche
		{
		//	printf("Entree dans le carrefour voiture %d par l'entree %d.\n", v->id, v->entree);
			tourneGauche(*v);
			printf("Voiture id:%d, sort  carrefour !\n", v->id);
			break;
		}
	}	
	pthread_exit(0);
}






void gestionCarrefour(int numCarrefour){
	int ligne,colonne;
	key_t clef;
	int semid,cpt=0;
	pthread_t thread_traitement[3];
	mess* tabMess = malloc (3 * sizeof(mess));

	//créer les 36 sémaphores du carrefour
	for(ligne = 0; ligne<6; ligne++){
		for(colonne = 0; colonne<6; colonne++){	
			clef = ftok("test", ID_PROJET+cpt+numCarrefour*100); //verifier unicité des clefs générée
			sem_in_out[ligne][colonne]=creerSem(clef);
			initSem(sem_in_out[ligne][colonne], 1);
			cpt++;		
		}
	}

	while(1){
		//allocation mess
		mess* mTemp = (mess*) malloc(sizeof(mess));

		// attente voiture
		//printf("Attente de voiture carrefour numero:%d...\n",numCarrefour);
		//reception voiture
		msgrcv(msgid[numCarrefour], mTemp, sizeof(mess), 0, 0);
		int indice=(mTemp->car.sortie)-(mTemp->car.entree)-1;
		tabMess[indice].car=mTemp->car;
		tabMess[indice].type=mTemp->type;
	//	printf("voiture recue carrefour numero:%d...\n",numCarrefour);
		//thread traiter voiture.
		pthread_create(&thread_traitement[indice], NULL, (void * (*)(void *))traitement, &tabMess[indice]);		
		//liberation mess
		free(mTemp);
	}
	//destruction semaphores du carrefour
	destructionSem();
}


