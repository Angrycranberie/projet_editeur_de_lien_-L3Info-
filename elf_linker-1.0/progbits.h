#ifndef _PROGBITS_H
#define	_PROGBITS_H 1
#include <elf.h>
#include "values.h"

// Structure permettant de se souvenir des sections que l'on doit merger. si id_section_merge[j] = i, cela veut dire que la section j du fichier 2 doit etre concatennee a la section i du fichier 1. si cela vaut -1, cela signifie 
// qu'il ne faut pas la merge.
typedef struct
{
  int nbmerge; // Contient le nombre de fusion à effectuer
  int id_section_merge[TMAX];
} Merge_table_progbits;

typedef struct
{
  int taille; // Taille de la section
  int offset; // Offset du debut de la section qui a été concaténée, si il y en a une, sinon la valeur n'a pas d'importance
  char name[NOMMAX]; // Retiens le nom de la section
  unsigned char* content; // Contenu octet par octet de la section
  int oldindexfich2 // Permet de connaitre l'ancien index de la section dans le fichier 2. Si la section est fusionnee ou fait partie du fichier 1, cela vaut -1.
} Section_progbits;


// Remarque importante: Cette table de section ne contient que les sections de type PROGBITS.
typedef struct 
{
  int nbSections; // Nombre de sections (après fusion, avec l'usage que l'on en fait)
  Section_progbits sections[TMAX]; // Tableau de sections (définis ci-dessus)
  int index_section[TMAX]; // Donne l'index des sections d'indice correspondant dans le tableau sections
  int fusion [TMAX]; // Indique si une fusion à eu lieu : O pour pas de fusion, 1 pour fusion pour chaque section
  Merge_table_progbits Mtable; // On garde en mémoire le tableau nous indiquant ce que l'on a fusionné
} Table_sections;

Section_progbits init_section_size(int taille); // initialise la taille de la partie content d'une section pour qu'elle soit de taille octets.
void affiche_table_section(Table_sections Tablesec); // Affiche la table des sections données en entrée, en donnant pour chacune:
  // - Leur nom
  // - Si elles sont issues d'une fusion
  // - La taille de leurs données
  // - Si elles sont issues d'une fusion, l'offset où on trouve les données de la seconde section fusionnée
  // - Le contenu en hexadecimal de la section

// Versions 32 bits
void getname_section_32(FILE* f, Elf32_Ehdr header, int id, char* nom); // Met dans la variable char le nom de la section d'index id, contenu dans le fichier elf decrit par f, et dont le header est déjà conteu dans header. 
Merge_table_progbits initmerge_32(Elf32_Ehdr header2); // Initialise la table de fusion, en plaçant des -1 (pas de fusion) à chaque index correspondant à une section du fichier 2, le nombre étant contenu dans header2.
Merge_table_progbits search_progbits_f2_32(FILE* f1, FILE* f2, Elf32_Ehdr header1, Elf32_Ehdr header2); // Remplie la table des fusions:
  // Cherche les sections progbits du fichier 1
  // Vérifie si il y a une table progbits de même nom dans le fichier 2
  // Si oui, on ajoute cette fusion dans la table des fusions que l'on renvoie à la fin.
  // Ceci, jusqu'à avoir épuisé les sections du fichier1.
  
int verif_fusion_progbits_32 (int id, Merge_table_progbits Mtable, Elf32_Ehdr header); // Vérifie si il faut fusionner la section d'index id du fichier 1
  // Renvoie -1 si la réponse est non
  // Renvoie l'index dans le fichier 2 de la section avec laquelle il doit fusionner sinon
Table_sections get_merged_progbits_32 (FILE* f1, FILE* f2, Elf32_Ehdr header1, Elf32_Ehdr header2, Merge_table_progbits Mtable); // Crée une nouvelle table des sections en fusionnant comme décrit dans la table de fusion Mtable.
  // Renvoie une table de sections correspodnant à la description ci-dessus
  // La fusion de deux section s'effectue par concatenation, la partie des données correspondant au fichier2 est concaténée à la suite de celles correspondant au fichier1.
  // On garde les indices du fichier1, et les sections du fichier2 que l'on ne fusionne pas sont ajoutées à la suite

// Versions 64 bits (description idem que pour la partie ci-dessus)
void getname_section_64(FILE* f, Elf64_Ehdr header, int id, char* nom);
Merge_table_progbits initmerge_64(Elf64_Ehdr header2);
Merge_table_progbits search_progbits_f2_64(FILE* f1, FILE* f2, Elf64_Ehdr header1, Elf64_Ehdr header2);
int verif_fusion_progbits_64 (int id, Merge_table_progbits Mtable, Elf64_Ehdr header);

Table_sections get_merged_progbits_64 (FILE* f1, FILE* f2, Elf64_Ehdr header1, Elf64_Ehdr header2, Merge_table_progbits Mtable);


#endif











