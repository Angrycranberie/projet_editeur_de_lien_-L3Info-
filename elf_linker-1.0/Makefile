
readelf_h: entete.o readelf_h.o
	${CC} -o readelf_h entete.o readelf_h.o
  
entete.o: entete.c entete.h elf.h
	${CC} ${CFLAGS} -c entete.c
  
readelf_h.o: readelf_h.c entete.h elf.h
	${CC} ${CFLAGS} -c readelf_h.c
  
  