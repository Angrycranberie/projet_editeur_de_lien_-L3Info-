#ifndef _TABLESECTION_H
#define	_TABLESECTION_H 1
#include <elf.h>

void Tablesection64(FILE* f, Elf64_Ehdr header);

void Tablesection32(FILE* f, Elf32_Ehdr header);

#endif