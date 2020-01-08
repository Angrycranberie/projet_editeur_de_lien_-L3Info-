#include <stdio.h>
#include <elf.h>
#include <string.h>
#include "readsection.h"
#include "tablesection.h"

void readsection_id(int id, FILE *f, section_list *sections){
    unsigned char temp;
    Elf64_Off offset = sections->sec_list[id].sh_offset;
    Elf64_Xword sh_size = (int)sections->sec_list[id].sh_size;
    int i;
    // On vérifie si l'id a un sens
    if (id >= sections->nb_section || id < 0)
    {
    	printf(" Erreur: id out of bounds\n");
      	return;
    }

	

    // On affiche le nom et l'id de la section
    printf("Hex dump of section %d named %s:\n", id, sections->names[id].nom);

    // On se place au niveau de l'offset
    fseek(f, offset, SEEK_SET);

    // On lit le contenu de la section
    for (i = 0; i < sh_size; i++)
    {
		if (i % 16 == 0)
			printf("  0x%08x ", i);
		fread(&temp, sizeof(unsigned char), 1, f);
		printf("%02x", temp);
		if (i % 4 == 3)
			printf(" ");
		if (i % 16 == 15)
			printf("\n");
    }
    printf("\n");

    // On rajoute un affichage en caractères (au cas ou, pour imiter la sortie de readelf)
    fseek(f, offset, SEEK_SET);
    printf("\nEn ascii cela donne :\n");
    for (i = 0; i < sh_size; i++)
    {
		if (i % 16 == 0)
			printf("  ");
		fread(&temp, sizeof(unsigned char), 1, f);
		if (temp >= 33 && temp <= 126)
			printf("%c", temp);
		else
			printf(".");
		if (i % 16 == 15)
			printf("\n");
    }
    printf("\n");
}

void readsection_name(char *name, FILE *f,section_list *sections){
	int i;
	// On teste les différents id pour voir si ils correspondent au nom
	for (i = 0; i < sections->nb_section; i = i + 1)
	{
		// On vérifie si il correspond au nom en paramètre, et si c'est le cas on execute la fonction précédente avec l'id trouvé et on s'arrête
		if (strcmp(name, sections->names[i].nom) == 0)
		{
			readsection_id(i, f, sections);
		return;
		}
	}
	// Si aucune section n'est trouvée avec le nom, on le signale
	printf("Aucune section trouvée\n");
}