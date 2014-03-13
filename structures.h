#ifndef _STRUCTURES_
#define _STRUCTURES_

	#include "grille.h"

	typedef struct
	{
		int *x;
		int *y;
	}position_t;

	typedef struct
	{
		pid_t *pid;
		char *nom;
	}fantome_t;

	typedef struct
	{
		grille_t *grille;
		fantome_t **fantomes;
		position_t **positions;
	}memPart_t;
#endif