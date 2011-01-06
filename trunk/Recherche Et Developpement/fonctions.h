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
#define NbVoituresGlobal 100


static int sem_in_out[4][2][2];
int msgid[4];
int idMemPartagee[4];

int* memoiresPartagees[8];
static pthread_mutex_t memPart;


pid_t pidCarrefour[4];
pid_t pidServeurControleur, pidAffichage, pidGenerateur,pidPere;
int msgidServeurControleur;

//identifiant pour les IPC
static int cptIdentifiant=0;

//compteur des identifiants de voitures
static int cptVoitures=1;

int idCptExitFaux;
//compteur voiture exit "faux"
int *cptExitFaux;

enum{
	FAUX=0,
	VRAI
};

//destinations
enum
{
	DROITE =1,	//1
	EN_FACE,	//2
	GAUCHE		//3
};

//entrees/sorties
enum
{
	OUEST = 1, 	//1
	SUD,		//2
	EST,		//3
	NORD		//4
};

//structure definissant une voiture
typedef struct {
	int id;
	int entree;
	int sortie;
	int sortieFinale;
	int numCarrefourFinal;
	int numCarrefour;
	int prioritaire;
} voiture;


//structure pour envoyer une voiture
typedef struct {
	long type;
	
	voiture car;
} mess;

//struture pour les semaphores
typedef struct{
	short sem_num;
	short sem_op;
	short sem_flg;
} sembuf;


//structure pour les requetes avec le serveurControleur
typedef struct{
	long type;
	
	int pidEchgeur;
	int idOrigine;
	int voieOrigine;
	int idDest;
	int voieDest;
} ReqEchgeur;

//structure pour les reponses du serveurControleur
typedef struct{
	long type;
	
	int voieDest;
} RepCtrleur;




//supprime les IPC crees lors de l'exécution
void supprimerIPC();

//affichage graphique de l'etat des carrefours
void affichageCarrefours();

//cree un semaphore	--> 	retourne l'id du semaphore
int creerSem(int clef, int nombre);

//initialise le semaphore d'id "id" à la valeur "valeur"
void initSem(int id, int valeur);


//P et V sur le semaphore d'id "id"
int PSem(int id);
int VSem(int id);

//fait tourner la voiture v à droite
void tourneDroite(voiture v);

//Fait aller la voiture v en face
void enFace(voiture v);

//fait tourner la voiture v à gauche
void tourneGauche(voiture v);

//permet de traiter le message recu par un carrefour : le message contient la voiture et la direction de celle-ci
void traitement(mess* message);

//permet de gerer les voitures arrivant à un carrefour : cette fonction est executee par chaque proccessus représentant un carrefour
void gestionCarrefour(int numCarrefour);

//envoi le message au carrefour correspondant + incrementation des compteurs de voitures
void envoiVoiture(mess messageAEnvoyer);

//cree une voiture aleatoire (entree sur le circuit, prioritaire ou non...) et l'envoi au carrefour correspondant
void creerVoiture();
