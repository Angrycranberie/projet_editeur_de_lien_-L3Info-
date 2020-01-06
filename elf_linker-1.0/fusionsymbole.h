#ifndef _FUSIONSYMBOLE_H
#define	_FUSIONSYMBOLE_H
#include <elf.h>
#include "progbits.h"

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



symbol_table_64 get_symbol_table_64(FILE * f, Elf64_Ehdr header); // Recupère la table de symbole du fichier elf passé en paramètres.

void print_symbol_table_64(symbol_table_64 symtable); // Affiche la table de symboles passée en paramètre

// Les deux fonctions ci-dessus sont a déplacer dans la partie 4.

symbol_table_64 fusion_symbol_tables_64(Table_sections sections, symbol_table_64 symtab1, symbol_table_64 symtab2); // Calcule la fusion des tables de symboles des deux fichiers elf dont les tables de symboles sont passees
                                                                                                                    // en argument, ainsi que la tables des sections progbits fusionnees. Renvoie la table des symboles fusionnees,
                                                                                                                    // ainsi que la table des chaines associee (dans une structure symbol_table64).
                                                                                                                   
// Versions 32 bits des fonctions précédentes) //

symbol_table_32 get_symbol_table_32(FILE * f, Elf32_Ehdr header);

void print_symbol_table_32(symbol_table_32 symtable); 

// Les deux fonctions ci-dessus sont a déplacer dans la partie 4.

symbol_table_32 fusion_symbol_tables_32(Table_sections sections, symbol_table_32 symtab1, symbol_table_32 symtab2); 
                              


#endif