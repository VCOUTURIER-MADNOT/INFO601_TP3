#include "mempart.h"

void memPart_supprimer(memPart_t** memPart)
{
	int i;
	for(i = 0; i < (*memPart)->grille->nbFantomes; i++)
	{
		free((*memPart)->fantomes[i]);
		free((*memPart)->positions[i]);
	}

	free((*memPart)->positions[(*memPart)->grille->nbFantomes]);
	free((*memPart)->fantomes);
	free((*memPart)->positions);
	free(*memPart);
	
	*memPart = NULL;
}

void memPart_initialiser(void* adresse, grille_t *grille)
{
	int i;
	int *ptrInt;
	pid_t *ptrPid;
	char *ptrChar;
	char temp[TAILLE_NOM_FANTOME];

	ptrInt = (int*)adresse;
	/* Stockage du nombre de fantome */
	*ptrInt = grille->nbFantomes;
	ptrInt++;

	printf(INFO"mempart_sauvegarder - nbFantomes sauvegardé\n");

	/* Stockage de la largeur */
	*ptrInt = grille->largeur;
	ptrInt++;
	printf(INFO"mempart_sauvegarder - largeur sauvegardée\n");

	/* Stockage de la hauteur */
	*ptrInt = grille->hauteur;
	ptrInt++;
	printf(INFO"mempart_sauvegarder - hauteur sauvegardée\n");

	/* Stockage des cases */
	ptrChar = (char*) ptrInt;
	memcpy(ptrChar, grille->cases, sizeof(char) * (strlen(grille->cases) + 1));
	ptrChar += sizeof(char) * (strlen(grille->cases) +1);
	printf(INFO"mempart_sauvegarder - cases sauvegardées\n");

	/* Stockage des fantomes */
	for(i = 0; i < grille->nbFantomes; i++)
	{
		ptrPid = (pid_t*) ptrChar;
		*ptrPid = (pid_t)-1;
		ptrPid++;

		ptrChar = (char*) ptrPid;
		sprintf(temp, "fantome");
		strcpy(ptrChar, temp);
		ptrChar += sizeof(char) * TAILLE_NOM_FANTOME;

		printf(INFO"mempart_sauvegarder - Fantome sauvegardé\n");
	}

	/* Stockage des positions */
	ptrInt = (int*)ptrChar;
	for(i = 0; i <= grille->nbFantomes; i++)
	{
		*ptrInt = -1;
		ptrInt++;

		*ptrInt = -1;
		ptrInt++;
		printf(INFO"mempart_sauvegarder - position %d sauvegardée\n", i);	
	}
}

memPart_t* memPart_charger(void* adresse)
{
	memPart_t *memoire = NULL;

	int *ptrInt, i;
	pid_t *ptrPid;
	char *ptrChar;

	ptrInt = (int*)adresse;

	memoire = (memPart_t*)malloc(sizeof(memPart_t));
	memoire->grille = (grille_t*)malloc(sizeof(grille_t));

	/* On récupère le nombre de fantomes */
	memoire->grille->nbFantomes = *ptrInt;
	ptrInt++;

	/* On récupère la largeur */
	memoire->grille->largeur = *ptrInt;
	ptrInt++;

	/* On récupère la hauteur */
	memoire->grille->hauteur = *ptrInt;
	ptrInt++;

	/* On récupère les cases */
	ptrChar = (char*)ptrInt;
	memoire->grille->cases = ptrChar;
	ptrChar += sizeof(char) * (strlen(memoire->grille->cases) + 1);

	/* On récupère les fantomes et les positions */
	memoire->fantomes = (fantome_t**)malloc(sizeof(fantome_t*)* memoire->grille->nbFantomes);
	for (i = 0; i < memoire->grille->nbFantomes; ++i)
	{
		memoire->fantomes[i] = (fantome_t*)malloc(sizeof(fantome_t));

		ptrPid = (pid_t*)ptrChar;
		memoire->fantomes[i]->pid = ptrPid;
		ptrPid++;

		ptrChar = (char*)ptrPid;
		memoire->fantomes[i]->nom = ptrChar;
		ptrChar += sizeof(char) * TAILLE_NOM_FANTOME;
	}

	memoire->positions = (position_t**)malloc(sizeof(position_t*) * (memoire->grille->nbFantomes +1));
	ptrInt = (int*)ptrChar;
	for (i = 0; i <= memoire->grille->nbFantomes; ++i)
	{
		memoire->positions[i] = (position_t*)malloc(sizeof(position_t));

		memoire->positions[i]->x = ptrInt;
		ptrInt++;

		memoire->positions[i]->y = ptrInt;
		ptrInt++;
	}
	return memoire;
}
