#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <errno.h>


sem_t sem_in_out[12];
sem_t sem_generale;

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

