#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include "reloca.h"
#include "tablesection.h"


// Lit une table de reimplantation. Si addend = 1, c'est une table de type rela (avec addends), sinon c'est une table de type rel (sans addends). Pour le moment, on ne fait de toute façon rien avec addends.
rela_table_64 read_rela_tables_64(int id, FILE* f, Elf64_Ehdr header, int addend, section_list seclist){
  Elf64_Off	starttable;
  Elf64_Xword sizetotal;
  
  // version 32(0) ou 64 bit(1) 
  int bits_version = header.e_ident[EI_CLASS] == ELFCLASS64;
  // indique si l'endianess est diffÃ©rent entre la machine et le fichier 1 -> oui 0 -> non
  int diff_endianess = (is_big_endian() != (int)(header.e_ident[EI_DATA] == ELFDATA2MSB));
  
  // Table à renvoyer
  rela_table_64 tablerela;
  // On prend le type rela, que l'on traitera comme un type rel si il n'y a pas d'addend (on ne remplira juste pas ce champ)
  
  // On trouve le nom de la table
  strcpy(tablerela.name, seclist.names[id].nom);
  
  // On donne le type de section (rel ou rela) 
  tablerela.relorrela = addend;
  
  // on positionne l'index de la table
  tablerela.indextable = id;
  
  int i;
  
  // On se deplace sur le header de la section pour lire l'offset et la taille totale correspondante
  fseek(f, header.e_shoff + id * header.e_shentsize + 2 * sizeof(Elf64_Word) + sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_SET);
  
  // On lit les tables associees a la table de rel
  tablerela.tablesymbole = seclist.sec_list[id].sh_link;
  tablerela.tableinfo = seclist.sec_list[id].sh_info;
  
  // on lit la taille des elements
  tablerela.tailleentree = seclist.sec_list[id].sh_entsize;
  
  
  // On calcule le nombre d'entrees dans la table de reimplantation
  tablerela.nbentries = seclist.sec_list[id].sh_size/tablerela.tailleentree;
  // On initialise notre liste d'entrees
  tablerela.entries = malloc(sizeof(Elf64_Rela)*tablerela.nbentries);
  
  // On lit chacune des entrees
  for (i=0;i<tablerela.nbentries;i++){
    
    // On se place au bon emplacement dans le tableau
    fseek(f, seclist.sec_list[id].sh_offset + i * tablerela.tailleentree, SEEK_SET); 
    
    // On lit l'offset
    bits_version ? fread(&tablerela.entries[i].r_offset,sizeof(Elf64_Addr),1,f) : (fread(&tablerela.entries[i].r_offset,sizeof(Elf32_Addr),1,f), tablerela.entries[i].r_offset &= 0xFFFFFFFF);
    
    // On lit les informations (type et index du symbole)
    bits_version ? fread(&tablerela.entries[i].r_info,sizeof(Elf64_Xword),1,f) : (fread(&tablerela.entries[i].r_info,sizeof(Elf32_Word),1,f), tablerela.entries[i].r_info &= 0xFFFFFFFF);
    // On lit l'addend si necessaire. Sinon, il est positionne a l'emplacement du trou a remplir dans la section donnee par le champ sh_info
    if (addend==1) 
    {
      bits_version ? fread(&tablerela.entries[i].r_addend,sizeof(Elf64_Sxword),1,f) : (fread(&tablerela.entries[i].r_addend,sizeof(Elf32_Sword),1,f), tablerela.entries[i].r_addend &= 0xFFFFFFFF);
    }
    
    // On corrige l'endianess
    if(diff_endianess){
        tablerela.entries[i].r_offset = bits_version ? reverse_8(tablerela.entries[i].r_offset) : reverse_4(tablerela.entries[i].r_offset);
        tablerela.entries[i].r_info = bits_version ? reverse_8(tablerela.entries[i].r_info) : reverse_4(tablerela.entries[i].r_info);
        tablerela.entries[i].r_addend = bits_version ? reverse_8(tablerela.entries[i].r_addend) : reverse_4(tablerela.entries[i].r_addend);
    }
  
  }
  return tablerela;
}

// Cherche les tables de reimplantation dans le fichier elf et affiche leur contenu (version 64 bits)
list_rela_table_64 search_reloca_tables_64(FILE* f, Elf64_Ehdr header, section_list seclist){
  int i;
  list_rela_table_64 listrela;
  Elf64_Word type;
  
  // On initialise la liste des tables de rel
  listrela.nbtables = 0;
  listrela.list = NULL;
  
  // On teste les differents id pour voir si ce sont des tables de reimplantation
  for (i=0;i<seclist.nb_section;i=i+1){
  
    // On separe les cas avec ou sans addends
    if (seclist.sec_list[i].sh_type == SHT_RELA ) {
      listrela.list = realloc(listrela.list,sizeof(rela_table_64)*(listrela.nbtables+1));
      listrela.list[listrela.nbtables] = read_rela_tables_64(i,f,header,1,seclist);
      listrela.nbtables++;
    }
    else if (seclist.sec_list[i].sh_type == SHT_REL ){
      listrela.list = realloc(listrela.list,sizeof(rela_table_64)*(listrela.nbtables+1));
      listrela.list[listrela.nbtables] = read_rela_tables_64(i,f,header,0,seclist);
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







