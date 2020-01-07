#include <stdio.h>
#include "entete.h"
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
  Elf_identificator headid1;
  Elf_identificator headid2;
  headid1 = Lecture_identificateur(f1);
  headid2 = Lecture_identificateur(f2);
  
  if (headid1.e_ident[4] == ELFCLASS32 && headid2.e_ident[4] == ELFCLASS32){
    Elf32_Ehdr header1;
    Elf32_Ehdr header2;
    header1 = Lecture32(f1,headid1);
    header2 = Lecture32(f2,headid2);
    table_progbits = get_merged_progbits_32 (f1, f2, header1, header2, Mtable);
    affiche_table_section(table_progbits);
  }
  else if (headid1.e_ident[4] == ELFCLASS64 && headid2.e_ident[4] == ELFCLASS64){
    Elf64_Ehdr header1;
    Elf64_Ehdr header2;
    header1 = Lecture64(f1,headid1);
    header2 = Lecture64(f2,headid2);
    Mtable = search_progbits_f2_64(f1, f2, header1, header2);
    table_progbits = get_merged_progbits_64 (f1, f2, header1, header2, Mtable);
    affiche_table_section(table_progbits);
  }
  fclose(f1);
  fclose(f2);
  return 0;
}