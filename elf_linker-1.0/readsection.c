#include <stdio.h>
#include <elf.h>
#include <string.h>
#include "readsection.h"

void readsection_id_64(int id, FILE *f, Elf64_Ehdr header)
{
  unsigned char temp;
  Elf64_Off offset;
  Elf64_Xword sh_size;
  unsigned char tab[100];
  Elf64_Word sh_name;
  int i;
  // On vérifie si l'id a un sens
  if (id >= header.e_shnum || id < 0)
  {
    printf(" Erreur: id out of bounds\n");
    return;
  }
  // On recupère l'index du nom de la section
  fseek(f, header.e_shoff + id * header.e_shentsize, SEEK_SET);
  fread(&sh_name, sizeof(Elf64_Word), 1, f);
  // On récupère le nom de la section
  fseek(f, header.e_shoff + header.e_shstrndx * header.e_shentsize + 2 * sizeof(Elf64_Word) + sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_SET);
  fread(&offset, sizeof(Elf64_Off), 1, f);
  fseek(f, offset + sh_name, SEEK_SET);
  int j = 0;
  fread(&tab[j], sizeof(unsigned char), 1, f);
  while (tab[j] != '\0')
  {
    j++;
    fread(&tab[j], sizeof(unsigned char), 1, f);
  }

  // On affiche le nom et l'id de la section
  printf("Hex dump of section %d named %s:\n", id, tab);

  // On récupère le offset et la taille pointant vers les données de la section
  fseek(f, header.e_shoff + id * header.e_shentsize + 2 * sizeof(Elf64_Word) + sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_SET);
  fread(&offset, sizeof(Elf64_Off), 1, f);
  fread(&sh_size, sizeof(Elf64_Xword), 1, f);
  // On se place au niveau de l'offset
  fseek(f, offset, SEEK_SET);

  // On lit le contenu de la section
  for (int i = 0; i < sh_size; i++)
  {
    if (i % 16 == 0)
      printf("  0x%08x ", offset + i);
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
  for (int i = 0; i < sh_size; i++)
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

void readsection_name_64(char *name, FILE *f, Elf64_Ehdr header)
{
  unsigned char tab[100];
  Elf64_Word sh_name;
  Elf64_Off offset;
  int i;
  // On teste les différents id pour voir si ils correspondent au nom
  for (i = 0; i < header.e_shnum; i = i + 1)
  {
    // On récupère l'index du nom
    fseek(f, header.e_shoff + i * header.e_shentsize, SEEK_SET);
    fread(&sh_name, sizeof(Elf64_Word), 1, f);

    // On récupère le nom de la section
    fseek(f, header.e_shoff + header.e_shstrndx * header.e_shentsize + 2 * sizeof(Elf64_Word) + sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_SET);
    fread(&offset, sizeof(Elf64_Off), 1, f);
    fseek(f, offset + sh_name, SEEK_SET);
    int j = 0;
    fread(&tab[j], sizeof(unsigned char), 1, f);
    while (tab[j] != '\0')
    {
      j++;
      fread(&tab[j], sizeof(unsigned char), 1, f);
    }

    // On vérifie si il correspond au nom en paramètre, et si c'est le cas on execute la fonction précédente avec l'id trouvé et on s'arrête
    if (strcmp(tab, name) == 0)
    {
      readsection_id_64(i, f, header);
      return;
    }
  }
  // Si aucune section n'est trouvée avec le nom, on le signale
  printf("Aucune section trouvée\n");
}

void readsection_id_32(int id, FILE *f, Elf32_Ehdr header)
{
  unsigned char temp;
  Elf32_Off offset;
  Elf32_Word sh_size;
  unsigned char tab[100];
  Elf32_Word sh_name;
  int i;
  // On vérifie si l'id a un sens
  if (id >= header.e_shnum || id < 0)
  {
    printf(" Erreur: id out of bounds\n");
    return;
  }
  // On récupère l'index du nom de la section
  fseek(f, header.e_shoff + id * header.e_shentsize, SEEK_SET);
  fread(&sh_name, sizeof(Elf32_Word), 1, f);
  // On récupère le nom de la section
  fseek(f, header.e_shoff + header.e_shstrndx * header.e_shentsize + 3 * sizeof(Elf32_Word) + sizeof(Elf32_Addr), SEEK_SET);
  fread(&offset, sizeof(Elf32_Off), 1, f);
  fseek(f, offset + sh_name, SEEK_SET);
  int j = 0;
  fread(&tab[j], sizeof(unsigned char), 1, f);
  while (tab[j] != '\0')
  {
    j++;
    fread(&tab[j], sizeof(unsigned char), 1, f);
  }

  // On affiche le nom et l'id de la section
  printf("Hex dump of section %d named %s:\n", id, tab);

  // On récupère le offset et la taille pointant vers les données de la section
  fseek(f, header.e_shoff + id * header.e_shentsize + 3 * sizeof(Elf32_Word) + sizeof(Elf32_Addr), SEEK_SET);
  fread(&offset, sizeof(Elf32_Off), 1, f);
  fread(&sh_size, sizeof(Elf32_Word), 1, f);
  // On se place au niveau de l'offset
  fseek(f, offset, SEEK_SET);

  // On lit le contenu de la section
  for (int i = 0; i < sh_size; i++)
  {
    if (i % 16 == 0)
      printf("  0x%08x ", offset + i);
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
  for (int i = 0; i < sh_size; i++)
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

void readsection_name_32(char *name, FILE *f, Elf32_Ehdr header)
{
  unsigned char tab[100];
  Elf32_Word sh_name;
  Elf32_Off offset;
  int i;
  // On teste les différents id pour voir si ils correspondent au nom
  for (i = 0; i < header.e_shnum; i = i + 1)
  {
    // On récupère l'index du nom
    fseek(f, header.e_shoff + i * header.e_shentsize, SEEK_SET);
    fread(&sh_name, sizeof(Elf32_Word), 1, f);

    // On récupère le nom de la section
    fseek(f, header.e_shoff + header.e_shstrndx * header.e_shentsize + 3 * sizeof(Elf32_Word) + sizeof(Elf32_Addr), SEEK_SET);
    fread(&offset, sizeof(Elf32_Off), 1, f);
    fseek(f, offset + sh_name, SEEK_SET);
    int j = 0;
    fread(&tab[j], sizeof(unsigned char), 1, f);
    while (tab[j] != '\0')
    {
      j++;
      fread(&tab[j], sizeof(unsigned char), 1, f);
    }

    // On vérifie si il correspond au nom en paramètre, et si c'est le cas on execute la fonction précédente avec l'id trouvé et on s'arrête
    if (strcmp(tab, name) == 0)
    {
      readsection_id_32(i, f, header);
      return;
    }
  }
  // Si aucune section n'est trouvée avec le nom, on le signale
  printf("Aucune section trouvée\n");
}
