#ifndef _TABLESECTION_H
#define	_TABLESECTION_H 1
#include <elf.h>
#include "util.h"

#define SHF_COMPRESSED (1 << 11) /* Section with compressed data. */

// structure qui regroupe toutes les informations de chaque section du fichier et leur nom associé
typedef struct sections{
    Elf64_Shdr *sec_list;
    section_name *names;
    int nb_section;
}section_list;

typedef struct name{
	char nom[100];
}section_name;

section_list *read_tablesection(FILE* f, Elf64_Ehdr header);

void print_sectionTable(section_list *sections);

#endif
