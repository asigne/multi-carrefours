#ifndef _SERVEUR_CONTROLEUR_
#define _SERVEUR_CONTROLEUR_
/*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/sem.h>
*/

#include "fonctions.h"

#define MAX_TRAFFIC 100

enum
{
	CARREFOUR_NO = 0,
	CARREFOUR_NE,
	CARREFOUR_SO,
	CARREFOUR_SE
};

typedef struct
{
	int idCarrefour;
	int idVoie;
	int Traffic;
} Traffic;


int msgidServeur;
int shmidServeur[4];
int* shMemServeur[4];


void* AnalyseTraffic(ReqEchgeur*);

int chemin_plus_rapide(Traffic , Traffic );

int AjouteTraffic(int , int , int );

void serveurControleur(int , int , int , int , int );

#endif

