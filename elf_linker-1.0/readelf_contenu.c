#include <stdio.h>
#include "entete.h"
#include <elf.h>
#include "readsection.h"
#include <string.h>
#include <stdlib.h>

// Donne les informations sur les sections du fichier elf passé en argument
int main (int argc, char ** argv){
  if (argc!=4) {
    printf("Erreur: PrĂ©cisez un nom de fichier, suivi de (nom ou id) puis du nom ou de l'id\n");
    return 1;
  }
  if (strcmp(argv[2],"nom")!=0 && strcmp(argv[2],"id")!=0) {
    printf("Erreur: Les options en seconde position sont nom et id\n");
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
  
  if (headid.e_ident[4] == ELFCLASS32 && strcmp(argv[2],"nom")==0){
    Elf32_Ehdr header;
    header = Lecture32(f,headid);
    readsection_name_32(argv[3], f, header);
  }
  else if (headid.e_ident[4] == ELFCLASS32 && strcmp(argv[2],"id")==0){
    Elf32_Ehdr header;
    header = Lecture32(f,headid);
    readsection_id_32(atoi(argv[3]), f, header);
  }
  else if (headid.e_ident[4] == ELFCLASS64 && strcmp(argv[2],"nom")==0){
    Elf64_Ehdr header;
    header = Lecture64(f,headid);
    readsection_name_64(argv[3], f, header);
  }
  else if (headid.e_ident[4] == ELFCLASS64 && strcmp(argv[2],"id")==0){
    Elf64_Ehdr header;
    header = Lecture64(f,headid);
    readsection_id_64(atoi(argv[3]), f, header);
  }
  return 0;
}