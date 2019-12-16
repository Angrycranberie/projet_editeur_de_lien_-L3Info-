#include <stdio.h>
#include "entete.h"
#include "elf.h"
// Donne les informations sur le header d'un fichier elf passé en argument.
int main (int argc, char ** argv){
  if (argc!=2) {
    printf("Erreur: PrÃ©cisez un nom de fichier, et un seul\n");
    return 1;
  }
  printf("a\n");
  FILE * f;
  f = fopen(argv[1],"r");
  if (f==NULL){
    printf("Erreur lors de l'ouverture en lecture du fichier\n");
    return 1;
  }
  Elf_identificator headid;
  headid = Lecture_identificateur(f);
  if (headid.e_ident[4] == ELFCLASS32){
    Lecture32(f,headid);
  }
  else {
    Lecture64(f,headid);
  }
}