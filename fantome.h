#ifndef _FANTOME_
#define _FANTOME_

	#include "constantes.h"
	#include "msgStruct.h"
	#include "mempart.h"
	#include "structures.h"
	#include "log.h"
	#include <sys/types.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <sys/msg.h>
	#include <sys/shm.h>
	#include <sys/sem.h>
	#include <time.h>
	#include <signal.h>
	#include <errno.h>

	int fantome_arreter(int, memPart_t *, int, void*, int);
	position_t *initPosition(grille_t *);
	void trouverPosition(grille_t *, position_t *);
	void verifier_depl( int, int, int, fantome_t *, position_t *, memPart_t *);

#endif