#ifndef _TABLESECTION_H
#define _TABLESECTION_H 1
#include <elf.h>
#include "util.h"


// Constante 'Section with compressed data', au cas où 'elf.h' serait indisponible.
#define SHF_COMPRESSED (1 << 11)

// Structure permettant de stocker le nom d'une section d'un fichier ELF.
typedef struct name
{
    char nom[100];
} section_name;

// Structure regroupant toutes les informations de chaque section du fichier et leur nom associé.
typedef struct sections
{
    Elf64_Shdr *sec_list;
    section_name *names;
    int nb_section;
} section_list;

/* Fonction de lecture d'une table de section d'un fichier ELF.
Les entrées sont un fichier ELF et son entête.
Renvoie une section sous la forme d'une structure 'section_list'.*/
section_list *read_tablesection(FILE *f, Elf64_Ehdr header);

/* Fonction d'affichage d'une table de section.
L'entrée est une section sous la forme d'une structure 'section_list'.*/
void print_sectionTable(section_list *sections);

#endif
