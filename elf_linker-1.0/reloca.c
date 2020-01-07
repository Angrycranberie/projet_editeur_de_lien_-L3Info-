#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include "reloca.h"


// temporairement, il y a une fonction interessante la dedans
#include "progbits.h"

// Lit une table de reimplantation. Si addend = 1, c'est une table de type rela (avec addends), sinon c'est une table de type rel (sans addends). Pour le moment, on ne fait de toute façon rien avec addends.
rela_table_64 read_rela_tables_64(int id, FILE* f, Elf64_Ehdr header, int addend){
  Elf64_Off	starttable;
  Elf64_Xword sizetotal;
  
  // Table à renvoyer
  rela_table_64 tablerela;
  // On prend le type rela, que l'on traitera comme un type rel si il n'y a pas d'addend (on ne remplira juste pas ce champ)
  
  // On trouve le nom de la table
  getname_section_64(f, header, id, tablerela.name);
  
  // On donne le type de section (rel ou rela)
  tablerela.relorrela = addend;
  
  // on positionne l'index de la table
  tablerela.indextable = id;
  
  int i;
  
  // On se deplace sur le header de la section pour lire l'offset et la taille totale correspondante
  fseek(f, header.e_shoff + id * header.e_shentsize + 2 * sizeof(Elf64_Word) + sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_SET);
  fread(&starttable,sizeof(Elf64_Off),1,f);
  fread(&sizetotal,sizeof(Elf64_Xword),1,f);
  fread(&tablerela.tablesymbole,sizeof(Elf64_Word),1,f);
  
  // on se deplace un peu plus loin pour lire la taille des elements
  fseek(f, sizeof(Elf64_Word) + sizeof(Elf64_Xword), SEEK_CUR);
  fread(&tablerela.tailleentree,sizeof(Elf64_Xword),1,f);
  
  
  // On calcule le nombre d'entrees dans la table de reimplantation
  tablerela.nbentries = sizetotal/tablerela.tailleentree;
  // On initialise notre liste d'entrees
  tablerela.entries = malloc(sizeof(Elf64_Rela)*tablerela.nbentries);
  
  // On lit chacune des entrees
  for (i=0;i<tablerela.nbentries;i++){
    
    // On se place au bon emplacement dans le tableau
    fseek(f, starttable + i * tablerela.tailleentree, SEEK_SET); 
    
    // On lit l'offset
    fread(&tablerela.entries[i].r_offset,sizeof(Elf64_Addr),1,f);
    
    // On lit les informations (type et index du symbole)
    fread(&tablerela.entries[i].r_info,sizeof(Elf64_Xword),1,f);
    // On lit l'addend si necessaire
    if (addend==1) fread(&tablerela.entries[i].r_addend,sizeof(Elf64_Sxword),1,f);
  
  }
  return tablerela;
}

// Cherche les tables de reimplantation dans le fichier elf et affiche leur contenu (version 64 bits)
list_rela_table_64 search_reloca_tables_64(FILE* f, Elf64_Ehdr header){
  int i;
  list_rela_table_64 listrela;
  Elf64_Word type;
  
  // On initialise la liste des tables de rel
  listrela.nbtables = 0;
  listrela.list = NULL;
  
  // On teste les differents id pour voir si ce sont des tables de reimplantation
  for (i=0;i<header.e_shnum;i=i+1){
    fseek(f, header.e_shoff + i * header.e_shentsize + sizeof(Elf64_Word), SEEK_SET);
    
    // On vérifie si il y a un addend ou non
    fread(&type,sizeof(Elf64_Word),1,f);
    
    // On separe les cas avec ou sans addends
    if (type == SHT_RELA ) {
      listrela.list = realloc(listrela.list,sizeof(rela_table_64)*(listrela.nbtables+1));
      listrela.list[listrela.nbtables] = read_rela_tables_64(i,f,header,1);
      listrela.nbtables++;
    }
    else if (type == SHT_REL ){
      listrela.list = realloc(listrela.list,sizeof(rela_table_64)*(listrela.nbtables+1));
      listrela.list[listrela.nbtables] = read_rela_tables_64(i,f,header,0);
      listrela.nbtables++;
    }
  }
  return listrela;
}

void print_rela_64 (rela_table_64 tablerela){
  uint32_t type;
  uint32_t sym;
  int i;
 // On affiche le nom et le nombre d'entrees de la table
  printf("Relocation section \'%s\' contains %d entries:\n",tablerela.name, tablerela.nbentries);
  
  // On affiche le haut du tableau à afficher
  printf("  Offset          Info           Type           Sym. Index    Table de symbole concernee\n");

  // On lit chacune des entrees
  for (i=0;i<tablerela.nbentries;i++){
    
    // Afficher l'offset
    printf("%012x  ",tablerela.entries[i].r_offset);
    
    // Afficher info
    printf("%012lx ",tablerela.entries[i].r_info);
    
    // Afficher le type: Attention, on ne va donner ici que le numero lu, car les methodes s'ensuivant varient selon le processeur
    type = ELF64_R_TYPE(tablerela.entries[i].r_info);
    printf("%5d            ",type); 
    
    //Afficher l'index du symbole
    sym = ELF64_R_SYM(tablerela.entries[i].r_info);
    printf("%5d            ",sym); 
    
    // Afficher la table des symboles concernée
    printf("         %5d",tablerela.tablesymbole); 
    
    // On passe a l'entree suivante
    printf("\n");
    
  }
  printf("\n\n");
}

void print_list_rela_64 (list_rela_table_64 list){
  int i;
  for (i=0;i<list.nbtables;i++){
    print_rela_64(list.list[i]);
  }

}




// Versions 32 bits


// Lit une table de reimplantation. Si addend = 1, c'est une table de type rela (avec addends), sinon c'est une table de type rel (sans addends). Pour le moment, on ne fait de toute façon rien avec addends.
rela_table_32 read_rela_tables_32(int id, FILE* f, Elf32_Ehdr header, int addend){
  Elf32_Off	starttable;
  Elf32_Word sizetotal;
  
  // Table à renvoyer
  rela_table_32 tablerela;
  // On prend le type rela, que l'on traitera comme un type rel si il n'y a pas d'addend (on ne remplira juste pas ce champ)
  
  // On trouve le nom de la table
  getname_section_32(f, header, id, tablerela.name);
  
  // On donne le type de section (rel ou rela)
  tablerela.relorrela = addend;
  
  // on positionne l'index de la table
  tablerela.indextable = id;
  
  int i;
  
  // On se deplace sur le header de la section pour lire l'offset et la taille totale correspondante
  fseek(f, header.e_shoff + id * header.e_shentsize + 2 * sizeof(Elf32_Word) + sizeof(Elf32_Word) + sizeof(Elf32_Addr), SEEK_SET);
  fread(&starttable,sizeof(Elf32_Off),1,f);
  fread(&sizetotal,sizeof(Elf32_Word),1,f);
  fread(&tablerela.tablesymbole,sizeof(Elf32_Word),1,f);
  
  // on se deplace un peu plus loin pour lire la taille des elements
  fseek(f, sizeof(Elf32_Word) + sizeof(Elf32_Word), SEEK_CUR);
  fread(&tablerela.tailleentree,sizeof(Elf32_Word),1,f);
  
  
  // On calcule le nombre d'entrees dans la table de reimplantation
  tablerela.nbentries = sizetotal/tablerela.tailleentree;
  printf("\n");
  // On initialise notre liste d'entrees
  tablerela.entries = malloc(sizeof(Elf32_Rela)*tablerela.nbentries);
  
  // On lit chacune des entrees
  for (i=0;i<tablerela.nbentries;i++){
    
    // On se place au bon emplacement dans le tableau
    fseek(f, starttable + i * tablerela.tailleentree, SEEK_SET); 
    
    // On lit l'offset
    fread(&tablerela.entries[i].r_offset,sizeof(Elf32_Addr),1,f);
    
    // On lit les informations (type et index du symbole)
    fread(&tablerela.entries[i].r_info,sizeof(Elf32_Word),1,f);
    // On lit l'addend si necessaire
    if (addend==1) fread(&tablerela.entries[i].r_addend,sizeof(Elf32_Sword),1,f);
  
  }
  return tablerela;
}

// Cherche les tables de reimplantation dans le fichier elf et affiche leur contenu (version 32 bits)
list_rela_table_32 search_reloca_tables_32(FILE* f, Elf32_Ehdr header){
  int i;
  list_rela_table_32 listrela;
  Elf32_Word type;
  
  // On initialise la liste des tables de rel
  listrela.nbtables = 0;
  listrela.list = NULL;
  
  // On teste les differents id pour voir si ce sont des tables de reimplantation
  for (i=0;i<header.e_shnum;i=i+1){
    fseek(f, header.e_shoff + i * header.e_shentsize + sizeof(Elf32_Word), SEEK_SET);
    
    // On vérifie si il y a un addend ou non
    fread(&type,sizeof(Elf32_Word),1,f);
    
    // On separe les cas avec ou sans addends
    if (type == SHT_RELA ) {
      listrela.list = realloc(listrela.list,sizeof(rela_table_32)*(listrela.nbtables+1));
      listrela.list[listrela.nbtables] = read_rela_tables_32(i,f,header,1);
      listrela.nbtables++;
    }
    else if (type == SHT_REL ){
      listrela.list = realloc(listrela.list,sizeof(rela_table_32)*(listrela.nbtables+1));
      listrela.list[listrela.nbtables] = read_rela_tables_32(i,f,header,0);
      listrela.nbtables++;
    }
  }
  return listrela;
}

void print_rela_32 (rela_table_32 tablerela){
  uint32_t type;
  uint32_t sym;
  int i;
 // On affiche le nom et le nombre d'entrees de la table
  printf("Relocation section \'%s\' contains %d entries:\n",tablerela.name, tablerela.nbentries);
  
  // On affiche le haut du tableau à afficher
  printf("  Offset          Info           Type           Sym. Index    Table de symbole concernee\n");

  // On lit chacune des entrees
  for (i=0;i<tablerela.nbentries;i++){
    
    // Afficher l'offset
    printf("%012x  ",tablerela.entries[i].r_offset);
    
    // Afficher info
    printf("%012lx ",tablerela.entries[i].r_info);
    
    // Afficher le type: Attention, on ne va donner ici que le numero lu, car les methodes s'ensuivant varient selon le processeur
    type = ELF32_R_TYPE(tablerela.entries[i].r_info);
    printf("%5d            ",type); 
    
    //Afficher l'index du symbole
    sym = ELF32_R_SYM(tablerela.entries[i].r_info);
    printf("%5d            ",sym); 
    
    // Afficher la table des symboles concernée
    printf("         %5d",tablerela.tablesymbole); 
    
    // On passe a l'entree suivante
    printf("\n");
    
  }
  printf("\n\n");
}

void print_list_rela_32 (list_rela_table_32 list){
  int i;
  for (i=0;i<list.nbtables;i++){
    print_rela_32(list.list[i]);
  }

}







