#ifndef _HEADER_H
#define	_HEADER_H 1
#include <elf.h>

Elf64_Ehdr read_header(FILE* f);

void print_header(Elf64_Ehdr header);

#endif