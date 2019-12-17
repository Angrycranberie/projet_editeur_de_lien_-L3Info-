#include <stdio.h>
#include <elf.h>
#include <string.h>
#include "symboltable.h"

// Lis le contenu de la table des symboles visée (version 32 bits)
void read_symbols_tables_32 (int id, FILE* f, Elf32_Ehdr header){
  Elf32_Off	offset;
  Elf32_Off	starttable;
  Elf32_Word	entsize;
  Elf32_Word sizetotal;
  Elf32_Word sh_name;
  Elf32_Word linkstring;
  unsigned char tab[100];
  Elf32_Sym symbole;
  unsigned char type;
  unsigned char bind;
  unsigned char visibility;
  
  int nb;
  int i;
  
  // On se déplace sur le header de la section pour lire l'offset, la taille totale et l'indexe de la table de chaine correspondante
  fseek(f, header.e_shoff + id * header.e_shentsize + 2 * sizeof(Elf32_Word) + sizeof(Elf32_Word) + sizeof(Elf32_Addr), SEEK_SET);
  fread(&starttable,sizeof(Elf32_Off),1,f);
  fread(&sizetotal,sizeof(Elf32_Word),1,f);
  fread(&linkstring,sizeof(Elf32_Word),1,f);
  
  // on se déplace un peu plus loin pour lire la taille des éléments
  fseek(f, sizeof(Elf32_Word) + sizeof(Elf32_Word), SEEK_CUR);
  fread(&entsize,sizeof(Elf32_Word),1,f);
  
  // On calcule le nombre d'entrees dans la table des symboles
  nb = sizetotal/entsize;
  
  // On récupère l'id du nom de la section
  fseek(f, header.e_shoff + id * header.e_shentsize , SEEK_SET);
  fread(&sh_name,sizeof(Elf32_Word),1,f);
  // On récupère le nom de la section
  fseek(f, header.e_shoff + header.e_shstrndx * header.e_shentsize + 2 * sizeof(Elf32_Word) + sizeof(Elf32_Word) + sizeof(Elf32_Addr), SEEK_SET);
  fread(&offset,sizeof(Elf32_Off),1,f);
  fseek(f, offset +  sh_name, SEEK_SET);  
  int j=0;
  fread(&tab[j],sizeof(unsigned char),1,f);
  while(tab[j]!='\0'){
    j++;
  fread(&tab[j],sizeof(unsigned char),1,f);
    }
    
  // On affiche le nom et le nombre d'entrees de la table
  printf("Symbol table \'%s\' contains %d entries:\n",tab, nb);
  
  // On affiche le haut du tableau à afficher
  printf("   Num:    Value          Size Type    Bind   Vis      Ndx Name\n");
  
  
   
  
  // On affiche les élements de la tablede symboles
  for (i=0;i<nb;i++){
    //On se place au bon emplacement de la table
    fseek(f, starttable + i * entsize, SEEK_SET); 
    // On recupere les parties de la structure symbole
    
    // Recuperation de l'indexe du nom dans la table de chaine associée à la table de symbole
    fread(&symbole.st_name,sizeof(Elf32_Word),1,f);
    
    // Recuperation de la valeur (addresse)
    fread(&symbole.st_value,sizeof(Elf32_Addr),1,f);
    
    // Recuperation de la taille du symbole
    fread(&symbole.st_size,sizeof(Elf32_Word),1,f);
    
    // Recuperation des infos
    fread(&symbole.st_info,sizeof(unsigned char),1,f);
    
    // Recuperation de la partie other
    fread(&symbole.st_other,sizeof(unsigned char),1,f);
    
    // Recuperation de l'indexe de la section concernée
    fread(&symbole.st_shndx,sizeof(Elf32_Section),1,f);
    
    // Recuperation du nom (de la même façon qu'auparavant)
    fseek(f, header.e_shoff + linkstring * header.e_shentsize + 2 * sizeof(Elf32_Word) + sizeof(Elf32_Word) + sizeof(Elf32_Addr), SEEK_SET);
    fread(&offset,sizeof(Elf32_Off),1,f);
    fseek(f, offset +  symbole.st_name, SEEK_SET);  
    j=0;
    fread(&tab[j],sizeof(unsigned char),1,f);
    while(tab[j]!='\0'){
      j++;
    fread(&tab[j],sizeof(unsigned char),1,f);
    }
    
    // On affiche le tout:
    
    // Numero
    printf("  %4d: ",i);
    
    // valeur
    printf("%016x ",symbole.st_value);
    
    // Taille
    printf("%5d ",symbole.st_size);
    
    // Type
    type = (symbole.st_info & 0xf);
    switch (type){
    
    case STT_NOTYPE:
    printf("NOTYPE  ");
    break;
    
    case STT_OBJECT:
    printf("OBJECT  ");
    break;
    
    case STT_FUNC:
    printf("FUNC    ");
    break;
    
    case STT_SECTION:
    printf("SECTION ");
    break;
    
    case STT_FILE:
    printf("FILE    ");
    break;
    
    case STT_COMMON:
    printf("COMMON  ");
    break;
    
    case STT_TLS:
    printf("TLS     ");
    break;
    
    case STT_NUM:
    printf("NUM     ");
    break;
    
    case STT_GNU_IFUNC:
    printf("IFUNC   ");
    break;
    
    
    default:
    printf("???     ");
    break;
    
    }
    
    // Binding
    bind = (symbole.st_info >> 4);
    switch (bind){
    
    case STB_LOCAL:
    printf("LOCAL  ");
    break;
    
    case STB_GLOBAL:
    printf("GLOBAL ");
    break;
    
    case STB_WEAK:
    printf("WEAK   ");
    break;
    
    case STB_NUM:
    printf("NUM    ");
    break;
    
    case STB_GNU_UNIQUE:
    printf("UNIQUE ");
    break;

    
    default:
    printf("???    ");
    break;
    
    }
    // Visibility (pas demandé dans le sujet, mais donné par readelf) 
    // cela viens de la section other, qui n'était pas utilisée dans la documentation, mais qui stocke
    // la visibilité du symbole ici.
    visibility = symbole.st_other & (0x03);
    switch (visibility){
    
    case STV_DEFAULT:
    printf("DEFAULT   ");
    break;
    
    case STV_INTERNAL:
    printf("INTERNAL  ");
    break;
    
    case STV_HIDDEN:
    printf("HIDDEN    ");
    break;
    
    case STV_PROTECTED:
    printf("PROTECTED ");
    break;
    
    default:
    printf("???       ");
    break;
    
    }
    
    // Section concernée
    switch(symbole.st_shndx){
    
    case (SHN_UNDEF):
    printf("UND ");
    break;
    
    case (SHN_ABS):
    printf("ABS ");
    break;
    
    case (SHN_COMMON):
    printf("COM ");
    break;
    
    default:
    printf("%3d ",symbole.st_shndx);
    
    }
    
    // Nom et fin de ligne
    printf("%s\n",tab);
  }

}

// Cherche les tables de symboles dans le fichier elf et affiche leur contenu (version 32 bits)
void search_symbols_tables_32 (FILE* f, Elf32_Ehdr header){
  int i;
  Elf32_Word type;
  // On teste les différents id pour voir si ce sont des tables de symboles
  for (i=0;i<header.e_shnum;i=i+1){
    fseek(f, header.e_shoff + i * header.e_shentsize + sizeof(Elf32_Word), SEEK_SET);
    fread(&type,sizeof(Elf32_Word),1,f);
    if (type == SHT_SYMTAB || type == SHT_DYNSYM) read_symbols_tables_32(i,f,header);
  }
}


// Lis le contenu de la table des symboles visée (version 64 bits)
void read_symbols_tables_64 (int id, FILE* f, Elf64_Ehdr header){
  Elf64_Off	offset;
  Elf64_Off	starttable;
  Elf64_Xword	entsize;
  Elf64_Xword sizetotal;
  Elf64_Word sh_name;
  Elf64_Word linkstring;
  unsigned char tab[100];
  Elf64_Sym symbole;
  unsigned char type;
  unsigned char bind;
  unsigned char visibility;
  
  int nb;
  int i;
  
  // On se déplace sur le header de la section pour lire l'offset, la taille totale et l'indexe de la table de chaine correspondante
  fseek(f, header.e_shoff + id * header.e_shentsize + 2 * sizeof(Elf64_Word) + sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_SET);
  fread(&starttable,sizeof(Elf64_Off),1,f);
  fread(&sizetotal,sizeof(Elf64_Xword),1,f);
  fread(&linkstring,sizeof(Elf64_Word),1,f);
  
  // on se déplace un peu plus loin pour lire la taille des éléments
  fseek(f, sizeof(Elf64_Word) + sizeof(Elf64_Xword), SEEK_CUR);
  fread(&entsize,sizeof(Elf64_Xword),1,f);
  
  // On calcule le nombre d'entrees dans la table des symboles
  nb = sizetotal/entsize;
  
  // On récupère l'id du nom de la section
  fseek(f, header.e_shoff + id * header.e_shentsize , SEEK_SET);
  fread(&sh_name,sizeof(Elf64_Word),1,f);
  // On récupère le nom de la section
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
  printf("Symbol table \'%s\' contains %d entries:\n",tab, nb);
  
  // On affiche le haut du tableau à afficher
  printf("   Num:    Value          Size Type    Bind   Vis      Ndx Name\n");
  
  
   
  
  // On affiche les élements de la tablede symboles
  for (i=0;i<nb;i++){
    //On se place au bon emplacement de la table
    fseek(f, starttable + i * entsize, SEEK_SET); 
    // On recupere les parties de la structure symbole
    
    // Recuperation de l'indexe du nom dans la table de chaine associée à la table de symbole
    fread(&symbole.st_name,sizeof(Elf64_Word),1,f);
    
    // Recuperation des infos
    fread(&symbole.st_info,sizeof(unsigned char),1,f);
    
    // Recuperation de la partie other
    fread(&symbole.st_other,sizeof(unsigned char),1,f);
    
    // Recuperation de l'indexe de la section concernée
    fread(&symbole.st_shndx,sizeof(Elf64_Section),1,f);
    
    // Recuperation de la valeur (addresse)
    fread(&symbole.st_value,sizeof(Elf64_Addr),1,f);
    
    // Recuperation de la taille du symbole
    fread(&symbole.st_size,sizeof(Elf64_Xword),1,f);
    
    // Recuperation du nom (de la même façon qu'auparavant)
    fseek(f, header.e_shoff + linkstring * header.e_shentsize + 2 * sizeof(Elf64_Word) + sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_SET);
    fread(&offset,sizeof(Elf64_Off),1,f);
    fseek(f, offset +  symbole.st_name, SEEK_SET);  
    j=0;
    fread(&tab[j],sizeof(unsigned char),1,f);
    while(tab[j]!='\0'){
      j++;
    fread(&tab[j],sizeof(unsigned char),1,f);
    }
    
    // On affiche le tout:
    
    // Numero
    printf("  %4d: ",i);
    
    // valeur
    printf("%016x ",symbole.st_value);
    
    // Taille
    printf("%5d ",symbole.st_size);
    
    // Type
    type = (symbole.st_info & 0xf);
    switch (type){
    
    case STT_NOTYPE:
    printf("NOTYPE  ");
    break;
    
    case STT_OBJECT:
    printf("OBJECT  ");
    break;
    
    case STT_FUNC:
    printf("FUNC    ");
    break;
    
    case STT_SECTION:
    printf("SECTION ");
    break;
    
    case STT_FILE:
    printf("FILE    ");
    break;
    
    case STT_COMMON:
    printf("COMMON  ");
    break;
    
    case STT_TLS:
    printf("TLS     ");
    break;
    
    case STT_NUM:
    printf("NUM     ");
    break;
    
    case STT_GNU_IFUNC:
    printf("IFUNC   ");
    break;
    
    
    default:
    printf("???     ");
    break;
    
    }
    
    // Binding
    bind = (symbole.st_info >> 4);
    switch (bind){
    
    case STB_LOCAL:
    printf("LOCAL  ");
    break;
    
    case STB_GLOBAL:
    printf("GLOBAL ");
    break;
    
    case STB_WEAK:
    printf("WEAK   ");
    break;
    
    case STB_NUM:
    printf("NUM    ");
    break;
    
    case STB_GNU_UNIQUE:
    printf("UNIQUE ");
    break;

    
    default:
    printf("???    ");
    break;
    
    }
    // Visibility (pas demandé dans le sujet, mais donné par readelf) 
    // cela viens de la section other, qui n'était pas utilisée dans la documentation, mais qui stocke
    // la visibilité du symbole ici.
    visibility = symbole.st_other & (0x03);
    switch (visibility){
    
    case STV_DEFAULT:
    printf("DEFAULT   ");
    break;
    
    case STV_INTERNAL:
    printf("INTERNAL  ");
    break;
    
    case STV_HIDDEN:
    printf("HIDDEN    ");
    break;
    
    case STV_PROTECTED:
    printf("PROTECTED ");
    break;
    
    default:
    printf("???       ");
    break;
    
    }
    
    // Section concernée
    switch(symbole.st_shndx){
    
    case (SHN_UNDEF):
    printf("UND ");
    break;
    
    case (SHN_ABS):
    printf("ABS ");
    break;
    
    case (SHN_COMMON):
    printf("COM ");
    break;
    
    default:
    printf("%3d ",symbole.st_shndx);
    
    }
    
    // Nom et fin de ligne
    printf("%s\n",tab);
  }

}

// Cherche les tables de symboles dans le fichier elf et affiche leur contenu (version 64 bits)
void search_symbols_tables_64 (FILE* f, Elf64_Ehdr header){
  int i;
  Elf64_Word type;
  // On teste les différents id pour voir si ce sont des tables de symboles
  for (i=0;i<header.e_shnum;i=i+1){
    fseek(f, header.e_shoff + i * header.e_shentsize + sizeof(Elf64_Word), SEEK_SET);
    fread(&type,sizeof(Elf64_Word),1,f);
    if (type == SHT_SYMTAB || type == SHT_DYNSYM) read_symbols_tables_64(i,f,header);
  }
}

