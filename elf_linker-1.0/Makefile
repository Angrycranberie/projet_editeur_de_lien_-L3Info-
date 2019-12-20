CC=gcc
CFLAGS=-Wall
PROGS=readelf_header readelf_section_list readelf_section_content readelf_symbols_table readelf_relocation
COMMON=debug.h debug.c util.h util.c

all : $(PROGS)

readelf_header: header.o readelf_header.o
	${CC} -o readelf_header header.o readelf_header.o

readelf_section_list: header.o readelf_section_list.o tablesection.o
	${CC} -o readelf_section_list readelf_section_list.o header.o tablesection.o

readelf_section_content: header.o readelf_section_content.o readsection.o
	${CC} -o readelf_section_content readelf_section_content.o header.o readsection.o

readelf_symbols_table: header.o readelf_symbols_table.o symboltable.o
	${CC} -o readelf_symbols_table readelf_symbols_table.o header.o symboltable.o

readelf_relocation: header.o readelf_relocation.o reloca.o
	${CC} -o readelf_relocation readelf_relocation.o header.o reloca.o

header.o: $(COMMON) header.c header.h
	${CC} ${CFLAGS} -c header.c

readelf_header.o: readelf_header.c header.h
	${CC} ${CFLAGS} -c readelf_header.c

tablesection.o: tablesection.c tablesection.h
	${CC} ${CFLAGS} -c tablesection.c

readelf_section_list.o: readelf_section_list.c header.h tablesection.h
	${CC} ${CFLAGS} -c readelf_section_list.c

readsection.o: readsection.c readsection.h
	${CC} ${CFLAGS} -c readsection.c

readelf_section_content.o: readelf_section_content.c header.h readsection.h
	${CC} ${CFLAGS} -c readelf_section_content.c

symboltable.o: symboltable.c symboltable.h
	${CC} ${CFLAGS} -c symboltable.c

readelf_symbols_table.o: readelf_symbols_table.c header.h symboltable.h
	${CC} ${CFLAGS} -c readelf_symbols_table.c

reloca.o: reloca.c reloca.h
	${CC} ${CFLAGS} -c reloca.c

readelf_relocation.o: readelf_relocation.c header.h reloca.h
	${CC} ${CFLAGS} -c readelf_relocation.c

clean_exec:
	rm *.o $(PROGS)

clean_test:
	rm Examples_loader/*.o
	rm -r Resultat

clean:
	make clean_exec > /dev/null
	make clean_test >/dev/null
