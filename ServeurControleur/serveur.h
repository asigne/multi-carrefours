#ifndef _SERVEUR_CONTROLEUR_
#define _SERVEUR_CONTROLEUR_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/sem.h>

#define MAX_TRAFFIC 10


int AjouteTraffic(int TrafficCourant, int Carrefour, int Voie);

enum
{
	OUEST = 1,
	SUD,
	EST,
	NORD
};


enum
{
	CARREFOUR_NO = 0,
	CARREFOUR_NE,
	CARREFOUR_SO,
	CARREFOUR_SE
};


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


typedef struct
{
	int idCarrefour;
	int idVoie;
	int Traffic;
} Traffic;


#endif

