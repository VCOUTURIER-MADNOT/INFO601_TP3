#include "grille.h"

grille_t* grille_creer(int l, int h)
{
	int i;
	grille_t* grille = NULL;
	grille = (grille_t*)malloc(sizeof(grille_t));
	grille->largeur = l;
	grille->hauteur = h;
	grille->nbFantomes = 0;
	grille->cases = (char*)malloc(sizeof(char) * (l * h));
	for (i = 0; i < (l*h); ++i)
	{
		grille->cases[i] = ' ';
	}

	return grille;
}

void grille_supprimer(grille_t** grille)
{
	free((*grille)->cases);
	free(*grille);
	*grille = NULL;
}

void grille_afficher(grille_t* grille)
{
	int i, j;
	/* Ligne du haut */
	printf("/");
	for (i = 0; i < grille->largeur; ++i)
		printf("-");

	printf("\\\n");
	/* Affichage du corps */
	for(i = 0; i < grille->hauteur; i++)
	{
		printf("|");
		for(j = 0; j < grille->largeur ; j++)
		{
			printf("%c", grille->cases[caseTableau(grille,i,j)]);
		}
		printf("|\n");
	}

	printf("\\");
	for (i = 0; i < grille->largeur; ++i)
		printf("-");

	printf("/\n");
}

grille_t* grille_charger(char* nomFichier)
{
	int fd, l, h, nonFin, i = 0;
	grille_t * grille = NULL;

	if((fd = open(nomFichier, O_RDONLY)) == -1)
	{
		fprintf(stderr, ERREUR"Grille (grille_charger) - Erreur lors de l'ouverture du fichier %s", nomFichier);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(read(fd, &l, sizeof(int)) == -1)
	{
		fprintf(stderr, ERREUR"Grille (grille_charger) - Erreur lors de la lecture de la largeur");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(read(fd, &h, sizeof(int)) == -1)
	{
		fprintf(stderr, ERREUR"Grille (grille_charger) - Erreur lors de la lecture de la hauteur");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	grille = grille_creer(l, h);

	if(read(fd, &(grille->nbFantomes), sizeof(int)) == -1)
	{
		fprintf(stderr, ERREUR"Grille (grille_charger) - Erreur lors de la lecture du nombre de fantomes sur la grille");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	while(i < strlen(grille->cases))
	{
		if((nonFin = read(fd, &(grille->cases[i]), sizeof(char))) == -1)
		{
			fprintf(stderr, ERREUR"Grille (grille_charger) - Erreur lors de la lecture de la case [ %d]", i);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		else if(nonFin == 0)
		{
			fprintf(stderr, ERREUR"Grille (grille_charger) - Erreur lors de la lecture de la case [ %d] : Fin du fichier atteinte", i);
			exit(EXIT_FAILURE);
		}
		i++;
	}

	if(close(fd) == -1)
	{
		fprintf(stderr, ERREUR"Grille (grille_charger) - Erreur lors de la fermeture du fichier %s", nomFichier);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	return grille;
}

void grille_sauvegarder(char *nomFichier, grille_t* grille)
{
	int fd, i;

	if((fd = open(nomFichier, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1)
	{
		fprintf(stderr, ERREUR"Grille (grille_sauvegarder) - Erreur lors de l'ouverture du fichier %s", nomFichier);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(write(fd, &(grille->largeur), sizeof(int)) == -1)
	{
		fprintf(stderr, ERREUR"Grille (grille_sauvegarder) - Erreur lors de l'écriture de la largeur de la grille");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(write(fd, &(grille->hauteur), sizeof(int)) == -1)
	{
		fprintf(stderr, ERREUR"Grille (grille_sauvegarder) - Erreur lors de l'écriture de la hauteur de la grille");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(write(fd, &(grille->nbFantomes), sizeof(int)) == -1)
	{
		fprintf(stderr, ERREUR"Grille (grille_sauvegarder) - Erreur lors de l'écriture du nombre de fantomes");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	for(i = 0; i < strlen(grille->cases); i++)
	{
		if(write(fd, &(grille->cases[i]), sizeof(char)) == -1)
		{
			fprintf(stderr, ERREUR"Grille (grille_sauvegarder) - Erreur lors de l'écriture de la case [%d]", i);
			perror(" ");
			exit(EXIT_FAILURE);
		}
	}
}