#ifndef _MEMPART_
#define _MEMPART_
	
	#include "grille.h"
	#include "structures.h"

	void memPart_supprimer(memPart_t**);
	memPart_t* memPart_charger(void*);
	void memPart_initialiser(void*, grille_t*);

#endif