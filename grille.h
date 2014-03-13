#ifndef _GRILLE_
#define _GRILLE_
	
	#include "constantes.h"
	#include <stdlib.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <string.h>

	typedef struct
	{
		int largeur;
		int hauteur;
		int nbFantomes;
		char* cases;	/* La grille (tableau 2D) sera représentée sur un seul tableau de chars */
	} grille_t;

	#include "outilsGrille.h"

	grille_t* grille_creer(int, int);
	void grille_supprimer(grille_t**);
	void grille_afficher(grille_t*);
	grille_t* grille_charger(char*);
	void grille_sauvegarder(char *, grille_t* grille);

#endif