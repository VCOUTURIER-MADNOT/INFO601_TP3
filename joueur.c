#include "joueur.h"

/* Fonction permettant d'afficher la grille avec le joueur ou avec le joueur et les fantomes */
/* Types :
*	- FANTOME_AFF : Affiche le joueur et les fantomes
*	- NORMAL_AFF : Affiche uniquement le joueur
*/
void afficherGrilleJoueur(int type, memPart_t *memoire, int semid)
{
	int i, x, y;
	grille_t grille;
	struct sembuf op;
	char message[TAILLE_LOG_MSG];

	/* Récupérer la grille */
	grille.largeur = memoire->grille->largeur;
	grille.hauteur = memoire->grille->hauteur;
	grille.nbFantomes = memoire->grille->nbFantomes;
	grille.cases = (char*)malloc(sizeof(char) * grille.largeur * grille.hauteur);
	strcpy(grille.cases, memoire->grille->cases);

	/* Bloquer tab de positions */
	op.sem_num = POS_SEM;
	op.sem_op = -1;
	op.sem_flg = 0;
	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"Joueur (afficherGrilleJoeur) - Erreur lors du bloquage du tableau de positions");
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"Joueur (%d) - Tableau de positions bloqué.", (int)getpid());
	printf("%s\n", message);

	/* Si type = FANTOME_AFF */
	if(type == FANTOME_AFF)
	{
		/* Pour chaque fantome */
		for(i = 0; i < grille.nbFantomes; i++)
		{
			x = *(memoire->positions[i]->x);
			y = *(memoire->positions[i]->y);
			/* Si position != -1 */
			if((x != -1) && (y != -1))
			{
				/* Mettre un F dans les cases correspondantes */
				grille.cases[y * grille.largeur + x] = 'F';
			}
		}
	}
	/* Mettre un J à la position du joueur */
	x = *(memoire->positions[grille.nbFantomes]->x);
	y = *(memoire->positions[grille.nbFantomes]->y);
	grille.cases[y * grille.largeur + x] = 'J';
	/* Débloquer tab de positions */
	op.sem_op = 1;
	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"Joueur (afficherGrilleJoeur) - Erreur lors du débloquage du tableau de positions");
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"Joueur (%d) - Tableau de position bloqué.", (int)getpid());
	printf("%s\n", message);

	/* Afficher la grille */
	grille_afficher(&grille);
}

int joueur_arreter(memPart_t *memoire, int msqid, void* adresse, int semid)
{
	char message[TAILLE_LOG_MSG];
	/* Sémaphores */
	struct sembuf op;

	/* On bloque le tableau de positions */
	op.sem_num = POS_SEM;
	op.sem_op = -1;
	op.sem_flg = 0;
	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"Joueur pid°%d (joueur_arreter) - Erreur lors du bloquage du tableau de positions", (int) getpid());
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"Joueur (%d) - Tableau de positions bloqué.", (int)getpid());
	printf("%s\n", message);

	/* On remet les positions à -1 */
	*(memoire->positions[memoire->grille->nbFantomes]->x) = -1;
	*(memoire->positions[memoire->grille->nbFantomes]->y) = -1;

	/* On débloque le tableau de positions */
	op.sem_op = 1;
	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"Joueur pid°%d (joueur_arreter) - Erreur lors du débloquage du tableau de fantomes", (int) getpid());
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"Joueur (%d) - Tableau de positions débloqué.", (int)getpid());
	printf("%s\n", message);

	/* Détacher mémoire partagée */
	if(shmdt(adresse) == -1)
	{
		fprintf(stderr, ERREUR"Joueur pid°%d (joueur_arreter) - Erreur lors du détachement de la mémoire partagée", (int) getpid());
		perror(" ");
		exit(EXIT_FAILURE);
	}

	/* Envoyer message de log au logger */
	sprintf(message, "Joueur (%d) - Joueur détruit.", (int)getpid());
	envoyerMsg(msqid, message);

	return 0;
}

/* Fonction qui retourne le nombre de fantomes restants */
int compter_Fantome(memPart_t *memoire,int semid)
{
	int i, nbF = 0;
	char message[TAILLE_LOG_MSG];
	/* Sémaphores */
	struct sembuf op;

	/* On bloque le tableau de fantomes */
	op.sem_num = FANTOME_SEM;
	op.sem_op = -1;
	op.sem_flg = 0;
	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"Joueur pid°%d (compter_Fantome) - Erreur lors du bloquage du tableau de fantomes", (int) getpid());
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"Joueur (%d) - Tableau de fantomes bloqué.", (int)getpid());
	printf("%s\n", message);

	for(i = 0; i < memoire->grille->nbFantomes; i++)
	{
		if(*(memoire->fantomes[i]->pid) != -1)
		{
			nbF++;
		}
	}

	/* On débloque le tableau de fantomes */
	op.sem_op = 1;
	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"Joueur pid°%d (compter_Fantome) - Erreur lors du débloquage du tableau de fantomes", (int) getpid());
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"Joueur (%d) - Tableau de fantomes débloqué.", (int)getpid());
	printf("%s\n", message);

	return nbF;
}

void afficher_menu(int fantomesRes, int nbVues)
{
	printf("Que souhaitez-vous faire ? (Fantomes restants : %d)\n", fantomesRes);
	printf("\t5 - Aller vers le haut.\n");
	printf("\t2 - Aller vers le bas.\n");
	printf("\t1 - Aller vers la gauche.\n");
	printf("\t3 - Aller vers la droite.\n");
	printf("\t8 - Afficher fantomes (%d fois restante(s)).\n", (NB_VUES_MAX - nbVues));
	printf("\t0 - Arrêter de jouer.\n");
}

int verifier_position(int x, int y, memPart_t *memoire)
{
	if(x >= 0 && x < memoire->grille->largeur)
		if(y >= 0 && y < memoire->grille->hauteur)
			if(memoire->grille->cases[y * memoire->grille->largeur + x] != 'X')
				return 1;

	return 0;
}

/* Fonction qui regarde s'il y a un ou des fantome(s) autour du joueur */
/* S'il y en a, il leur envoie un signal pour leur demander de s'arrêter */
void traiter_fantome(int x, int y, memPart_t *memoire, int msqid, int semid)
{
	int xF, yF, i;
	char message[TAILLE_LOG_MSG];
	struct sembuf op;

	/* On parcourt tous les fantomes */
	for(i = 0; i < memoire->grille->nbFantomes; i++)
	{
		xF = *(memoire->positions[i]->x);
		yF = *(memoire->positions[i]->y);
		/* Si le fantome est dans une case adjacente au joueur */
		if((xF <= x+1 && xF >= x-1) && (yF <= y+1 && yF >= y-1))
		{
			/* Bloquer le tableau de fantomes */
			op.sem_num = FANTOME_SEM;
			op.sem_op = -1;
			op.sem_flg = 0;
			if(semop(semid, &op, 1) == -1)
			{
				fprintf(stderr, ERREUR"Joueur (main) - Erreur lors du bloquage du tableau de fantomes");
				perror(" ");
				exit(EXIT_FAILURE);
			}
			sprintf(message, INFO"Joueur (%d) - Tableau de fantomes bloqué.", (int)getpid());
			printf("%s\n", message);

			/* Envoi d'un signal au fantome */
			if(kill(*(memoire->fantomes[i]->pid), SIGUSR1) == -1)
			{
				fprintf(stderr, ERREUR"Joueur (main) - Erreur lors de l'envoi du signal au fantome");
				perror(" ");
				exit(EXIT_FAILURE);
			}
			sprintf(message, "Joueur (%d) - Signal de destruction au Fantome (nom = %s, pid = %d) envoyé.", (int)getpid(), memoire->fantomes[i]->nom, *(memoire->fantomes[i]->pid));
			envoyerMsg(msqid, message);

			/* Débloque le tableau de fantomes */
			op.sem_op = 1;
			if(semop(semid, &op, 1) == -1)
			{
				fprintf(stderr, ERREUR"Joueur (main) - Erreur lors du débloquage du tableau de fantomes");
				perror(" ");
				exit(EXIT_FAILURE);
			}
			sprintf(message, INFO"Joueur (%d) - Tableau de fantomes débloqué.", (int)getpid());
			printf("%s\n", message);
		}
	}
}

int main(int argc, char** argv)
{
	int cleMsg, x, xOff=0, y, yOff=0, msqid, semid, shmid, continuer = 1, cmde, nbVues = 0, fantomesRes;
	char c;
	char message[TAILLE_LOG_MSG];
	void *adresse;
	memPart_t *memoire;

	/* Messages */
	msgCle_t msgCle;
	msgStop_t msgStop;

	/* Sémaphores */
	struct sembuf op;

	/* gestion des arguments (clé de la file de messages + nom du fantôme) */
	if(argc < 2)
	{
		fprintf(stderr, ERREUR"Joueur (main) - Manque d'arguments (clé de la file de message + nom du fantome)\n");
		exit(EXIT_FAILURE);
	}

	cleMsg = atoi(argv[1]);

	/* Récupérer la file de message pour récupérer les autres clés */
	if((msqid = msgget((key_t)cleMsg, 0)) == -1)
	{
		fprintf(stderr, ERREUR"Joueur (main) - Erreur lors de la récupération de la file de message");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(msgrcv(msqid, &msgCle, sizeof(msgCle_t) - sizeof(long), MSG_CLE, 0) == -1)
	{
		fprintf(stderr, ERREUR"Joueur (main) - Erreur lors de la récupération du message de clés");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(msgsnd(msqid, &msgCle, sizeof(msgCle_t) - sizeof(long), MSG_CLE) == -1)
	{
		fprintf(stderr, ERREUR"Joueur (main) - Erreur lors du renvoie du message de clés");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	/* Récupérer la mémoire partagée*/
	if((shmid = shmget((key_t)msgCle.cleSeg, 0, 0)) == -1)
	{
		fprintf(stderr, ERREUR"Joueur (main) - Erreur lors de la récupération de la mémoire partagée");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if((adresse = shmat(shmid, NULL, 0)) == (void*)-1)
	{
		fprintf(stderr, ERREUR"Joueur (main) - Erreur lors de l'attachement de la mémoire partagée");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	memoire = memPart_charger(adresse);

	/* Récupérer le tableau de sémaphore */
	if((semid = semget((key_t)msgCle.cleSem, 0, 0)) == -1)
	{
		fprintf(stderr, ERREUR"Joueur (main) - Erreur lors de la récupération du tableau de sémaphores");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	/* On bloque le tableau de positions */
	op.sem_num = POS_SEM;
	op.sem_op = -1;
	op.sem_flg = 0;

	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"Joueur (main) - Erreur lors du bloquage du tableau de positions");
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"Joueur (%d) - Tableau de positions bloqué.", (int)getpid());
	printf("%s\n", message);

	/* On stocke la position du joueur*/
	x = *(memoire->positions[memoire->grille->nbFantomes]->x) = 0;
	y = *(memoire->positions[memoire->grille->nbFantomes]->y) = 0;

	/* On débloque le tableau de positions */
	op.sem_op = 1;

	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"Joueur (main) - Erreur lors du débloquage du tableau de positions");
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"Joueur (%d) - Tableau de positions débloqué.", (int)getpid());
	printf("%s\n", message);

	/* Envoyer message au logger pour dire que le Joueur est prêt */
	sprintf(message, "Joueur (%d) - Joueur initialisé à la position [%d,%d].", (int)getpid(), x, y);
	envoyerMsg(msqid, message);

	/* Commencer la boucle */
	while(continuer)
	{
		/* Afficher la grille qu'avec le joueur */
		afficherGrilleJoueur(NORMAL_AFF, memoire, semid);
		/* Demander à se déplacer */
		fantomesRes = compter_Fantome(memoire, semid);
		afficher_menu(fantomesRes, nbVues);

		printf(ACTION"Choix :");
		if(scanf("%d", &cmde) == EOF)
		{
			printf(WARNING"Donneur d'ordres - Erreur lors de la saisie clavier de la commande principale\n");
			perror(" ");
		}
		while(((c=getchar()) != '\n') && ( c != EOF));

		sprintf(message, "Joueur (%d) - Le joueur à choisi la commande %d.", (int)getpid(), cmde);
		envoyerMsg(msqid, message);

		switch(cmde)
		{
			case 0:
				/* Envoie d'un signal au logger */
				if(kill(msgCle.pidLogger, SIGINT) == -1)
				{
					fprintf(stderr, ERREUR"Joueur (main) - Erreur lors de l'envoi du signal d'arrêt au logger");
					perror(" ");
					exit(EXIT_FAILURE);
				}
				sprintf(message, "Joueur (%d) - Signal d'arrêt envoyé au logger.", (int)getpid());
				envoyerMsg(msqid, message);
				continuer = 0;
				break;
			case 8:
				if(nbVues < NB_VUES_MAX)
				{
					nbVues++;
					afficherGrilleJoueur(FANTOME_AFF, memoire, semid);	
					sprintf(message, "Joueur (%d) - Affichage des fantomes (%d restante(s)).", (int)getpid(), (NB_VUES_MAX - nbVues));
					envoyerMsg(msqid, message);
				}
				else
				{
					sprintf(message, "Joueur (%d) - Affichage des fantomes non autorisé).", (int)getpid());
					printf(WARNING"%s\n", message);
					envoyerMsg(msqid, message);
				}
				break;
			case 5:
				/* Vers le haut */
				xOff = 0;
				yOff = -1;
				break;
			case 2:
				/* Vers le bas */
				xOff = 0;
				yOff = 1;
				break;
			case 1:
				/* Vers la gauche */
				xOff = -1;
				yOff = 0;
				break;
			case 3:
				/* Vers la droite */
				xOff = 1;
				yOff = 0;
				break;
			default:
				xOff = 0;
				yOff = 0;
				break;
		}

		/* Vérifier si placement correcte */
		if( verifier_position(x+xOff, y+yOff, memoire))
		{		
			/* Bloquer tableau de positions */
			op.sem_num = POS_SEM;
			op.sem_op = -1;
			op.sem_flg = 0;

			if(semop(semid, &op, 1) == -1)
			{
				fprintf(stderr, ERREUR"Joueur (main) - Erreur lors du bloquage du tableau de positions");
				perror(" ");
				exit(EXIT_FAILURE);
			}
			sprintf(message, INFO"Joueur (%d) - Tableau de positions bloqué.", (int)getpid());
			printf("%s\n", message);
			

			/* Stocker position */
			x = *(memoire->positions[memoire->grille->nbFantomes]->x) += xOff;
			y = *(memoire->positions[memoire->grille->nbFantomes]->y) += yOff; 

			
			/* Vérifier si fantome est à côté */
			traiter_fantome(x, y, memoire, msqid, semid);

			/* Débloquer le tableau de positions */
			op.sem_op = 1;

			if(semop(semid, &op, 1) == -1)
			{
				fprintf(stderr, ERREUR"Joueur (main) - Erreur lors du débloquage du tableau de positions");
				perror(" ");
				exit(EXIT_FAILURE);
			}
			sprintf(message, INFO"Joueur (%d) - Tableau de positions débloqué.", (int)getpid());
			printf("%s\n", message);
		}
		

		/* Vérifier le message d'arrêt du logger */
		if(msgrcv(msqid, &msgStop, sizeof(msgStop_t) - sizeof(long), MSG_STOP, IPC_NOWAIT) == -1)
		{		
			if(errno != ENOMSG)
			{
				fprintf(stderr, ERREUR"Joueur (main) - Erreur lors de la réception du message de stop");
				perror(" ");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			sprintf(message, "Joueur (%d) - Message d'arrêt reçu.", (int)getpid());
			envoyerMsg(msqid, message);
			if(msgsnd(msqid, &msgStop, sizeof(msgStop_t) - sizeof(long), MSG_STOP) == -1)
			{
				fprintf(stderr, ERREUR"Joueur (main) - Erreur lors du renvoie du message de stop");
				perror(" ");
				exit(EXIT_FAILURE);
			}
			continuer = joueur_arreter(memoire, msqid, adresse, semid);
		}
	}

	sprintf(message, "Joueur (%d) - Joueur arrêté.", (int)getpid());
	envoyerMsg(msqid, message);
	return EXIT_SUCCESS;
}