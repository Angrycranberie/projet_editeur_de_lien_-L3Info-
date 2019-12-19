#ifndef _ENTETE_MODULARISE_H
#define	_ENTETE_MODULARISE_H 1
#include "elf.h"

Elf64_Ehdr Lecture(FILE* f);
void affichage_header(Elf64_Ehdr header);

#endif