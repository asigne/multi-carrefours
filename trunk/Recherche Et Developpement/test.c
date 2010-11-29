#include "fonctions.h"

int main(int argc, char **argv)
{
	int msgid;
	if((msgid = msgget(666, IPC_CREAT | IPC_EXCL | 0666)) == -1){
		//perror(strerror(errno));
	}
	
	int pid = fork();

	if(pid == 0)
	{
		// processus gérant le carrefour 1 (NORD-OUEST)
 		int ligne,colonne;
		key_t clef;
		int semid,cpt=0;
		afficheEtatSem();	//Afficher l'état des sémaphores
		//créer les 36 sémaphores du carrefour
		for(ligne = 0; ligne<6; ligne++){
			for(colonne = 0; colonne<6; colonne++){	
				clef = ftok(argv[0], ID_PROJET+cpt); //verifier unicité des clefs générée
				sem_in_out[ligne][colonne]=creerSem(clef);
				initSem(sem_in_out[ligne][colonne], 1);
				cpt++;		
			}
		}
		
		pthread_t thread_traitement[3];
		
		while(1)
		{
			// attendre voiture
			printf("Attente de voiture...\n");
			mess* m = (mess*) malloc(sizeof(mess));
			msgrcv(msgid, m, sizeof(mess), 0, 0);
			// file de la voiture = m.type
			m->car.entree = m->type-1;
			
			// créer thread traiter voiture.
			
			//pthread_t thread_traitement;
			pthread_create(&thread_traitement[m->type-1], NULL, (void * (*)(void *))traitement, m);
			
			//usleep(100);
		}
		//destruction semaphores du carrefour
		destructionSem();
	}
	else{
		// pere
		mess m;
		m.car.id = 11;
		m.car.sortie = EN_FACE;
		m.car.entree = -1;
		m.type = 1; //OUEST
		
		mess m1;
		m1.car.id = 15;
		m1.car.sortie = GAUCHE;
		m1.car.entree = -1;
		m1.type = 2; //SUD
		
		mess m2;
		m2.car.id = 12;
		m2.car.sortie = GAUCHE;
		m2.car.entree = -1;
		m2.type = 3; //EST
		
		
		msgsnd(msgid, &m, sizeof(mess) - sizeof(long), 0);
		msgsnd(msgid, &m2, sizeof(mess) - sizeof(long), 0);
		msgsnd(msgid, &m1, sizeof(mess) - sizeof(long), 0);	
		//printf("2 Nouvelle voiture dans la file...\n");
		
		wait();
		
		printf("Fils mort...\n");
	}
}


void traitement(mess* v1)
{
	voiture* v = &(v1->car);
	switch(v->sortie)
	{
		// v->entree numéro de l'entrée
		case DROITE:
		{
			printf("Entree dans le carrefour voiture %d par l'entree %d.\n", v->id, v->entree);			
			tourneDroite(*v);
			printf("Voiture id:%d, sort  carrefour !\n", v->id);
			break;
		}
		case EN_FACE:
		{
			printf("Entree dans le carrefour voiture %d par l'entree %d.\n", v->id, v->entree);
			enFace(*v);
			printf("Voiture id:%d, sort  carrefour !\n", v->id);
			break;
		}
		case GAUCHE:
		{
			printf("Entree dans le carrefour voiture %d par l'entree %d.\n", v->id, v->entree);
			tourneGauche(*v);
			printf("Voiture id:%d, sort  carrefour !\n", v->id);
			break;
		}
	}	
	free(v1);
	
	pthread_exit(0);
}







