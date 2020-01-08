#ifndef _HEADER_H
#define	_HEADER_H 1
#include <elf.h>


/* Fonction de lecture de l'entête d'un fichier ELF passé en paramètre.
La lecture se fait par défaut en 64 bits pour faciliter le changement ensuite si c'est en 32 bits. */
Elf64_Ehdr read_header(FILE* f);

/* Fonction d'affichage de l'entête d'un fichier ELF.
Le paramètre est un entête sous la forme d'une structure 'Elf64_Ehdr' fournie par 'elf.h'.*/
void print_header(Elf64_Ehdr header);

#endif