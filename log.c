#include "log.h"

void envoyerMsg(int msqid, char message[TAILLE_LOG_MSG])
{
	msgLog_t msgLog;

	msgLog.type = MSG_LOG;
	msgLog.pid = getpid();
	strcpy(msgLog.msg, message);
	if(msgsnd(msqid, &msgLog, sizeof(msgLog_t)-sizeof(long), MSG_LOG) == -1)
	{
		fprintf(stderr, ERREUR"%d - Erreur lors de l'envoi du message au logger", getpid());
		perror(" ");
		exit(EXIT_FAILURE);
	}
}

void stockerMsg(int fd, char* msg)
{
	time_t timestamp;

	time(&timestamp);
	if(lseek(fd, 0, SEEK_END) == -1)
	{
		fprintf(stderr, ERREUR"Log (stockerMsg) - Erreur lors du déplacement à la fin du fichier de log\n");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(write(fd, &timestamp, sizeof(time_t)) == -1)
	{
		fprintf(stderr, ERREUR"Log (stockerMsg) - Erreur lors de l'écriture du timestamp dans le fichier de log\n");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(write(fd, msg, sizeof(char) * TAILLE_LOG_MSG) == -1)
	{
		fprintf(stderr, ERREUR"Log (stockerMsg) - Erreur lors de l'écriture du message dans le fichier de log\n");
		perror(" ");
		exit(EXIT_FAILURE);
	}	
}

void afficherLog(char *filename)
{
	int tailleLue, fd;
	time_t timestamp;
	char buffer[TAILLE_LOG_MSG], date[30];
	struct tm *tm_info;

	if((fd = open(filename, O_RDONLY)) == -1)
	{
		fprintf(stderr, ERREUR"Log (afficherLog) - Erreur lors de l'ouverture du fichier de log");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	do
	{
		if((tailleLue = read(fd, &timestamp, sizeof(time_t))) == -1)
		{
			fprintf(stderr, ERREUR"Log (afficherLog) - Erreur lors de la lecture du timestamp");
			perror(" ");
			exit(EXIT_FAILURE);
		}

		if((tailleLue = read(fd, &buffer, sizeof(char) * TAILLE_LOG_MSG)) == -1)
		{
			fprintf(stderr, ERREUR"Log (afficherLog) - Erreur lors de la lecture du message");
			perror(" ");
			exit(EXIT_FAILURE);
		}
		if(tailleLue > 0)
		{
			tm_info = localtime(&timestamp);
			strftime(date, 30,"%d %B à %H:%M:%S", tm_info);
			printf("%s - %s\n", date, buffer);
		}
	}while(tailleLue > 0);

	if(close(fd) == -1)
	{
		fprintf(stderr, ERREUR"Log (afficherLog) - Erreur lors de l'ouverture du fichier de log");
		perror(" ");
		exit(EXIT_FAILURE);
	}
}