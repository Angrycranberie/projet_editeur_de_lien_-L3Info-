#include <stdio.h>
#include <elf.h>
#include "header.h"

// Donne les informations sur le header d'un fichier ELF passé en argument.
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

    Elf64_Ehdr header = Lecture(f);
    affichage_header(header);
    return 0;
}