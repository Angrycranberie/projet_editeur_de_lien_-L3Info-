#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include "header.h"
#include "readsection.h"

// Donne les informations sur les sections du fichier ELF passé en argument
int main(int argc, char **argv)
{
  if (argc != 4)
  {
    printf("Erreur: Précisez un nom de fichier, suivi de ('name' ou 'id') puis du nom ou de l'id\n");
    return 1;
  }
  if (strcmp(argv[2], "nom") != 0 && strcmp(argv[2], "id") != 0)
  {
    printf("Erreur: Les options en seconde position sont 'name' et 'id'\n");
    return 1;
  }
  FILE *f;
  f = fopen(argv[1], "r");
  if (f == NULL)
  {
    printf("Erreur lors de l'ouverture en lecture du fichier\n");
    return 1;
  }

  // ATTENTION : pas fonctionnel pour le moment
  // En attente de modularisation
  Elf64_Ehdr header = Lecture(f);
  if (header.e_ident[EI_CLASS] == ELFCLASS32 && strcmp(argv[2], "name") == 0)
  {
    readsection_name_32(argv[3], f, header);
  }
  else if (header.e_ident[EI_CLASS] == ELFCLASS32 && strcmp(argv[2], "id") == 0)
  {
    readsection_id_32(atoi(argv[3]), f, header);
  }
  else if (header.e_ident[EI_CLASS] == ELFCLASS64 && strcmp(argv[2], "name") == 0)
  {
    readsection_name_64(argv[3], f, header);
  }
  else if (header.e_ident[EI_CLASS] == ELFCLASS64 && strcmp(argv[2], "id") == 0)
  {
    readsection_id_64(atoi(argv[3]), f, header);
  }
  return 0;
}