

#ifndef _RELOCA_H
#define	_RELOCA_H 1
#include <elf.h>

void read_rela_tables_64(int id, FILE* f, Elf64_Ehdr header, int addend);
void search_reloca_tables_64(FILE* f, Elf64_Ehdr header);

#endif