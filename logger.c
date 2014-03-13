#include "logger.h"

int creerMemPartagee(int cle, int taille, int msqid)
{
	int shmid;
	char message[TAILLE_LOG_MSG];

	if((shmid = shmget((key_t)cle, taille, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
		fprintf(stderr, ERREUR"Logger (creerMemPartagee) - Erreur lors de la création du segment de mémoire partagée (cle = %d)", cle);
	  	perror(" ");
		exit(EXIT_FAILURE);
	}

	sprintf(message, "Logger - Mémoire partagée créée.");
	envoyerMsg(msqid, message);
	return shmid;
}

void supprimerMemPartagee(int shmid)
{
	/* Suppression du segment de memoire partagee */
  	if(shmctl(shmid, IPC_RMID, 0) == -1) {
    	fprintf(stderr, ERREUR"Logger (supprimerMemPartagee) - Erreur lors de la suppression de la mémoire partagée");
		perror(" ");
    	exit(EXIT_FAILURE);
  	}
}

int creerFileMessages(int cle)
{
	char message[TAILLE_LOG_MSG];
	int msqid;

	if((msqid = msgget((key_t)cle, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1)
	{
		fprintf(stderr, ERREUR"Logger (creerFileMessages) - Erreur lors de la création de la file de message (cle = %d)", cle);
	  	perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, "Logger - File de messages créée.");
	envoyerMsg(msqid, message);
	return msqid;
}

void supprimerFileMessages(int msqid)
{
	if(msgctl(msqid, IPC_RMID, 0) == -1) {
		fprintf(stderr, ERREUR"Logger (supprimerFileMessages) - Erreur lors de la suppression de la file de messages");
		perror(" ");
		exit(EXIT_FAILURE);
	}
}

int creerTabSem(int cle, int msqid)
{
	char message[TAILLE_LOG_MSG];
	unsigned short val[2] = {1, 1};

	int semid;

	if((semid = semget((key_t)cle, NB_SEM, S_IWUSR | S_IRUSR | IPC_CREAT | IPC_EXCL)) == -1)
	{
		fprintf(stderr, ERREUR"Logger (creerTabSem) - Erreur lors de la création du tableau de sémaphores\n");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	/* Initialisation des semaphores en exclusion mutuelle*/
	if(semctl(semid, 0, SETALL, val) == -1) {
		fprintf(stderr, ERREUR"Logger (creerTabSem) - Erreur lors de l'initialisation de la sémaphore n°1");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	sprintf(message, "Logger - Tableau de sémaphores crée.");
	envoyerMsg(msqid, message);
	return semid;
}

void supprimerTabSem(int semid)
{
	if(semctl(semid, 0, IPC_RMID) == -1)
	{
		fprintf(stderr, ERREUR"Logger (supprimerTabSem) - Erreur lors de la suppression du tableau de sémaphores");
		perror(" ");
		exit(EXIT_FAILURE);
	}
}

int logger_stop(int msqid, int shmid, int semid, pid_t pidFils)
{
	msgStop_t msgStop;
	int statut;

	msgStop.type = MSG_STOP;
	if(msgsnd(msqid, &msgStop, sizeof(msgStop_t) - sizeof(long), MSG_STOP) == -1)
	{
		fprintf(stderr, ERREUR"Logger (logger_stop) - Erreur lors de l'envoi du message d'arrêt");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(kill(pidFils, SIGUSR2) == -1)
	{
		fprintf(stderr, ERREUR"Joueur (main) - Erreur lors de l'envoi du signal d'arrêt au logger");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(wait(&statut) == -1)
	{
		fprintf(stderr, ERREUR"Logger (main) - Erreur lors de l'attente du fils");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	sleep(1);

	supprimerFileMessages(msqid);
	supprimerMemPartagee(shmid);
	supprimerTabSem(semid);
	return 0;
}

void deplacerFantome(memPart_t *memoire, int msqid, int semid)
{
	int numFantome;
	pid_t pidFantome;
	struct sembuf op;
	msgDepl_t msgdepl;
	char nomFantome[TAILLE_NOM_FANTOME];
	char message[TAILLE_LOG_MSG];

	numFantome = rand() % (memoire->grille->nbFantomes);
	/* On récupère le PID du fantome */
	op.sem_num = FANTOME_SEM;
	op.sem_op = -1;
	op.sem_flg = 0;
	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"Logger (deplacerFantome) - Erreur lors du 'puis-je' sur la sémaphore du tableau de fantomes");
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, "Logger - Tableau de fantomes bloqué.");
	printf("%s\n", message);


	pidFantome = *(memoire->fantomes[numFantome]->pid);
	strcpy(nomFantome,memoire->fantomes[numFantome]->nom);

	op.sem_op = 1;
	if(semop(semid, &op, 1) == -1)
	{
		fprintf(stderr, ERREUR"Logger (deplacerFantome) - Erreur lors du 'vas-y' sur la sémaphore du tableau de fantomes");
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, "Logger - Tableau de fantomes débloqué.");
	printf("%s\n", message);

	msgdepl.type = (long)pidFantome;

	if(pidFantome != -1)
	{
		if(msgsnd(msqid, &msgdepl, sizeof(msgDepl_t) - sizeof(long), (long)pidFantome) == -1)
		{
			fprintf(stderr, ERREUR"Logger (deplacerFantome) - Erreur lors de l'envoi de l'ordre de déplacement au fantome n°%d", numFantome);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		sprintf(message, "Logger - Message de déplacement envoyé au fantome '%s'.", nomFantome);
		envoyerMsg(msqid, message);
	}
}

void traiterMessage(char *nomLog, int msqid)
{
	msgLog_t msgLog;
	struct msqid_ds msDesc;
	int fd, msgOk = 1, stop = 0;
	/* Signaux */
	int resSig;
	sigset_t ensemble;
	struct timespec timer;

	sigemptyset(&ensemble);
	sigaddset(&ensemble, SIGUSR2);
	sigaddset(&ensemble, SIGINT);
	sigprocmask(SIG_BLOCK, &ensemble, NULL);
	timer.tv_sec = 0;
	timer.tv_nsec = 500;

	/* Ouverture du log */
	if((fd = open(nomLog, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR)) == -1)
	{
		fprintf(stderr, ERREUR"Traiterment Message - Erreur lors de l'ouverture du fichier de log");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	while(!stop || msgOk)
	{
		if(msgctl(msqid, IPC_STAT, &msDesc) == -1)
		{
			fprintf(stderr, ERREUR"Erreur lors de la récupération des stats de la file de messages\n");
			perror(" ");
			exit(EXIT_FAILURE);
		}
		/* Si on reçoit un message */
		if(msgrcv(msqid, &msgLog, sizeof(msgLog_t) - sizeof(long), MSG_LOG, IPC_NOWAIT) == -1)
		{
			if(errno != ENOMSG)
			{
				fprintf(stderr, ERREUR"Traiterment Message - Erreur lors de la réception d'un message");
				perror(" ");
				exit(EXIT_FAILURE);
			}
			msgOk = 0;
		}
		else
		{
			/* On le stocke dans le fichier de log */
			stockerMsg(fd, msgLog.msg);
			msgOk = 1;
		}

		/* On regarde si le père s'est arrêté */
		if((resSig = sigtimedwait(&ensemble, NULL, &timer)) == -1)
		{
			if(errno != EAGAIN)
			{
				fprintf(stderr, ERREUR"Logger (main) - Erreur lors de l'attente d'un signal");
				perror(" ");
				exit(EXIT_FAILURE);
			}
		}
		if(resSig == SIGUSR2)
		{
			/* On attend que les fantomes se soient arrêtés pour récupérer tous les messages */
			sleep(1);
			msgOk = 1;
			stop = 1;
		}
	}

	if(close(fd) == -1)
	{
		fprintf(stderr, ERREUR"Traiterment Message - Erreur lors de la fermeture du fichier de log");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	afficherLog("log.bin");
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
	char message[TAILLE_LOG_MSG];
	int shmid, msqid, semid, tailleMem, continuer = 1, i = 0;
	char *grilleFichier;
	char *logFichier;
	int cleMsq, cleSeg, cleSem;
	void *adresse;
	grille_t * grille;
	memPart_t *memoire;
	pid_t pid;

	/* Messages */
	msgCle_t msgCle;

	/* Signaux */
	int resSig;
	sigset_t ensemble;
	struct timespec timer;

	sigemptyset(&ensemble);
	sigaddset(&ensemble, SIGINT);
	sigprocmask(SIG_BLOCK, &ensemble, NULL);
	timer.tv_sec = 1;
	timer.tv_nsec = 0;

	srand(time(NULL));

	if(argc < 6)
	{
		fprintf(stderr, ERREUR"Logger (main) - Arguments manquants\n");
		exit(EXIT_FAILURE);
	}

	grilleFichier = argv[1];
	logFichier = argv[2];
	cleMsq = atoi(argv[3]);
	cleSeg = atoi(argv[4]);
	cleSem = atoi(argv[5]);

	/* Création de la file de messages */
	msqid = creerFileMessages(cleMsq);

	/* Récupération des informations concernant la grille */
	grille = grille_charger(grilleFichier);

	/* Calcule de la taille de la mémoire partagée */
	tailleMem = sizeof(int) * 3; 											/* nbFantomes, largeur, hauteur */
	tailleMem += grille->nbFantomes * (sizeof(pid_t) + TAILLE_NOM_FANTOME * sizeof(char)); 	/* tableau de fantomes */
	tailleMem += (grille->nbFantomes+1) * (sizeof(int) * 2); 				/* tableau de positions */
	tailleMem += sizeof(char) * (grille->largeur * grille->hauteur +1); 	/* grille */

	/* Création de la mémoire partagée */
	shmid = creerMemPartagee(cleSeg, tailleMem, msqid);

	/* Création du tableau de sémaphores */
	semid = creerTabSem(cleSem, msqid);

	/* On attache la mémoire partagée*/
	if((adresse = shmat(shmid, NULL, 0)) == (void*)-1)
	{
		fprintf(stderr, ERREUR"Logger (main) - Erreur lors de l'attachement de la mémoire partagée");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	memPart_initialiser(adresse, grille);
	memoire = memPart_charger(adresse);

	/* Envoi des cles */
	msgCle.type = MSG_CLE;
	msgCle.cleMsq = cleMsq;
	msgCle.cleSeg = cleSeg;
	msgCle.cleSem = cleSem;
	msgCle.pidLogger = getpid();

	if(msgsnd(msqid, &msgCle, sizeof(msgCle_t) - sizeof(long), MSG_CLE) == -1)
	{
		fprintf(stderr, ERREUR"Logger (main) - Erreur lors de l'envoi des clés");
		perror(" ");
		exit(EXIT_FAILURE);
	}
	sprintf(message, "Logger - Clés envoyées.");
	envoyerMsg(msqid, message);

	if((pid = fork()) == -1)
	{
		fprintf(stderr, ERREUR"Logger (main) - Erreur lors de la création du fils");
		perror(" ");
		exit(EXIT_FAILURE);
	}
	if(pid == 0)
	{
		traiterMessage(logFichier, msqid);
	}

	/* Boucle du serveur */
	while(continuer)
	{
		/* Déplace les fantomes une fois tous les TEMPS_DEPLACEMENT boucles*/
		if(i % TEMPS_DEPLACEMENT == 0)
		{
			deplacerFantome(memoire, msqid, semid);	
		}
		
		/* On regarde si on a reçu un signal + temporisation d'1sec */
		if((resSig = sigtimedwait(&ensemble, NULL, &timer)) == -1)
		{
			if(errno != EAGAIN)
			{
				fprintf(stderr, ERREUR"Logger (main) - Erreur lors de l'attente d'un signal");
				perror(" ");
				exit(EXIT_FAILURE);
			}
		}
		if(resSig == SIGINT)
		{
			sprintf(message, "Logger - Signal d'arrêt reçu.");
			envoyerMsg(msqid, message);
			continuer = logger_stop(msqid, shmid, semid, pid);
		}

		i++;
	}

	if(shmdt(adresse) == -1)
	{
		fprintf(stderr, ERREUR"Logger (main) - Erreur lors du détachement de la mémoire partagée");
		perror(" ");
		exit(EXIT_FAILURE);
	}
	grille_supprimer(&grille);

	return EXIT_SUCCESS;
}