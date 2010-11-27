#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/sem.h>



int sem_in_out[4];

enum
{
	DROITE = 0,
	EN_FACE,
	GAUCHE
};

enum
{
	OUEST = 0,
	SUD,
	EST,
	NORD
};


typedef struct 
{
	int id;
	int entree;
	int sortie;
} voiture;


typedef struct 
{
	long type;
	voiture car;
} mess;

void traitement(mess*);



typedef struct
{
	short sem_num;
	short sem_op;
	short sem_flg;
} sembuf;


int creerSem(int clef);

void initSem(int id, int valeur);

int PSem(int id);

int VSem(int id);

void destructionSem(int id);

void afficheEtatSem();
