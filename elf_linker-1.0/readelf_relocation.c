#include <stdio.h>
#include <elf.h>
#include "header.h"
#include "reloca.h"

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
  Elf64_Ehdr header = Lecture(f);
  if (header.e_ident[EI_CLASS] == ELFCLASS32)
  {
    search_reloca_tables_32(f, header);
  }
  else if (header.e_ident[EI_CLASS] == ELFCLASS64)
  {
    search_reloca_tables_64(f, header);
  }
  return 0;
}
