#include "fantome.h"

int fantome_arreter(int numFantome, memPart_t *memoire, int msqid, void* adresse, int semid)
{
	char nomFantome[TAILLE_NOM_FANTOME];
	char message[TAILLE_LOG_MSG];
	
	/* Sémaphores */
	struct sembuf op;

	/* On bloque le tableau de fantomes */
	op.sem_num = FANTOME_SEM;
	op.sem_op = -1;
	op.sem_flg = 0;

	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"Fantome pid°%d (fantome_arreter) - Erreur lors du bloquage du tableau de fantomes", (int) getpid());
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"%s (%d) - Tableau de fantomes bloqué.", memoire->fantomes[numFantome]->nom, (int)getpid());
	printf("%s\n", message);

	/* Mettre le pid à -1 et le nom à fantome */
	*(memoire->fantomes[numFantome]->pid) = (pid_t) -1;
	strcpy(nomFantome, memoire->fantomes[numFantome]->nom);
	strcpy(memoire->fantomes[numFantome]->nom, "fantome");

	/* On débloque le tableau de fantomes */
	op.sem_op = 1;

	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"Fantome pid°%d (fantome_arreter) - Erreur lors du débloquage du tableau de fantomes", (int) getpid());
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"%s (%d) - Tableau de fantomes débloqué.", nomFantome, (int)getpid());
	printf("%s\n", message);

	/* On bloque le tableau de positions */
	op.sem_num = POS_SEM;
	op.sem_op = -1;
	op.sem_flg = 0;

	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"Fantome pid°%d (fantome_arreter) - Erreur lors du bloquage du tableau de positions", (int) getpid());
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"%s (%d) - Tableau de positions bloqué.", nomFantome, (int)getpid());
	printf("%s\n", message);
	/* On remet les positions à -1 */
	*(memoire->positions[numFantome]->x) = -1;
	*(memoire->positions[numFantome]->y) = -1;

	/* On débloque le tableau de positions */
	op.sem_op = 1;

	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"Fantome pid°%d (fantome_arreter) - Erreur lors du débloquage du tableau de fantomes", (int) getpid());
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"%s (%d) - Tableau de positions débloqué.", nomFantome, (int)getpid());
	printf("%s\n", message);

	/* Détacher mémoire partagée */
	if(shmdt(adresse) == -1)
	{
		fprintf(stderr, ERREUR"Fantome pid°%d (fantome_arreter) - Erreur lors du détachement de la mémoire partagée", (int) getpid());
		perror(" ");
		exit(EXIT_FAILURE);
	}

	/* Envoyer message de log au logger */
	sprintf(message, "%s (%d) - Fantome arrêté.", nomFantome, (int)getpid());
	envoyerMsg(msqid, message);
	printf("%s\n", message);
	return 0;
}

position_t *initPosition(grille_t *grille)
{
	position_t *position;
	int correcte = 0;

	position = (position_t*)malloc(sizeof(position_t));
	position->x = (int*) malloc(sizeof(int));
	position->y = (int*) malloc(sizeof(int));

	while(!correcte)
	{
		*(position->x) = rand() % (grille->largeur);
		*(position->y) = rand() % (grille->hauteur);
		if(grille->cases[*(position->y) * grille->largeur + *(position->x)] != 'X')
		{
			correcte = 1;
		}
	}

	return position;
}

void trouverPosition(grille_t *grille, position_t *position)
{
	int xOff, xTemp, yOff, yTemp, correcte = 0;

	while(!correcte)
	{
		xOff = -1 + rand() % 3;
		xTemp = *(position->x) + xOff;
		yOff = -1 + rand() % 3;
		yTemp = *(position->y) + yOff;


		if(grille->cases[yTemp * grille->largeur + xTemp] != 'X')
		{
			if((yTemp >= 0) && (yTemp < grille->hauteur) 
				&& (xTemp >= 0) && (xTemp < grille->largeur))
			{
				*(position->x) = xTemp;
				*(position->y) = yTemp;
				correcte = 1;	
			}
		}
	}
}

void verifier_depl(int msqid, int semid, int numFantome, fantome_t *fantome, position_t *position, memPart_t *memoire)
{
	char message[TAILLE_LOG_MSG];
	msgDepl_t msgDepl;

	struct sembuf op;

	if(msgrcv(msqid, &msgDepl, sizeof(msgDepl_t) - sizeof(long), getpid(), IPC_NOWAIT) == -1)
	{
		if(errno != ENOMSG)
		{
			fprintf(stderr, ERREUR"%s (main) - Erreur lors de la réception du message de déplacement", fantome->nom);
			perror(" ");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		sprintf(message, "%s (%d) - Message de déplacement reçu.", fantome->nom, (int)getpid());
		envoyerMsg(msqid, message);
		printf("%s\n", message);
		trouverPosition(memoire->grille, position);

		/* On bloque le tableau de positions */
		op.sem_num = POS_SEM;
		op.sem_op = -1;
		op.sem_flg = 0;
		if(semop(semid, &op, 1) == -1)
		{
			fprintf(stderr, ERREUR"%s (main) - Erreur lors du bloquage du tableau de positions", fantome->nom);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		/* Envoyer message de log au logger */
		sprintf(message, INFO"%s (%d) - Tableau de positions bloqué.", fantome->nom, (int)getpid());
		printf("%s\n", message);

		/* On modifie la position */
		*(memoire->positions[numFantome]->x) = *(position->x);
		*(memoire->positions[numFantome]->y) = *(position->y);

		/* On débloque le tableau de positions */
		op.sem_op = 1;
		if(semop(semid, &op, 1) == -1)
		{
			fprintf(stderr, ERREUR"%s (main) - Erreur lors du débloquage du tableau de positions", fantome->nom);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		sprintf(message, INFO"%s (%d) - Tableau de positions débloqué.", fantome->nom, (int)getpid());
		printf("%s\n", message);

		/* On envoie un message de log au logger */
		sprintf(message, "%s (%d) - Fantome déplacé à la position [%d,%d].", fantome->nom, (int)getpid(), *(position->x), *(position->y));
		envoyerMsg(msqid, message);
		printf("%s\n", message);
	}
}

int main(int argc, char** argv)
{
	int cleMsg, msqid, shmid, semid, numFantome, i = 0, continuer = 1;
	fantome_t fantome;
	void *adresse;
	memPart_t* memoire;
	pid_t pidTemp;
	position_t *position;
	char message[TAILLE_LOG_MSG];

	/* Messages */
	msgCle_t msgCle;
	msgStop_t msgStop;

	/* Sémaphores */
	struct sembuf op;

	/* Signaux */
	int resSig;
	sigset_t ensemble;
	struct timespec timer;

	sigemptyset(&ensemble);
	sigaddset(&ensemble, SIGUSR1);
	sigprocmask(SIG_BLOCK, &ensemble, NULL);
	timer.tv_sec = 0;
	timer.tv_nsec = 100000;

	/* gestion des arguments (clé de la file de messages + nom du fantôme) */
	if(argc < 3)
	{
		fprintf(stderr, ERREUR"Fantome (main) - Manque d'arguments (clé de la file de message + nom du fantome)\n");
		exit(EXIT_FAILURE);
	}

	srand(time(NULL));

	cleMsg = atoi(argv[1]);

	fantome.nom = (char*)malloc(sizeof(char) * TAILLE_NOM_FANTOME);
	fantome.pid = (pid_t*)malloc(sizeof(pid_t));

	strcpy(fantome.nom, argv[2]);
	*(fantome.pid) = getpid();

	/* Récupérer la file de message pour récupérer les autres clés */
	if((msqid = msgget((key_t)cleMsg, 0)) == -1)
	{
		fprintf(stderr, ERREUR"%s (main) - Erreur lors de la récupération de la file de message", fantome.nom);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(msgrcv(msqid, &msgCle, sizeof(msgCle_t) - sizeof(long), MSG_CLE, 0) == -1)
	{
		fprintf(stderr, ERREUR"%s (main) - Erreur lors de la récupération du message de clés", fantome.nom);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(msgsnd(msqid, &msgCle, sizeof(msgCle_t) - sizeof(long), MSG_CLE) == -1)
	{
		fprintf(stderr, ERREUR"%s (main) - Erreur lors du renvoie du message de clés", fantome.nom);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	/* Récupérer la mémoire partagée*/
	if((shmid = shmget((key_t)msgCle.cleSeg, 0, 0)) == -1)
	{
		fprintf(stderr, ERREUR"%s (main) - Erreur lors de la récupération de la mémoire partagée", fantome.nom);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if((adresse = shmat(shmid, NULL, 0)) == (void*)-1)
	{
		fprintf(stderr, ERREUR"%s (main) - Erreur lors de l'attachement de la mémoire partagée", fantome.nom);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	memoire = memPart_charger(adresse);

	/* Récupérer le tableau de sémaphore */
	if((semid = semget((key_t)msgCle.cleSem, 0, 0)) == -1)
	{
		fprintf(stderr, ERREUR"%s (main) - Erreur lors de la récupération du tableau de sémaphores", fantome.nom);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	/* On bloque le tableau de fantomes */
	op.sem_num = FANTOME_SEM;
	op.sem_op = -1;
	op.sem_flg = 0;

	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"%s (main) - Erreur lors du bloquage du tableau de fantomes", fantome.nom);
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"%s (%d) - Tableau de fantomes bloqué.", fantome.nom, (int)getpid());
	printf("%s\n", message);

	/* On récupère le numéro et on sauvegarde pid et nom */
	while((pidTemp = *(memoire->fantomes[i]->pid)) != -1)
	{
		i++;
	}

	numFantome = i;
	*(memoire->fantomes[numFantome]->pid) = *(fantome.pid);
	strcpy(memoire->fantomes[numFantome]->nom, fantome.nom);

	/* On débloque le tableau de fantomes */
	op.sem_op = 1;

	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"%s (main) - Erreur lors du débloquage du tableau de fantomes", fantome.nom);
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"%s (%d) - Tableau de fantomes débloqué.", fantome.nom, (int)getpid());
	printf("%s\n", message);

	position = initPosition(memoire->grille);

	/* On bloque le tableau de positions */
	op.sem_num = POS_SEM;
	op.sem_op = -1;
	op.sem_flg = 0;

	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"%s (main) - Erreur lors du bloquage du tableau de positions", fantome.nom);
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"%s (%d) - Tableau de positions bloqué.", fantome.nom, (int)getpid());
	printf("%s\n", message);

	/* On stocke la position */
	*(memoire->positions[numFantome]->x) = *(position->x);
	*(memoire->positions[numFantome]->y) = *(position->y);

	/* On débloque le tableau de positions */
	op.sem_op = 1;

	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"%s (main) - Erreur lors du débloquage du tableau de positions", fantome.nom);
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, INFO"%s (%d) - Tableau de positions débloqué.", fantome.nom, (int)getpid());
	printf("%s\n", message);

	/* Envoyer message au logger pour dire que le fantôme est prêt */
	sprintf(message, "%s (%d) - Fantome initialisé à la position [%d,%d] et au numéro %d.", fantome.nom, (int)getpid(), *(position->x), *(position->y), numFantome);
	envoyerMsg(msqid, message);
	printf("%s\n", message);

	/* Commencer la boucle */
	while(continuer)
	{
		/* Vérification si le joueur n'a pas tué le fantôme avec un signal */
		if((resSig = sigtimedwait(&ensemble, NULL, &timer)) == -1)
		{
			if(errno != EAGAIN)
			{
				fprintf(stderr, ERREUR"Logger (main) - Erreur lors de l'attente d'un signal");
				perror(" ");
				exit(EXIT_FAILURE);
			}
		}
		/* Si fantôme tué */
		if(resSig == SIGUSR1)
		{
			sprintf(message, "%s (%d) - Signal de destruction reçu.", fantome.nom, (int)getpid());
			envoyerMsg(msqid, message);
			printf("%s\n", message);
			/* Arrêt du fantôme */
			continuer =fantome_arreter(numFantome, memoire, msqid, adresse, semid);
		}

		/* Vérifier message du logger pour le déplacement */
		verifier_depl(msqid, semid, numFantome, &fantome, position, memoire);
		
		/* Vérifier le message d'arrêt du logger */
		if(msgrcv(msqid, &msgStop, sizeof(msgStop_t) - sizeof(long), MSG_STOP, IPC_NOWAIT) == -1)
		{
			if(errno != ENOMSG)
			{
				fprintf(stderr, ERREUR"%s (main) - Erreur lors de la réception du message de stop", fantome.nom);
				perror(" ");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			if(msgsnd(msqid, &msgStop, sizeof(msgStop_t) - sizeof(long), MSG_STOP) == -1)
			{
				fprintf(stderr, ERREUR"%s (main) - Erreur lors du renvoie du message de stop", fantome.nom);
				perror(" ");
				exit(EXIT_FAILURE);
			}
			sprintf(message, "%s (%d) - Message d'arrêt reçu.", fantome.nom, (int)getpid());
			envoyerMsg(msqid, message);
			printf("%s\n", message);
			continuer = fantome_arreter(numFantome, memoire, msqid, adresse, semid);
		}
	}
	return EXIT_SUCCESS;
}