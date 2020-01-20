

#ifndef _RELOCA_H
#define	_RELOCA_H 1
#include <elf.h>
#include "values.h"
#include "tablesection.h"
#include "header.h"



// Structure permettant de stocker en memoire une table de rel 64 bits
typedef struct
{
  int indextable; // Index de la table de rel
  char name[NOMMAX]; // nom de la section
  int nbentries; // Nombre de relocation
  Elf64_Xword tailleentree; // taille d'une entree
  int tableinfo; // index de la table ou les "trous" sont presents
  int tablesymbole; // index de la table des symboles concernee
  int relorrela; // 0 si rel, 1 si rela, indique la presence d'addend
  Elf64_Rela * entries; // contenu de la table
} rela_table_64;

// structure permettant de stocker une liste de tables de rel 64 bits
typedef struct
{
  int nbtables; // nombres de tables de rel
  rela_table_64* list // liste des tables de rel
} list_rela_table_64;



void print_rela_64 (rela_table_64 tablerela, Elf64_Ehdr header); // Affiche une table de rel
void print_list_rela_64 (list_rela_table_64 list, Elf64_Ehdr header); // Affiche une liste de tables de rel. Necessite le header pour savoir si on est en 32 ou 64 bits
list_rela_table_64 search_reloca_tables_64(FILE* f, Elf64_Ehdr header, section_list seclist); // cherche toutes les tables de rel dans le fichier donné
rela_table_64 read_rela_tables_64(int id, FILE* f, Elf64_Ehdr header, int addend, section_list seclist); // Récupère la table de rel à l'indice donnée, avec addend si addend vaut 1, sinon 0.

#endif