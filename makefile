#
# CONFIGURATION GENERALE
#

EXEC = logger fantome joueur
OBJETS = grille.o outilsGrille.o mempart.o log.o
NOM_PROJET = INFO0601_TP3_ADNOT_COUTURIER

#
# SUFFIXES
#

.SUFFIXES: .c .o

#
# OBJETS
#

EXEC_O = $(EXEC:=.o)
OBJETS_O = $(OBJETS) $(EXEC_O)

#
# ARGUMENTS ET COMPILATEUR
#

CC = gcc
CCFLAGS = -Wall -O3 -Werror -ansi -pedantic -pthread
CCLIBS = 

#
# REGLES
#

all: $(OBJETS) $(EXEC_O)
	@echo "Creation des executables..."
	@for i in $(EXEC); do \
	$(CC) -o $$i $$i.o $(OBJETS) $(CCLIBS); \
	done
	@echo "Termine."

#
# REGLES PAR DEFAUT
#

.c.o: .h
	@cd $(dir $<) && ${CC} ${CCFLAGS} -c $(notdir $<) -o $(notdir $@)

#
# REGLES GENERALES
#

clean:
	@echo "Suppresion des objets, des fichiers temporaires..."
	@rm -f $(OBJETS) $(EXEC_O)
	@rm -f *~ *#
	@rm -f $(EXEC)
	@rm -f dependances
	@echo "Termine."

depend:
	@echo "Creation des dependances..."
	@sed -e "/^# DEPENDANCES/,$$ d" makefile > dependances
	@echo "# DEPENDANCES" >> dependances
	@for i in $(OBJETS_O); do \
	$(CC) -MM -MT $$i $(CCFLAGS) `echo $$i | sed "s/\(.*\)\\.o$$/\1.c/"` >> dependances; \
	done
	@cat dependances > makefile
	@rm dependances
	@echo "Termine."

#
# CREATION ARCHIVE
#

ARCHIVE_FILES = *

archive: clean
	@echo "Creation de l'archive $(NOM_PROJET)$(shell date '+%y%m%d.tar.gz')..."
	@REP=`basename $$PWD`; cd .. && tar zcf $(NOM_PROJET)$(shell date '+%y%m%d.tar.gz') $(addprefix $$REP/,$(ARCHIVE_FILES))
	@echo "Termine."

# DEPENDANCES
grille.o: grille.c grille.h constantes.h outilsGrille.h
outilsGrille.o: outilsGrille.c outilsGrille.h constantes.h grille.h
mempart.o: mempart.c mempart.h grille.h constantes.h outilsGrille.h \
 structures.h
log.o: log.c log.h constantes.h msgStruct.h
logger.o: logger.c logger.h constantes.h grille.h outilsGrille.h \
 structures.h mempart.h msgStruct.h log.h
fantome.o: fantome.c fantome.h constantes.h msgStruct.h mempart.h \
 grille.h outilsGrille.h structures.h log.h
joueur.o: joueur.c joueur.h constantes.h msgStruct.h structures.h \
 grille.h outilsGrille.h mempart.h log.h
