#ifndef _MSG_STRUCT_
#define _MSG_STRUCT_
	
	#include <sys/types.h>
	/* Messages utilisés pour annoncer au joueur et aux fantomes que le jeu s'arrete */
	typedef struct{
		long type;
	} msgStop_t;

	/* Messages utilisés pour demander au fantome de se déplacer */
	typedef struct{
		long type; /* Correspond au pid */
	} msgDepl_t;

	/* Messages utilisés pour envoyer les données à enregistrer au log */
	typedef struct{
		long type;
		pid_t pid;
		char msg[TAILLE_LOG_MSG];
	} msgLog_t;

	/* Messages utilisés pour envoyer les clés */
	typedef struct{
		long type;
		int cleSem;
		int cleSeg;
		int cleMsq;
		pid_t pidLogger;
	} msgCle_t;

#endif