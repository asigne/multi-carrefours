#ifndef _SERVEUR_CONTROLEUR_
#define _SERVEUR_CONTROLEUR_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/sem.h>

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
	
	int idDest;
	int voieDest;
	
} RepCtrleur;


typedef struct
{
	int idCarrefour;
	int idVoie;
	int Traffic;
} Traffic;

typedef struct
{
	int Open[4];
} Liste;


#endif

