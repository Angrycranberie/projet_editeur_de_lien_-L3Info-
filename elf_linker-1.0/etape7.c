#include <stdio.h>
#include "header.h"
#include <elf.h>
#include "progbits.h"
#include "fusionsymbole.h"
#include "tablesection.h"
#include "symboltable.h"
#include <string.h>
#include <stdlib.h>


// Donne les informations sur les sections du fichier elf passé en argument
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
  Elf64_Ehdr header1;
  Elf64_Ehdr header2;
  header1 = read_header(f1);
  header2 = read_header(f2);
  symbol_table_64 symtable1;
  symbol_table_64 symtable2;
  symbol_table_64 symtablefusion;

  section_list seclist1 = *read_tablesection(f1,header1);
  section_list seclist2 = *read_tablesection(f2,header2);

  symtable1 = read_symbols_tables_64(f1, header1, seclist1);
  symtable2 = read_symbols_tables_64(f2, header2, seclist2);
  
  //Resultats de l'etape 6
  Mtable = search_progbits_f2(&seclist1, &seclist2);
  table_progbits = get_merged_progbits (f1, f2, &seclist1, &seclist2, Mtable);
  
  
  print_symbol_table_64(symtable1);
  print_symbol_table_64(symtable2);
  
  //etape 7 a proprement parler (fusion des tables de symboles)
  symtablefusion = fusion_symbol_tables_64(table_progbits,  symtable1, symtable2);
  
  // affichage du resultat
  print_symbol_table_64(symtablefusion);
  

  fclose(f1);
  fclose(f2);
  return 0;
}