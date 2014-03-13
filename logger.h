#ifndef _LOGGER_
#define _LOGGER_

	#include "constantes.h"
	#include "grille.h"
	#include "structures.h"
	#include "mempart.h"
	#include "msgStruct.h"
	#include "log.h"
	#include <sys/shm.h>
	#include <sys/msg.h>
	#include <sys/sem.h>
	#include <sys/wait.h>
	#include <signal.h>
	#include <stdlib.h>

	int creerMemPartagee(int, int, int);
	void supprimerMemPartagee(int);
	int creerFileMessages(int);
	void supprimerFileMessages(int);
	int creerTabSem(int, int);
	void supprimerTabSem(int);
	int logger_stop(int, int, int, pid_t);
	void deplacerFantome(memPart_t *, int, int);
	void traiterMessage(char*, int);

#endif