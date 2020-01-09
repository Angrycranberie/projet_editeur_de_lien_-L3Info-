#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include "progbits.h"
#include "tablesection.h"

// Fonction d'initialisation de la table des fusions (décrite dans 'progbits.h') avec des -1.
Merge_table_progbits initmerge(section_list *sections)
{
  Merge_table_progbits Mtable;
  Mtable.nbmerge = 0;
  int i;
  for (i = 0; i < sections->nb_section; i++) { Mtable.id_section_merge[i] = -1; }
  return Mtable;
}

// cherche une section qui porte le nom donné en argument et etant de type progbits
int search_section_name_progbits(char *nom,section_list *sections){
    int i;
    int id = -1;
    for(i =0 ; i < sections->nb_section ; i++){
        if(strcmp(sections->names[i].nom,nom) == 0 && sections->sec_list[i].sh_type == SHT_PROGBITS){
            id = i;
        }
    }
    return id;
}

/* Fonction de recherche des sections progbits dans le fichier 2.  
On regarde ensuite si il existe une section de même nom dans le fichier 1.
Renvoie le nombre de fusions effectuées, et desquelles il s'agit. */
Merge_table_progbits search_progbits_f2(section_list *sections1, section_list *sections2){
    int i;
    Elf64_Word type2;
    char nom1[NOMMAX];
    char nom2[NOMMAX];
    int id;

    // On crée la table.
    Merge_table_progbits Mtable = initmerge(sections2);

    // On parcourt les sections du fichier 2.
    for (i = 0 ; i < sections2->nb_section ; i = i + 1){
      
        // On verifie si la section actuelle du fichier 2 est de type progbits.
        type2 = sections2->sec_list[i].sh_type;
        // Si la section actuelle du fichier 2 est de type progbits, on cherche son nom.
        if (type2 == SHT_PROGBITS){
            
            strcpy(nom2,sections2->names[i].nom);
            printf("On trouve la section %s dans le fichier 2 qui est de type progbits\n", nom2);

            /* On regarde alors si son nom correspond à une section du fichier 1 de type progbits en parcourant les sections du fichier 1.
            'test' verifie si on a trouvé une section de même nom. */
            id = search_section_name_progbits((char *)nom2 , sections1);
            

            if (id == -1)
                printf("Il faut ajouter la section %s dans le numero %d\n", nom2, sections1->nb_section + i - Mtable.nbmerge);
            else{
                Mtable.id_section_merge[i] = id;
                Mtable.nbmerge++;
                strcpy(nom1,sections2->names[i].nom);
                printf("Il faut merge les sections %s dans le numero %d\n", nom1, id);
            }
        }
    }
    return Mtable;
}

/* Fonction vérifiant si une fusion (de progbits) est à faire sur cette section.
Retourne le numero avec lequel fusionner si il y a une fusion, -1 sinon. */
int verif_fusion_progbits(int id, Merge_table_progbits Mtable, section_list *sections){
    int i;
    for (i = 0; i < sections->nb_section; i++){
        if (Mtable.id_section_merge[i] == id)
            return i;
    }
    return -1;
}

/* Fonction initialisant la section en allouant la bonne taille (pour pouvoir y ranger toutes les donnees). */
Section_progbits init_section_size(int taille)
{
  Section_progbits Section;
  Section.taille = taille;
  Section.content = (unsigned char *)malloc(taille * sizeof(unsigned char *));

  // On initialise l'offset à 0, on le mettra à jour en cas de fusion.
  Section.offset = 0;
  return Section;
}

// Effectue la fusion des sections PROGBITS des deux fichiers
Table_sections get_merged_progbits(FILE *f1, FILE *f2, section_list *sections1, section_list *sections2, Merge_table_progbits Mtable)
{
  int i;
  int verif;
  Elf64_Xword size1;
  Elf64_Xword size2;
  Elf64_Word type;
  int index; // header1.e_shnum + header2.e_shnum - Mtable.nbmerge -2.

  // Structure permettant de stocker les informations utiles pour l'etape 6.
  Table_sections Tablesec;
  Tablesec.nbSections = 0;

  // On stocke Mtable (pour le retourner avec le reste) dans Tablesec.
  Tablesec.Mtable = Mtable;

  // Stocke les offsets des entrees de la table de sections dans leurs fichiers respectifs.
  Elf64_Off offset1;
  Elf64_Off offset2;

  // On commence par les progbits du fichier 1, on saute donc le 0 qui n'en est jamais 1.
  for (i = 1; i < sections1->nb_section; i++)
  {
    // On recupere le nom de la section.
    strcpy(Tablesec.sections[Tablesec.nbSections].name,sections1->names[i].nom);
    type = sections1->sec_list[i].sh_type;

    if (type == SHT_PROGBITS)
    {
      // On ne récupère que les sections PROGBITS. On saute les champs qui ne nous interessent pas.
      offset1 = sections1->sec_list[i].sh_offset;
      size1 = sections1->sec_list[i].sh_size;

      // On vérifie si on a une fusion à faire.
      verif = verif_fusion_progbits(i, Mtable, sections2);
      // Cas de la fusion
      if (verif != -1)
      {
        Tablesec.fusion[Tablesec.nbSections] = 1;
        
        // On lit le size pour le rajouter (l'offset est gardé pour plus tard, voir l'impression du contenu).
        offset2 = sections2->sec_list[verif].sh_offset;
        size2 = sections2->sec_list[verif].sh_size;
      }
      // On note sinon l'absence de fusion.
      else
        Tablesec.fusion[Tablesec.nbSections] = 0;

      // On met à jour notre structure avec les informations apportées.
      Tablesec.index_section[Tablesec.nbSections] = i;
      if (verif == -1)
      {
        Tablesec.sections[Tablesec.nbSections] = init_section_size(size1);
      }
      else
      {
        Tablesec.sections[Tablesec.nbSections] = init_section_size(size1 + size2);
        Tablesec.sections[Tablesec.nbSections].offset = size1;
      }
      strcpy(Tablesec.sections[Tablesec.nbSections].name,sections1->names[i].nom);
      Tablesec.sections[Tablesec.nbSections].oldindexfich2 = -1;

      // On imprime maintenant le contenu des sections ; on commence par se placer correctement dans le fichier 1.
      fseek(f1, offset1, SEEK_SET);

      // On écrit le contenu dans la structure.
      fread(Tablesec.sections[Tablesec.nbSections].content, sizeof(unsigned char), size1, f1);

      // Si on fait une fusion, on concatene la section contenue dans le fichier 2.
      if (verif != -1)
      {
        fseek(f2, offset2, SEEK_SET);
        fread(Tablesec.sections[Tablesec.nbSections].content + Tablesec.sections[Tablesec.nbSections].offset, sizeof(unsigned char), size2, f2);
      }

      // On incremente la taille du tableau des sections progbits.
      Tablesec.nbSections++;
    }
  }

  // On doit maintenant rajouter les sections de la partie 2.
  // On maintient à jour l'index du prochain element a rajouter.
  index = sections1->nb_section;
  // On ignore le 0, qui est vide.
  for (i = 1; i < sections2->nb_section; i++)
  {
    if (Mtable.id_section_merge[i] == -1)
    {
        // On se place puis on recupere les donnees du header de la fonction.
        strcpy(Tablesec.sections[Tablesec.nbSections].name,sections2->names[i].nom);
        type = sections2->sec_list[i].sh_type;

      // On ne la rajoute que si elle est du type PROGBITS.
      if (type == SHT_PROGBITS)
      {
        // On saute les champs qui ne nous interessent pas.
        offset2 = sections2->sec_list[i].sh_offset;
        size2 = sections2->sec_list[i].sh_size;

        // On met à jour notre structure avec les informations apportées.
        Tablesec.index_section[Tablesec.nbSections] = index;
        Tablesec.fusion[Tablesec.nbSections] = 0;
        Tablesec.sections[Tablesec.nbSections] = init_section_size(size2);
        strcpy(Tablesec.sections[Tablesec.nbSections].name,sections2->names[i].nom);
        Tablesec.sections[Tablesec.nbSections].oldindexfich2 = i;

        // On imprime maintenant le contenu de la section ; on commence par se placer correctement dans le fichier 2.
        fseek(f2, offset2, SEEK_SET);

        // On écrit le contenu de la section dans la structure.
        fread(Tablesec.sections[Tablesec.nbSections].content, sizeof(unsigned char), size2, f2);

        // On incrémente l'index.
        index++;
        Tablesec.nbSections++;
      }
    }
  }
  return Tablesec;
}

void affiche_table_section(Table_sections Tablesec)
{
  unsigned char temp;

  printf("Liste des sections de type PROGBITS dans le fichier final :\n");
  // On affiche chacune des sections individuellement.
  int i;
  for (i = 0; i < Tablesec.nbSections; i++)
  {
    // Permet de reconnaitre si il y a eu fusion.
    if (Tablesec.fusion[i] == 1)
    {
      // Cas de la fusion.
      printf("  On a fusionne les deux parties nommees %s, a l'index %d\n", Tablesec.sections[i].name, Tablesec.index_section[i]);
      printf("  La taille de la section est %016x\n  L'offset ou on trouve les elements du fichier2 est %016x\n", Tablesec.sections[i].taille, Tablesec.sections[i].offset);
    }
    else
    {
      printf("  La partie %s est stockee a l'index %d\n", Tablesec.sections[i].name, Tablesec.index_section[i]);
      printf("  La taille de la section est %016x\n", Tablesec.sections[i].taille);
    }

    printf("  Contenu hexadecimal de la section :\n");
    int j;
    for (j = 0; j < Tablesec.sections[i].taille; j++)
    {
      if (j % 16 == 0)
        printf("    ");
      temp = Tablesec.sections[i].content[j];
      printf("%02x", temp);
      if (j % 4 == 3)
        printf(" ");
      if (j % 16 == 15)
        printf("\n");
    }
    printf("\n");
  }
}
