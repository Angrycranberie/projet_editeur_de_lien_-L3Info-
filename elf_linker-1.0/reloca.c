#include <stdio.h>
#include <elf.h>
#include <string.h>
#include "reloca.h"

// Lit une table de reimplantation. Si addend = 1, c'est une table de type rela (avec addends), sinon c'est une table de type rel (sans addends). Pour le moment, on ne fait de toute façon rien avec addends.
void read_rela_tables_64(int id, FILE* f, Elf64_Ehdr header, int addend){
  Elf64_Off	offset;
  Elf64_Off	starttable;
  Elf64_Xword	entsize;
  Elf64_Xword sizetotal;
  Elf64_Word sh_name;
  unsigned char tab[100];
  uint32_t type;
  uint32_t sym;
  Elf64_Word linksym;
  // On prend le type rela, que l'on traitera comme un type rel si il n'y a pas d'addend (on ne remplira juste pas ce champ)
  Elf64_Rela rela_entry;
  
  
  int nb;
  int i;
  
  // On se deplace sur le header de la section pour lire l'offset et la taille totale correspondante
  fseek(f, header.e_shoff + id * header.e_shentsize + 2 * sizeof(Elf64_Word) + sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_SET);
  fread(&starttable,sizeof(Elf64_Off),1,f);
  fread(&sizetotal,sizeof(Elf64_Xword),1,f);
  fread(&linksym,sizeof(Elf64_Word),1,f);
  
  // on se deplace un peu plus loin pour lire la taille des elements
  fseek(f, sizeof(Elf64_Word) + sizeof(Elf64_Xword), SEEK_CUR);
  fread(&entsize,sizeof(Elf64_Xword),1,f);
  
  // On calcule le nombre d'entrees dans la table de reimplantation
  nb = sizetotal/entsize;
  // On recupère l'id du nom de la section
  fseek(f, header.e_shoff + id * header.e_shentsize , SEEK_SET);
  fread(&sh_name,sizeof(Elf64_Word),1,f);
  // On recupère le nom de la section
  fseek(f, header.e_shoff + header.e_shstrndx * header.e_shentsize + 2 * sizeof(Elf64_Word) + sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_SET);
  fread(&offset,sizeof(Elf64_Off),1,f);
  fseek(f, offset +  sh_name, SEEK_SET);  
  int j=0;
  fread(&tab[j],sizeof(unsigned char),1,f);
  while(tab[j]!='\0'){
    j++;
  fread(&tab[j],sizeof(unsigned char),1,f);
    }
  // On affiche le nom et le nombre d'entrees de la table
  printf("Relocation section \'%s\' at offset 0x%x contains %d entries:\n",tab ,starttable, nb);
  
  // On affiche le haut du tableau à afficher
  printf("  Offset          Info           Type           Sym. Index    Table de symbole concernee\n");

  // On lit chacune des entrees
  for (i=0;i<nb;i++){
    
    // On se place au bon emplacement dans le tableau
    fseek(f, starttable + i * entsize, SEEK_SET); 
    
    // On lit l'offset
    fread(&rela_entry.r_offset,sizeof(Elf64_Addr),1,f);
    
    // On lit les informations (type et index du symbole)
    fread(&rela_entry.r_info,sizeof(Elf64_Xword),1,f);
    
    // On lit l'addend si necessaire
    if (addend==1) fread(&rela_entry.r_addend,sizeof(Elf64_Sxword),1,f);
    
    // On affiche ce que l'on a lu
    
    // Afficher l'offset
    printf("%012x  ",rela_entry.r_offset);
    
    // Afficher info
    printf("%012x ",rela_entry.r_info);
    
    // Afficher le type: Attention, on ne va donner ici que le numero lu, car les methodes s'ensuivant varient selon le processeur
    type = ELF64_R_TYPE(rela_entry.r_info);
    printf("%5d            ",type); 
    
    //Afficher l'index du symbole
    sym = ELF64_R_SYM(rela_entry.r_info);
    printf("%5d            ",sym); 
    
    // Afficher la table des symboles concernée
    printf("         %5d",linksym); 
    
    // On passe a l'entree suivante
    printf("\n");
    
    // La partie commentee, si elle était complete, donnerait une sortie semblable à readelf -r, mais cela n'est pas demande pour l'etape 5 (pas de ...).
    
    /*
    // Afficher la suite (on doit aller voir le symbole)
    sym = ELF64_R_SYM(rela_entry.r_info);
    fseek(f, header.e_shoff + linkstring * header.e_shentsize + 2 * sizeof(Elf64_Word) + sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_SET);
    fread(&offset,sizeof(Elf64_Off),1,f);
    fseek(f, offset +  sym + sizeof(Elf64_Word) , SEEK_SET);
    Elf64_Addr symbol_value
    
    // Recuperation des infos
    fread(&symbole.st_info,sizeof(unsigned char),1,f);
    
    // Recuperation de la partie other
    fread(&symbole.st_other,sizeof(unsigned char),1,f);
    
    // Recuperation de l'indexe de la section concernee
    fread(&symbole.st_shndx,sizeof(Elf64_Section),1,f);
    */
  
  }
}

// Cherche les tables de reimplantation dans le fichier elf et affiche leur contenu (version 64 bits)
void search_reloca_tables_64(FILE* f, Elf64_Ehdr header){
  int i;
  Elf64_Word type;
  // On teste les differents id pour voir si ce sont des tables de reimplantation
  for (i=0;i<header.e_shnum;i=i+1){
    fseek(f, header.e_shoff + i * header.e_shentsize + sizeof(Elf64_Word), SEEK_SET);
    fread(&type,sizeof(Elf64_Word),1,f);
    
    // On separe les cas avec ou sans addends
    if (type == SHT_RELA ) read_rela_tables_64(i,f,header,1);
    else if (type == SHT_REL ) read_rela_tables_64(i,f,header,0);
  }
}
