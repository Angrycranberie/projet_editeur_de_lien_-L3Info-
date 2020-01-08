#include <stdio.h>
#include <elf.h>
#include "header.h"
#include "reloca.h"
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
  // etape 1 lecture du header
  Elf64_Ehdr header = read_header(f);
  //etape 2 lecture de la liste des sections
  section_list seclist = *read_tablesection(f,header);
  
  //etape 5 lecture des tables de rel
  list_rela_table_64 listrela;
  listrela = search_reloca_tables_64(f, header, seclist);
  // affichage du resultat
  print_list_rela_64(listrela);

  return 0;
}
