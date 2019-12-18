#include <stdio.h>
#include "entete.h"
#include <elf.h>
#include "reloca.h"
// Donne les informations sur les sections du fichier elf passe en argument
int main (int argc, char ** argv){
  if (argc!=2) {
    printf("Erreur: PrĂ©cisez un nom de fichier, et un seul\n");
    return 1;
  }
  FILE * f;
  f = fopen(argv[1],"r");
  if (f==NULL){
    printf("Erreur lors de l'ouverture en lecture du fichier\n");
    return 1;
  }
  Elf_identificator headid;
  headid = Lecture_identificateur(f);
  if (headid.e_ident[4] == ELFCLASS32){
    Elf32_Ehdr header;
    header = Lecture32(f,headid);
    // ajouter la version 32 bits
  }
  else if (headid.e_ident[4] == ELFCLASS64){
    Elf64_Ehdr header;
    header = Lecture64(f,headid);
    search_reloca_tables_64(f, header);
  }
  return 0;
}