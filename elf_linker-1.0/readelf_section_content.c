#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include "header.h"
#include "readsection.h"
#include "tablesection.h"

// Donne les informations sur les sections du fichier ELF passé en argument
int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Erreur: Précisez un nom de fichier, suivi de ('name' ou 'id') puis du nom ou de l'id\n");
        return 1;
    }
    if (strcmp(argv[2], "name") != 0 && strcmp(argv[2], "id") != 0)
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

    Elf64_Ehdr header = read_header(f);
    section_list *sections = read_tablesection(f,header);
    if (strcmp(argv[2], "id") == 0){
        readsection_id(atoi(argv[3]), f, sections);
    }
    else if (strcmp(argv[2], "name") == 0){
        readsection_name(argv[3],f,sections);
    }
    return 0;
}