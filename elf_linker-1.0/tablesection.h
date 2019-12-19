#ifndef _TABLESECTION_H
#define	_TABLESECTION_H 1
#include <elf.h>

#define SHF_COMPRESSED (1 << 11) /* Section with compressed data. */

void Tablesection64(FILE* f, Elf64_Ehdr header);

void Tablesection32(FILE* f, Elf32_Ehdr header);

#endif
