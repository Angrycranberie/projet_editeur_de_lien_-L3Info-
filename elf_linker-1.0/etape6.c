#include <stdio.h>
#include "header.h"
#include <elf.h>
#include "progbits.h"
#include <string.h>
#include <stdlib.h>

// Donne les informations sur les sections du fichier elf passé en argument
int main (int argc, char ** argv){
  if (argc!=3) {
    printf("Erreur: Preciser deux noms de fichiers  a fusionner\n");
    return 1;
  }
  Merge_table_progbits Mtable;
  FILE * f1;
  FILE * f2;
  Table_sections table_progbits;
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
  section_list *sections1 = read_tablesection(f1,header1);
  section_list *sections2 = read_tablesection(f2,header2);
  Mtable = search_progbits_f2(sections1,sections2);
  table_progbits = get_merged_progbits (f1, f2,sections1,sections2, Mtable);
  affiche_table_section(table_progbits);
  fclose(f1);
  fclose(f2);
  return 0;
}