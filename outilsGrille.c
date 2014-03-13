#include "outilsGrille.h"

/* Cette fonction permet de récupérer l'indice dans un tableau 1D à partir des coords d'un tableau 2D */
/* Retourne l'indice correspondant en cas de réussite */
/* En cas d'echec (en dehors du tableau), la fonction retourne -1 */
int caseTableau(grille_t *grille, int ligne, int colonne)
{
	if(ligne >= grille->hauteur || colonne >= grille->largeur)
	{
		fprintf(stderr, WARNING"L'indice demandé [ %d, %d] n'existe pas\n", ligne, colonne);
		return -1;
	}

	return (ligne * grille->largeur + colonne);
}