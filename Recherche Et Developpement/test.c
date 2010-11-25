
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
		
		sem_init(&sem_generale, 0, 1);
		
		for(i = 0; i<12; i++)
			sem_init(&sem_in_out[i], 0, 1);
		
		
		pthread_t thread_traitement[3];
		
		while(1)
		{
			// attendre voiture
			mess* m = (mess*) malloc(sizeof(mess));
			
			printf("Attente de voiture...\n");
			
			msgrcv(msgid, m, sizeof(mess), 0, 0);
			
			printf("Lecture message...\n");
			
			// file de la voiture = m.type
			// les infos de la voiture = m.car
			
			//voiture* v = m.car;
			
			printf("MSG type = %d\n", (int)m->type);
			
			m->car.entree = m->type-1;
			
			printf("Message de la voiture %d.\n", m->car.id);
			
			// crée thread traiter voiture.
			
			//pthread_t thread_traitement;
			pthread_create(&thread_traitement[m->type-1], NULL, (void * (*)(void *))traitement, m);
			
			printf("lancement thread...\n");
			
			//pthread_join(thread_traitement, NULL);
			
			usleep(100);
			
		}
		
	} else {
		// pere
		
		sleep(2);
		
		mess m;
		m.car.id = 11;
		m.car.sortie = EN_FACE;
		m.car.entree = -1;
		m.type = 1;
		
		mess m1;
		m1.car.id = 15;
		m1.car.sortie = EN_FACE;
		m1.car.entree = -1;
		m1.type = 3;
		
		
		mess m2;
		m2.car.id = 12;
		m2.car.sortie = EN_FACE;
		m2.car.entree = -1;
		m2.type = 2;
		
		
		msgsnd(msgid, &m, sizeof(mess) - sizeof(long), 0);
		//msgsnd(msgid, &m2, sizeof(mess) - sizeof(long), 0);
		//sleep(2);
		msgsnd(msgid, &m1, sizeof(mess) - sizeof(long), 0);
		
		
		printf("2 Nouvelle voiture dans la file...\n");
		
		wait();
		
		printf("Fils mort...\n");
	}
	
}


void traitement(mess* v1)
{
	voiture* v = &(v1->car);
	printf("lancement traitement\n");
	
	switch(v->sortie)
	{
	case DROITE:
	{
		// bloque l'accès au sémaphores.
		sem_wait(&sem_generale);
		printf("Acces au carrefour voiture %d.\n", v->id);
		
		// v->entree numéro de l'entrée
		
		// bloque toute les sémaphores
		sem_wait(&sem_in_out[(v->entree*3)]);   // la sienne vers gauche
		sem_wait(&sem_in_out[(v->entree*3)+1]); // la sienne vers en face.
		sem_wait(&sem_in_out[(v->entree*3)+2]); // la sienne vers droite.

		sem_wait(&sem_in_out[((v->entree*3)+6)%12]); // ceux qui viennent d'en face
		
		sem_wait(&sem_in_out[((v->entree*3)+10)%12]); // ceux qui viennent de sa gauche
		
		sem_post(&sem_generale);
		
		sleep(2);
		
		printf("Voiture id:%d, passe le carrefour !\n", v->id);
		
		sem_wait(&sem_generale);
		
		// libere le carrefour
		sem_post(&sem_in_out[((v->entree*3)+10)%12]);
		
		sem_post(&sem_in_out[((v->entree*3)+6)%12]);
		
		sem_post(&sem_in_out[(v->entree*3)+2]);
		sem_post(&sem_in_out[(v->entree*3)+1]);
		sem_post(&sem_in_out[(v->entree*3)]);
		
		
		// fin on libere les sémaphores
		sem_post(&sem_generale);
		
		
		break;
	}
	case EN_FACE:
	{
		// bloque l'accès au sémaphores.
		sem_wait(&sem_generale);
		printf("Acces au carrefour voiture %d.\n", v->id);
		
		// v->entree numéro de l'entrée
		
		// bloque toute les sémaphores
		sem_wait(&sem_in_out[(v->entree*3)]);   // la sienne vers gauche
		sem_wait(&sem_in_out[(v->entree*3)+1]); // la sienne vers en face.
		sem_wait(&sem_in_out[(v->entree*3)+2]); // la sienne vers droite.
		
		
		sem_wait(&sem_in_out[((v->entree*3)+3)%12]);
		sem_wait(&sem_in_out[((v->entree*3)+4)%12]); // ceux qui viennent de sa droite
		sem_wait(&sem_in_out[((v->entree*3)+5)%12]);
		
		sem_wait(&sem_in_out[((v->entree*3)+6)%12]); // ceux qui viennent d'en face
		
		sem_wait(&sem_in_out[((v->entree*3)+9)%12]);
		sem_wait(&sem_in_out[((v->entree*3)+10)%12]); // ceux qui viennent de sa gauche
		
		sem_post(&sem_generale);
		
		sleep(2);
		
		printf("Voiture id:%d, passe le carrefour !\n", v->id);
		
		sem_wait(&sem_generale);
		
		// libere le carrefour
		sem_post(&sem_in_out[((v->entree*3)+10)%12]);
		sem_post(&sem_in_out[((v->entree*3)+9)%12]);
		
		sem_post(&sem_in_out[((v->entree*3)+6)%12]);
		
		sem_post(&sem_in_out[((v->entree*3)+5)%12]);
		sem_post(&sem_in_out[((v->entree*3)+4)%12]);
		sem_post(&sem_in_out[((v->entree*3)+3)%12]);
		
		sem_post(&sem_in_out[(v->entree*3)+2]);
		sem_post(&sem_in_out[(v->entree*3)+1]);
		sem_post(&sem_in_out[(v->entree*3)]);
		
		// fin on libere les sémaphores
		
		sem_post(&sem_generale);
		
		printf("Voiture id:%d, sort  carrefour !\n", v->id);
		
		break;
	}
	case GAUCHE:
	{
		
		// bloque l'accès au sémaphores.
		sem_wait(&sem_generale);
		
		// v->entree numéro de l'entrée
		
		// bloque toute les sémaphores
		sem_wait(&sem_in_out[(v->entree*3)]);   // la sienne vers gauche
		sem_wait(&sem_in_out[(v->entree*3)+1]); // la sienne vers en face.
		sem_wait(&sem_in_out[(v->entree*3)+2]); // la sienne vers droite.
		
		
		sem_wait(&sem_in_out[((v->entree*3)+3)%12]);
		sem_wait(&sem_in_out[((v->entree*3)+4)%12]); // ceux qui viennent de sa droite
		
		sem_wait(&sem_in_out[((v->entree*3)+6)%12]); // ceux qui viennent d'en face
		sem_wait(&sem_in_out[((v->entree*3)+7)%12]);
		sem_wait(&sem_in_out[((v->entree*3)+8)%12]);
		
		sem_wait(&sem_in_out[((v->entree*3)+9)%12]);
		sem_wait(&sem_in_out[((v->entree*3)+10)%12]); // ceux qui viennent de sa gauche
		sem_post(&sem_generale);
		
		sleep(2);
		
		printf("Voiture id:%d, passe le carrefour !\n", v->id);
		
		sem_wait(&sem_generale);
		// libere le carrefour
		sem_post(&sem_in_out[((v->entree*3)+10)%12]);
		sem_post(&sem_in_out[((v->entree*3)+9)%12]);
		
		sem_post(&sem_in_out[((v->entree*3)+8)%12]);
		sem_post(&sem_in_out[((v->entree*3)+7)%12]);
		sem_post(&sem_in_out[((v->entree*3)+6)%12]);
		
		sem_post(&sem_in_out[((v->entree*3)+4)%12]);
		sem_post(&sem_in_out[((v->entree*3)+3)%12]);
		
		sem_post(&sem_in_out[(v->entree*3)+2]);
		sem_post(&sem_in_out[(v->entree*3)+1]);
		sem_post(&sem_in_out[(v->entree*3)]);
		
		// fin on libere les sémaphores
		
		sem_post(&sem_generale);
		
		break;
	}
	}
	
	free(v1);
	
	pthread_exit(0);
}







