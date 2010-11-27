
#include "voiture.h"


int main()
{
	int msgid;
	if((msgid = msgget(666, IPC_CREAT | IPC_EXCL | 0666)) == -1)
	{
		//perror(strerror(errno));
	}
	
	int pid = fork();
	
	if(pid == 0)
	{
		// carrefour
		
		int i;
		
		for(i = 0; i<4; i++){
			sem_in_out[i]=creerSem(i+10000);
			initSem(sem_in_out[i], 1);
		}
		
		pthread_t thread_traitement[3];
		
		while(1)
		{
			// attendre voiture
			mess* m = (mess*) malloc(sizeof(mess));
			
			printf("Attente de voiture...\n");
			
			msgrcv(msgid, m, sizeof(mess), 0, 0);
			
			//printf("Lecture message...\n");
			
			// file de la voiture = m.type
			// les infos de la voiture = m.car
			
			//voiture* v = m.car;
			
			//printf("MSG type = %d\n", (int)m->type);
			
			m->car.entree = m->type-1;
			
			//printf("Message de la voiture %d.\n", m->car.id);
			
			// crée thread traiter voiture.
			
			//pthread_t thread_traitement;
			pthread_create(&thread_traitement[m->type-1], NULL, (void * (*)(void *))traitement, m);
			
			//printf("lancement thread...\n");
			
			//pthread_join(thread_traitement, NULL);
			
			//usleep(100);
			
		}
		
	} else {
		// pere
		
		//sleep(2);
		
		mess m;
		m.car.id = 11;
		m.car.sortie = EN_FACE;
		m.car.entree = -1;
		m.type = 1;
		
		mess m1;
		m1.car.id = 15;
		m1.car.sortie = EN_FACE;
		m1.car.entree = -1;
		m1.type = 4;
		
		
		mess m2;
		m2.car.id = 12;
		m2.car.sortie = EN_FACE;
		m2.car.entree = -1;
		m2.type = 2;
		
		
		msgsnd(msgid, &m, sizeof(mess) - sizeof(long), 0);
		//msgsnd(msgid, &m2, sizeof(mess) - sizeof(long), 0);
		sleep(1);
		msgsnd(msgid, &m1, sizeof(mess) - sizeof(long), 0);
		
		
		//printf("2 Nouvelle voiture dans la file...\n");
		
		wait();
		
		printf("Fils mort...\n");
	}
	
}


void traitement(mess* v1)
{
	voiture* v = &(v1->car);
	//printf("lancement traitement\n");
	
	switch(v->sortie)
	{
		case DROITE:
		{
			// v->entree numéro de l'entrée
			printf("Entree dans le carrefour voiture %d par l'entree %d.\n", v->id, v->entree);
				
			//bloquage du carrefour
			switch (v->entree){
				case OUEST:	
				{				
					PSem(sem_in_out[0]);
					break;
				}
				case SUD:	
				{				
					PSem(sem_in_out[1]);
					break;
				}
				case EST:	
				{				
					PSem(sem_in_out[2]);
					break;
				}
				case NORD:	
				{				
					PSem(sem_in_out[3]);
					break;
				}
			}
			printf("Voiture id:%d, passe le carrefour !\n", v->id);
		
			//liberation du carrefour
			switch (v->entree){
				case OUEST:	
				{
					VSem(sem_in_out[0]);
					break;
				}
				case SUD:	
				{
					VSem(sem_in_out[1]);
					break;
				}
				case EST:	
				{
					VSem(sem_in_out[2]);
					break;
				}
				case NORD:	
				{
					VSem(sem_in_out[3]);
					break;
				}
			}
			
			printf("Voiture id:%d, sort  carrefour !\n", v->id);
		
			break;
		}
		case EN_FACE:
		{
			// v->entree numéro de l'entrée
			printf("Entree dans le carrefour voiture %d par l'entree %d.\n", v->id, v->entree);
				
			//bloquage du carrefour
			switch (v->entree){
				case OUEST:	
				{				
					PSem(sem_in_out[0]);
					printf("sem 0 bloquée par %d\n", v->id);
					PSem(sem_in_out[1]);
					printf("sem 1 bloquée par %d\n", v->id);
					
					break;
				}
				case SUD:	
				{				
					PSem(sem_in_out[1]);
					PSem(sem_in_out[2]);
					break;
				}
				case EST:	
				{				
					PSem(sem_in_out[2]);
					PSem(sem_in_out[3]);
					break;
				}
				case NORD:	
				{				
					PSem(sem_in_out[3]);
					printf("sem 3 bloquée par %d\n", v->id);
					PSem(sem_in_out[0]);
					printf("sem 0 bloquée par %d\n", v->id);
					break;
				}
			}			
			printf("Voiture id:%d, passe le carrefour !\n", v->id);

			//liberation du carrefour
			switch (v->entree){
				case OUEST:	
				{
					VSem(sem_in_out[0]);
					printf("sem 0 débloquée par %d\n", v->id);
					VSem(sem_in_out[1]);
					printf("sem 1 débloquée par %d\n", v->id);
					break;
				}
				case SUD:	
				{
					VSem(sem_in_out[1]);
					VSem(sem_in_out[2]);
					break;
				}
				case EST:	
				{
					VSem(sem_in_out[2]);
					VSem(sem_in_out[3]);
					break;
				}
				case NORD:	
				{
					VSem(sem_in_out[3]);
					VSem(sem_in_out[0]);
					break;
				}
			}
			
			printf("Voiture id:%d, sort  carrefour !\n", v->id);
		
			break;
		}
		case GAUCHE:
		{
			// v->entree numéro de l'entrée
			printf("Entree dans le carrefour voiture %d par l'entree %d.\n", v->id, v->entree);
				
			//bloquage du carrefour
			switch (v->entree){
				case OUEST:	
				{				
					PSem(sem_in_out[0]);
					PSem(sem_in_out[1]);
					PSem(sem_in_out[2]);
					break;
				}
				case SUD:	
				{				
					PSem(sem_in_out[1]);
					PSem(sem_in_out[2]);
					PSem(sem_in_out[3]);
					break;
				}
				case EST:	
				{				
					PSem(sem_in_out[2]);
					printf("sem 2 bloquée par %d\n", v->id);
					PSem(sem_in_out[3]);
					printf("sem 3 bloquée par %d\n", v->id);
					PSem(sem_in_out[0]);
					printf("sem 0 bloquée par %d\n", v->id);
					break;
				}
				case NORD:	
				{				
					PSem(sem_in_out[3]);
					PSem(sem_in_out[0]);
					PSem(sem_in_out[1]);
					break;
				}
			}
			printf("Voiture id:%d, passe le carrefour !\n", v->id);
		
			//liberation du carrefour
			switch (v->entree){
				case OUEST:	
				{
					VSem(sem_in_out[0]);
					VSem(sem_in_out[1]);
					VSem(sem_in_out[2]);
					break;
				}
				case SUD:	
				{
					VSem(sem_in_out[1]);
					VSem(sem_in_out[2]);
					VSem(sem_in_out[3]);
					break;
				}
				case EST:	
				{
					VSem(sem_in_out[2]);
					printf("sem 2 debloquée par %d\n", v->id);
					VSem(sem_in_out[3]);
					printf("sem 3 debloquée par %d\n", v->id);
					VSem(sem_in_out[0]);
					printf("sem 0 debloquée par %d\n", v->id);
					break;
				}
				case NORD:	
				{
					VSem(sem_in_out[3]);
					VSem(sem_in_out[0]);
					VSem(sem_in_out[1]);
					break;
				}
			}
			
			printf("Voiture id:%d, sort  carrefour !\n", v->id);
		
			break;
		}
	}	
	free(v1);
	
	pthread_exit(0);
}






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

	sleep(1);

    return semop(id, &op, 1); //Entrée dans la section critique P()
}

int VSem(int id){
	sembuf op;
     
	op.sem_num = 0; //Numéro de notre sémaphore
	op.sem_op = 1; //Pour un V() on incrémente
	op.sem_flg = 0; //On ne s'en occupe pas

    semop(id, &op, 1); //Sortie de la section critique V()
}

void destructionSem(int id){
    semctl(id, 0, IPC_RMID, 0); //Destruction du sémaphore
}

void afficheEtatSem(){
	int i;
	printf("Etat des sémaphores");
	for(i = 0; i<4; i++){
		printf("%d : %d\t",i,sem_in_out[i]);
	}
	printf("\n");
}



