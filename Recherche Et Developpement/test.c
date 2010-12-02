#include "fonctions.h"



int main(int argc, char **argv)
{
	int i;
	msgid[0]= msgget(666, IPC_CREAT | IPC_EXCL | 0666);
	msgid[1]= msgget(667, IPC_CREAT | IPC_EXCL | 0666);
	msgid[2]= msgget(668, IPC_CREAT | IPC_EXCL | 0666);
	msgid[3]= msgget(669, IPC_CREAT | IPC_EXCL | 0666);


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
	else{
		// pere
		mess m1;
		m1.car.id = 1;		 
		m1.car.entree = NORD;
		m1.car.sortie = SUD;   
		m1.type = 1;
	
		mess m2;
		m2.car.id = 2;
		m2.car.entree = NORD;
		m2.car.sortie = OUEST;
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


		
		
		msgsnd(msgid[0], &m2, sizeof(mess) - sizeof(long), 0);
		msgsnd(msgid[0], &m1, sizeof(mess) - sizeof(long), 0);	
	//	msgsnd(msgid[0], &m3, sizeof(mess) - sizeof(long), 0);
	//	msgsnd(msgid[0], &m4, sizeof(mess) - sizeof(long), 0);	
	//	msgsnd(msgid[0], &m5, sizeof(mess) - sizeof(long), 0);
	//	msgsnd(msgid[0], &m6, sizeof(mess) - sizeof(long), 0);
		//printf("2 Nouvelle voiture dans la file...\n");


	/*	for(i=0;i<4;i++){
			printf("%d\n",pidCarrefour[i]);
		}*/

		//sleep(5);

		afficheEtatSem();

		
		waitpid(pidCarrefour[0], NULL, NULL);
		waitpid(pidCarrefour[1], NULL, NULL);
		waitpid(pidCarrefour[2], NULL, NULL);
		waitpid(pidCarrefour[3], NULL, NULL);

		printf("Mort du père...\n");
	}
}










