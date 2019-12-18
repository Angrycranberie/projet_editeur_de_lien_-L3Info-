
all : readelf_h readelf_S readelf_x readelf_sym readelf_r

readelf_h: entete.o readelf_h.o
	${CC} -o readelf_h entete.o readelf_h.o
  
readelf_S: entete.o readelf_S.o tablesection.o
	${CC} -o readelf_S readelf_S.o entete.o tablesection.o
 
readelf_x: entete.o readelf_x.o readsection.o
	${CC} -o readelf_x readelf_x.o entete.o readsection.o
 
readelf_sym: entete.o readelf_sym.o symboltable.o
	${CC} -o readelf_sym readelf_sym.o entete.o symboltable.o
 
readelf_r: entete.o readelf_r.o reloca.o
	${CC} -o readelf_r readelf_r.o entete.o reloca.o
  
entete.o: entete.c entete.h
	${CC} ${CFLAGS} -c entete.c
  
readelf_h.o: readelf_h.c entete.h
	${CC} ${CFLAGS} -c readelf_h.c
  
tablesection.o: tablesection.c tablesection.h
	${CC} ${CFLAGS} -c tablesection.c
  
readelf_S.o: readelf_S.c entete.h tablesection.h
	${CC} ${CFLAGS} -c readelf_S.c
  
readsection.o: readsection.c readsection.h
	${CC} ${CFLAGS} -c readsection.c
 
readelf_x.o: readelf_x.c entete.h readsection.h
	${CC} ${CFLAGS} -c readelf_x.c


symboltable.o: symboltable.c symboltable.h
	${CC} ${CFLAGS} -c symboltable.c
 
readelf_sym.o: readelf_sym.c entete.h symboltable.h
	${CC} ${CFLAGS} -c readelf_sym.c
 
reloca.o: reloca.c reloca.h
	${CC} ${CFLAGS} -c reloca.c
 
readelf_r.o: readelf_r.c entete.h reloca.h
	${CC} ${CFLAGS} -c readelf_r.c