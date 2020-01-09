#ifndef _PROGBITS_H
#define _PROGBITS_H 1
#include <elf.h>
#include "values.h"
#include "tablesection.h"


/* Structure permettant de se souvenir des sections que l'on doit fusionner.
- Si id_section_merge[j] = i, cela veut dire que la section j du fichier 2 doit etre concatennée à la section i du fichier 1,
- Si cela vaut -1, cela signifie qu'il ne faut pas la fusionner. */
typedef struct
{
  int nbmerge; // Contient le nombre de fusion à effectuer.
  int id_section_merge[TMAX];
} Merge_table_progbits;

typedef struct
{
  int taille;             // Taille de la section.
  int offset;             // Offset du debut de la section qui a été concaténée, si il y en a une, sinon la valeur n'a pas d'importance.
  char name[NOMMAX];      // Retient le nom de la section.
  unsigned char *content; // Contenu octet par octet de la section.
  int oldindexfich2;       // Permet de connaitre l'ancien index de la section dans le fichier 2. Si la section est fusionnée ou fait partie du fichier 1, cela vaut -1.
} Section_progbits;

// Remarque importante : cette table des sections ne contient que les sections de type PROGBITS.
typedef struct
{
  int nbSections;                  // Nombre de sections (après fusion, avec l'usage que l'on en fait).
  Section_progbits sections[TMAX]; // Tableau de sections (défini ci-dessus).
  int index_section[TMAX];         // Donne l'index des sections d'indice correspondant dans le tableau sections.
  int fusion[TMAX];                // Indique si une fusion a eu lieu : O pour pas de fusion, 1 pour fusion pour chaque section.
  Merge_table_progbits Mtable;     // On garde en mémoire le tableau nous indiquant ce que l'on a fusionné.
} Table_sections;


// Initialise la taille de la partie 'content' d'une section pour qu'elle soit de taille octets.
Section_progbits init_section_size(int taille);

/* Fonction d'affichage de la table des sections données en entrée, en donnant pour chacune :
- Leur nom,
- Si elles sont issues d'une fusion,
- La taille de leurs données,
- Si elles sont issues d'une fusion, l'offset où on trouve les données de la seconde section fusionnée,
- Le contenu en hexadecimal de la section. */
void affiche_table_section(Table_sections Tablesec);


// Fonction d'initialisation de la table de fusion, plaçant des -1 (pas de fusion) à chaque index correspondant à une section du fichier 2, le nombre étant contenu dans sections->nb_section.
Merge_table_progbits initmerge(section_list *sections);

// Cherche si il existe une section du nom donné en argument de type PROGBYTE, donne la dernière respectant ces conditions
int search_section_name_progbits(char *nom,section_list *sections);

/* Fonction de remplissage de la table des fusions :
- Cherche les sections progbits du fichier 1,
- Vérifie si il y a une table progbits de même nom dans le fichier 2 ; si oui, on ajoute cette fusion dans
la table des fusions que l'on renvoie à la fin, ceci, jusqu'à avoir épuisé les sections du fichier1. */
Merge_table_progbits search_progbits_f2(section_list *sections1, section_list *sections2);

/* Fonction vérifiant si il faut fusionner la section d'index id du fichier 1.
- Renvoie -1 si la réponse est non,
- Renvoie l'index dans le fichier 2 de la section avec laquelle il doit fusionner sinon. */
int verif_fusion_progbits(int id, Merge_table_progbits Mtable, section_list *sections);

/* Fonction créant une nouvelle table des sections en fusionnant comme décrit dans la table de fusion Mtable.
La fusion de deux sections s'effectue par concaténation, la partie des données correspondant au fichier 2 est concaténée à la suite de celles correspondant au fichier1.
On garde les indices du fichier 1, et les sections du fichier 2 que l'on ne fusionne pas sont ajoutées à la suite.
Renvoie une table de sections correspondant à la description ci-dessus. */
Table_sections get_merged_progbits(FILE *f1, FILE *f2, section_list *sections1, section_list *sections2, Merge_table_progbits Mtable);

#endif
