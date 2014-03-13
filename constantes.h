#ifndef _CONSTANTES_
#define _CONSTANTES_

	/* Constantes pour l'affichage */
		#define ERREUR "\033[31;01m[Erreur]\033[31;00m - "
		#define INFO "\033[34;01m[Info]\033[34;00m - "
		#define WARNING "\033[33;01m[Attention]\033[33;00m - "
		#define ACTION "\033[32;01m[Action]\033[32;00m - "

	/* Constantes pour les longueurs */
		#define TAILLE_NOM_FANTOME 30

	/* Constantes pour le log */
		#define TAILLE_LOG_MSG 300

	/* Constantes pour les messages */
			/* Types de messages */
		#define MSG_LOG 3
		#define MSG_CLE 2
		#define MSG_STOP 5
		/* le type du message de déplacement correspondra au PID du fantome à déplacer */

	/* Constantes pour les sémaphores */
		#define NB_SEM 2
		#define POS_SEM 0
		#define FANTOME_SEM 1

	/* Constante pour le nombre de boucle à faire entre chaque demande de déplacement d'un fantôme */
		#define TEMPS_DEPLACEMENT 1

	/* Constantes pour le joueur */
		#define FANTOME_AFF 1
		#define NORMAL_AFF 0
		#define NB_VUES_MAX 3

	#define _POSIX_C_SOURCE 200809L
	#define _XOPEN_SOURCE

#endif