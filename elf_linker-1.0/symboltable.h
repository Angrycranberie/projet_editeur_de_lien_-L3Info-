#ifndef _SYMBOLTABLE_H
#define	_SYMBOLTABLE_H 1
#include <elf.h>

void read_symbols_tables_32 (int id, FILE* f, Elf32_Ehdr header);

void search_symbols_tables_32 (FILE* f, Elf32_Ehdr header);

void read_symbols_tables_64 (int id, FILE* f, Elf64_Ehdr header);

void search_symbols_tables_64 (FILE* f, Elf64_Ehdr header);

#endif