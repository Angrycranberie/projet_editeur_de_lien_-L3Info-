
all : readelf_h readelf_S

readelf_h: entete.o readelf_h.o
	${CC} -o readelf_h entete.o readelf_h.o
  
readelf_S: entete.o readelf_S.o tablesection.o
	${CC} -o readelf_S readelf_S.o entete.o tablesection.o
  
entete.o: entete.c entete.h
	${CC} ${CFLAGS} -c entete.c
  
readelf_h.o: readelf_h.c entete.h
	${CC} ${CFLAGS} -c readelf_h.c
  
tablesection.o: tablesection.c tablesection.h
	${CC} ${CFLAGS} -c tablesection.c
  
readelf_S.o: readelf_S.c entete.h tablesection.h
	${CC} ${CFLAGS} -c readelf_S.c
  
  