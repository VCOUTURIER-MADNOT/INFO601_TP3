#ifndef _JOUEUR_
#define _JOUEUR_

	#include "constantes.h"
	#include "msgStruct.h"
	#include "structures.h"
	#include "mempart.h"
	#include "log.h"
	#include <sys/types.h>
	#include <sys/sem.h>
	#include <sys/msg.h>
	#include <sys/shm.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdio.h>
	#include <signal.h>
	#include <unistd.h>
	#include <errno.h>

	void afficherGrilleJoeur(int, memPart_t *, int);
	int joueur_arreter(memPart_t *, int, void*, int);
	void afficher_menu(int, int);
	int verifier_position(int, int, memPart_t *);
	void traiter_fantome(int, int, memPart_t *, int, int);
#endif