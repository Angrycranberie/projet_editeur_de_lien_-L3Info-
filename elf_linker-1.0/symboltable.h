#ifndef _SYMBOLTABLE_H
#define	_SYMBOLTABLE_H 1
#include <elf.h>
#include "header.h"

#include "tablesection.h"

// Structure representant la table de chaines associee
typedef struct
{
  int endoftable; // Donne le nombre de caracteres dans la table des chaines
  unsigned char * strings; // Table des chaines
} string_table;

typedef struct
{
  int id1; // Donne l'id dans la table de symbole du fichier 1. Vaut -1 si le symbole n'y figurait pas.
  int id2; // Donne l'id dans la table de symbole du fichier 2. Vaut -1 si le symbole n'y figurait pas.
} oldid;

// Structure permettant de stocker en memoire une table de symboles 64 bits
typedef struct
{
  int indextable; // Index de la table des symboles 
  int indexstring; // Index de la table des chaines
  int nbsymbols; // Nombre de symboles
  int tailleentree; // taille d'une entree
  Elf64_Sym * entries; // table des symboles a proprement parler
  oldid * renum; // table de renumerotation: Conserve pour chaque entree, dans renum.id1, l'id qu'il avait dans le fichier 1, et dans renum.id2, celui dans le fichier 2. (voir plus haut) Utilisé uniquement pour la fusion
  string_table stringtable; // table des chaines associee
} symbol_table_64;

// Structure permettant de stocker en memoire une table de symboles 32 bits (inutile ici)
typedef struct
{
  int indextable; // Index de la table des symboles 
  int indexstring; // Index de la table des chaines
  int nbsymbols; // Nombre de symboles
  int tailleentree; // taille d'une entree
  Elf32_Sym * entries; // table des symboles a proprement parler
  oldid * renum; // table de renumerotation: Conserve pour l' entree i, dans renum[i].id1, l'id qu'il avait dans le fichier 1, et dans renum[i].id2, celui dans le fichier 2. (voir plus haut) Utilisé uniquement pour la fusion
  string_table stringtable; // table des chaines associee
} symbol_table_32;


symbol_table_64 read_symbols_tables_64 (FILE* f, Elf64_Ehdr header, section_list seclist);

void print_symbol_table_64 (symbol_table_64 symtable);



// Fonctions inutiles//
symbol_table_32 read_symbols_tables_32 (FILE* f, Elf32_Ehdr header);

void print_symbol_table_32 (symbol_table_32 symtable);
// Fonctions inutiles //

#endif