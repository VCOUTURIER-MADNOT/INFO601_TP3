#ifndef _LOG_
#define _LOG_

	#include "constantes.h"
	#include "msgStruct.h"
	#include <sys/msg.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <time.h>
	#include <unistd.h>
	#include <errno.h>
	#include <fcntl.h>


	void envoyerMsg(int, char[TAILLE_LOG_MSG]);
	void stockerMsg(int, char[TAILLE_LOG_MSG]);
	void afficherLog(char*);

#endif