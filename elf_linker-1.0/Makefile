CC = gcc

all : readelf_entete readelf_section readelf_contenu readelf_symbole readelf_reimplant

readelf_entete: entete.o readelf_entete.o
	${CC} -o readelf_entete entete.o readelf_entete.o
  
readelf_section: entete.o readelf_section.o tablesection.o
	${CC} -o readelf_section readelf_section.o entete.o tablesection.o
 
readelf_contenu: entete.o readelf_contenu.o readsection.o
	${CC} -o readelf_contenu readelf_contenu.o entete.o readsection.o
 
readelf_symbole: entete.o readelf_symbole.o symboltable.o
	${CC} -o readelf_symbole readelf_symbole.o entete.o symboltable.o
 
readelf_reimplant: entete.o readelf_reimplant.o reloca.o
	${CC} -o readelf_reimplant readelf_reimplant.o entete.o reloca.o
  
entete.o: entete.c entete.h
	${CC} ${CFLAGS} -c entete.c
  
readelf_entete.o: readelf_entete.c entete.h
	${CC} ${CFLAGS} -c readelf_entete.c
  
tablesection.o: tablesection.c tablesection.h
	${CC} ${CFLAGS} -c tablesection.c
  
readelf_section.o: readelf_section.c entete.h tablesection.h
	${CC} ${CFLAGS} -c readelf_section.c
  
readsection.o: readsection.c readsection.h
	${CC} ${CFLAGS} -c readsection.c
 
readelf_contenu.o: readelf_contenu.c entete.h readsection.h
	${CC} ${CFLAGS} -c readelf_contenu.c

symboltable.o: symboltable.c symboltable.h
	${CC} ${CFLAGS} -c symboltable.c
 
readelf_symbole.o: readelf_symbole.c entete.h symboltable.h
	${CC} ${CFLAGS} -c readelf_symbole.c
 
reloca.o: reloca.c reloca.h
	${CC} ${CFLAGS} -c reloca.c
 
readelf_reimplant.o: readelf_reimplant.c entete.h reloca.h
	${CC} ${CFLAGS} -c readelf_reimplant.c

clean_exec:
	rm *.o readelf_entete readelf_section readelf_contenu readelf_symbole readelf_reimplant

clean_test:
	rm Examples_loader/*.o
	rm -r Resultat

clean:
	make clean_exec > /dev/null
	make clean_test >/dev/null
