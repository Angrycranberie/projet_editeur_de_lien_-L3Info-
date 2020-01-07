#ifndef _SYMBOLTABLE_H
#define	_SYMBOLTABLE_H 1
#include <elf.h>

// Structure representant la table de chaines associee
typedef struct
{
  int endoftable; // Donne le nombre de caracteres dans la table des chaines
  unsigned char * strings; // Table des chaines
} string_table;

// Structure permettant de stocker en memoire une table de symboles 64 bits
typedef struct
{
  int indextable; // Index de la table des symboles 
  int indexstring; // Index de la table des chaines
  int nbsymbols; // Nombre de symboles
  int tailleentree; // taille d'une entree
  Elf64_Sym * entries; // table des symboles a proprement parler
  string_table stringtable; // table des chaines associee
} symbol_table_64;

// Structure permettant de stocker en memoire une table de symboles 32 bits
typedef struct
{
  int indextable; // Index de la table des symboles 
  int indexstring; // Index de la table des chaines
  int nbsymbols; // Nombre de symboles
  int tailleentree; // taille d'une entree
  Elf32_Sym * entries; // table des symboles a proprement parler
  string_table stringtable; // table des chaines associee
} symbol_table_32;


symbol_table_32 read_symbols_tables_32 (FILE* f, Elf32_Ehdr header);

void print_symbol_table_32 (symbol_table_32 symtable);

symbol_table_64 read_symbols_tables_64 (FILE* f, Elf64_Ehdr header);

void print_symbol_table_64 (symbol_table_64 symtable);

#endif