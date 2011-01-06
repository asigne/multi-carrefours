#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/sem.h>
#include <signal.h>
#include <time.h>

#define ID_PROJET 'P'
int cptVoitures=1;

static int sem_in_out[4][2][2];
int msgid[4];
int idMemPartagee[4];

int* memoiresPartagees[8];
static pthread_mutex_t memPart;


pid_t pidCarrefour[4];
pid_t pidServeurControleur, pidAffichage;
int msgidServeurControleur;

int cptIdentifiant=0;

enum{
	FAUX=0,
	VRAI
};

enum
{
	DROITE =1,	//1
	EN_FACE,	//2
	GAUCHE		//3
};

enum
{
	OUEST = 1, 	//1
	SUD,		//2
	EST,		//3
	NORD		//4
};


typedef struct {
	int id;
	int entree;
	int sortie;
	int sortieFinale;
	int numCarrefourFinal;
	int numCarrefour;
	int prioritaire;
} voiture;


typedef struct {
	long type;
	voiture car;
} mess;

void traitement(mess*);



typedef struct{
	short sem_num;
	short sem_op;
	short sem_flg;
} sembuf;



typedef struct
{
	long type;
	
	int pidEchgeur;
	int idOrigine;
	int voieOrigine;
	int idDest;
	int voieDest;
	
} ReqEchgeur;

typedef struct
{
	long type;
	int voieDest;
	
} RepCtrleur;

void envoiVoiture(mess messageAEnvoyer);

/*int creerSem(int clef);
void initSem(int id, int valeur);
int PSem(int id);
int VSem(int id);
void destructionSem();
void afficheEtatSem();

void tourneDroite(int entreeVoiture);
void enFace(int entreeVoiture);
void tourneGauche(int entreeVoiture);*/	