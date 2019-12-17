
all : readelf_h readelf_S readelf_x

readelf_h: entete.o readelf_h.o
	${CC} -o readelf_h entete.o readelf_h.o
  
readelf_S: entete.o readelf_S.o tablesection.o
	${CC} -o readelf_S readelf_S.o entete.o tablesection.o
 
readelf_x: entete.o readelf_x.o readsection.o
	${CC} -o readelf_x readelf_x.o entete.o readsection.o
  
entete.o: entete.c entete.h
	${CC} ${CFLAGS} -c entete.c
  
readelf_h.o: readelf_h.c entete.h
	${CC} ${CFLAGS} -c readelf_h.c
  
tablesection.o: tablesection.c tablesection.h entete.h
	${CC} ${CFLAGS} -c tablesection.c
  
readelf_S.o: readelf_S.c entete.h tablesection.h
	${CC} ${CFLAGS} -c readelf_S.c
  
readsection.o: readsection.c readsection.h entete.h
	${CC} ${CFLAGS} -c readsection.c
 
readelf_x.o: readelf_x.c entete.h readsection.h
	${CC} ${CFLAGS} -c readelf_x.c
