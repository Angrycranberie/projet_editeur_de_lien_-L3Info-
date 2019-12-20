#include <stdio.h>
#include <elf.h>
#include "header.h"
#include "tablesection.h"

// Donne les informations sur les sections du fichier ELF passé en argument
int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Erreur: Précisez un nom de fichier, et un seul\n");
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
  Elf64_Ehdr header = read_header(f);
  if (header.e_ident[EI_CLASS] == ELFCLASS32)
  {
    Tablesection32(f, header);
  }
  else if (header.e_ident[EI_CLASS] == ELFCLASS64)
  {
    Tablesection64(f, header);
  }
  return 0;
}