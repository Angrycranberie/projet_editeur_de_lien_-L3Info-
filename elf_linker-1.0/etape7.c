#include <stdio.h>
#include "entete.h"
#include <elf.h>
#include "progbits.h"
#include "fusionsymbole.h"
#include "symboltable.h"
#include <string.h>
#include <stdlib.h>


// Donne les informations sur les sections du fichier elf pass√© en argument
int main (int argc, char ** argv){
  if (argc!=3) {
    printf("Erreur: Preciser deux noms de fichiers  a fusionner\n");
    return 1;
  }
  Merge_table_progbits Mtable;
  Table_sections table_progbits;
  FILE * f1;
  FILE * f2;
  f1 = fopen(argv[1],"r");
  if (f1==NULL){
    printf("Erreur lors de l'ouverture en lecture du fichier\n");
    return 1;
  }
  f2 = fopen(argv[2],"r");
  if (f2==NULL){
    printf("Erreur lors de l'ouverture en lecture du fichier\n");
    return 1;
  }
  Elf_identificator headid1;
  Elf_identificator headid2;
  headid1 = Lecture_identificateur(f1);
  headid2 = Lecture_identificateur(f2);
  if (headid1.e_ident[4] == ELFCLASS32 && headid2.e_ident[4] == ELFCLASS32){
    symbol_table_32 symtable1;
    symbol_table_32 symtable2;
    symbol_table_32 symtablefusion;
    Elf32_Ehdr header1;
    Elf32_Ehdr header2;
    header1 = Lecture32(f1,headid1);
    header2 = Lecture32(f2,headid2);   
     
    // Cette partie pourra Ítre remplacee lorsque le programme sera modularise (on recuperera les resultats de l'etape 4)
    symtable1 = read_symbols_tables_32(f1, header1);
    symtable2 = read_symbols_tables_32(f2, header2);
    
    
    //Resultats de l'etape 6
    Mtable = search_progbits_f2_32(f1, f2, header1, header2);
    table_progbits = get_merged_progbits_32 (f1, f2, header1, header2, Mtable);
    
    
    print_symbol_table_32(symtable1);
    print_symbol_table_32(symtable2);
    
    //etape 7 a proprement parler (fusion des tables de symboles)
    symtablefusion = fusion_symbol_tables_32(table_progbits, symtable1, symtable2);
    
    // affichage du resultat
    print_symbol_table_32(symtablefusion);
  }
  else if (headid1.e_ident[4] == ELFCLASS64 && headid2.e_ident[4] == ELFCLASS64){
    symbol_table_64 symtable1;
    symbol_table_64 symtable2;
    symbol_table_64 symtablefusion;
    Elf64_Ehdr header1;
    Elf64_Ehdr header2;
    header1 = Lecture64(f1,headid1);
    header2 = Lecture64(f2,headid2);
    
    // Cette partie pourra Ítre remplacee lorsque le programme sera modularise (on recuperera les resultats de l'etape 4)
    symtable1 = read_symbols_tables_64(f1, header1);
    symtable2 = read_symbols_tables_64(f2, header2);
    
    
    //Resultats de l'etape 6
    Mtable = search_progbits_f2_64(f1, f2, header1, header2);
    table_progbits = get_merged_progbits_64 (f1, f2, header1, header2, Mtable);
    
    
    print_symbol_table_64(symtable1);
    print_symbol_table_64(symtable2);
    
    //etape 7 a proprement parler (fusion des tables de symboles)
    symtablefusion = fusion_symbol_tables_64(table_progbits, symtable1, symtable2);
    
    // affichage du resultat
    print_symbol_table_64(symtablefusion);
  }
  fclose(f1);
  fclose(f2);
  return 0;
}