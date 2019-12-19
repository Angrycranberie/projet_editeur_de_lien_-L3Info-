#ifndef _PROGBITS_H
#define	_PROGBITS_H 1
#include <elf.h>

// Structure permettant de se souvenir des sectiosn que l'on doit merger. si id_section_merge[j] = i, cela veut dire que la section j du fichier 2 doit etre concatennee a la section i du fichier 1. si cela vaut -1, cela signifie 
// qu'il ne faut pas la merge.
typedef struct
{
  int nbmerge;
  int id_section_merge[100];
} Merge_table_progbits;

// Version 64 bits

typedef struct
{
  int taille;
  int offset;
  char name[100];
  unsigned char* content;
} Section_progbits;


typedef struct 
{
  int nbSections;
  Section_progbits sections[100];
  int index_section[100];
  // Indique si une fusion à eu lieu : O pour pas de fusion, 1 pour fusion
  int fusion [100];
  Merge_table_progbits Mtable;
} Table_sections;

Section_progbits init_section_size(int taille);
void affiche_table_section(Table_sections Tablesec);

// Versions 32 bits
void getname_section_32(FILE* f, Elf32_Ehdr header, int id, char* nom);
Merge_table_progbits initmerge_32(FILE* f2, Elf32_Ehdr header1, Elf32_Ehdr header2);
Merge_table_progbits search_progbits_f2_32(FILE* f1, FILE* f2, Elf32_Ehdr header1, Elf32_Ehdr header2);
int verif_fusion_progbits_32 (int id, Merge_table_progbits Mtable, Elf32_Ehdr header);
Table_sections get_merged_progbits_32 (FILE* f1, FILE* f2, Elf32_Ehdr header1, Elf32_Ehdr header2, Merge_table_progbits Mtable);

// Versions 64 bits
void getname_section_64(FILE* f, Elf64_Ehdr header, int id, char* nom);
Merge_table_progbits initmerge_64(FILE* f2, Elf64_Ehdr header1, Elf64_Ehdr header2);
Merge_table_progbits search_progbits_f2_64(FILE* f1, FILE* f2, Elf64_Ehdr header1, Elf64_Ehdr header2);
int verif_fusion_progbits_64 (int id, Merge_table_progbits Mtable, Elf64_Ehdr header);

Table_sections get_merged_progbits_64 (FILE* f1, FILE* f2, Elf64_Ehdr header1, Elf64_Ehdr header2, Merge_table_progbits Mtable);


#endif