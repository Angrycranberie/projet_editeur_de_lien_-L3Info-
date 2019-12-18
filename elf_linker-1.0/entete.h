#ifndef _ENTETE_H
#define	_ENTETE_H 1
#include <elf.h>

typedef struct
{
  unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
} Elf_identificator;

Elf_identificator Lecture_identificateur_vocal (FILE* f);

Elf32_Ehdr Lecture32_vocal(FILE* f, Elf_identificator headid);

Elf64_Ehdr Lecture64_vocal(FILE* f, Elf_identificator headid);

Elf_identificator Lecture_identificateur (FILE* f);

Elf32_Ehdr Lecture32(FILE* f, Elf_identificator headid);

Elf64_Ehdr Lecture64(FILE* f, Elf_identificator headid);

#endif