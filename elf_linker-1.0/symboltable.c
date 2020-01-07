#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include "symboltable.h"
#include "values.h"

symbol_table_64 read_symbols_tables_64(FILE * f, Elf64_Ehdr header){
  symbol_table_64 symtable;
  Elf64_Word type;
  Elf64_Xword totalsize;
  Elf64_Xword entsize;
  Elf64_Off offset;
  int nb;
  int i;
  
  // Verifie si on  a trouve la table des symboles
  int test = 0;
  for (i=0;i<header.e_shnum && test == 0;i=i+1){
    fseek(f, header.e_shoff + i * header.e_shentsize + sizeof(Elf64_Word), SEEK_SET);
    
    // On verifie si on a bien une table de symboles
    fread(&type, sizeof(Elf64_Word),1,f);
    if (type==SHT_SYMTAB){
      // On indique qu'on a trouve la table des symboles
      test = 1;
      // On saute des valeurs sans importance
      fseek(f, sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_CUR);
      
      // On recupere l'offset, la taille totale de la section et l'addresse de la table de chaine correspondante
      fread(&offset, sizeof(Elf64_Off),1,f);
      fread(&totalsize, sizeof(Elf64_Xword),1,f);
      fread(&symtable.indexstring, sizeof(Elf64_Word),1,f);
      
      // On saute des valeurs sans importance
      fseek(f, sizeof(Elf64_Word)  + sizeof(Elf64_Xword), SEEK_CUR);
      
      // On recupere la taille d'une entree de la section
      fread(&entsize, sizeof(Elf64_Xword),1,f);
      symtable.tailleentree = entsize;
      
      // On stocke l'index
      symtable.indextable = i;
      
      // Calcul du nobre d'entrees dans la table
      nb = totalsize/entsize;
      symtable.nbsymbols = nb;
      
      // On alloue la place necessaire 
      symtable.entries = malloc(nb * sizeof(Elf64_Sym));
      
      // On recupere toutes les entrees de la table des symboles
      for(int j=0; j < nb; j++){
        // On se place dans la table des symboles
        fseek(f, offset + j * entsize, SEEK_SET);
        
        // On recupere une entree
        fread(&symtable.entries[j].st_name, sizeof(Elf64_Word),1,f);
        fread(&symtable.entries[j].st_info, sizeof(unsigned char),1,f);
        fread(&symtable.entries[j].st_other, sizeof(unsigned char),1,f);
        fread(&symtable.entries[j].st_shndx, sizeof(Elf64_Section),1,f);
        fread(&symtable.entries[j].st_value, sizeof(Elf64_Addr),1,f);
        fread(&symtable.entries[j].st_size, sizeof(Elf64_Xword),1,f);
      }
    }
    
  }
  // Ici, on a trouve la table des symboles (normalement), on recupere maintenant la table de chaine associee
  
  // On se place pour recuperer son offset et sa taille
  fseek(f, header.e_shoff + symtable.indexstring * header.e_shentsize + sizeof(Elf64_Word) * 2 + sizeof(Elf64_Xword) + sizeof(Elf64_Addr) , SEEK_SET);
  fread(&offset, sizeof(Elf64_Off),1,f);
  fread(&totalsize, sizeof(Elf64_Xword),1,f);
  symtable.stringtable.endoftable = totalsize;
  
  // On alloue la place necessaire 
  symtable.stringtable.strings = malloc(totalsize);
  
  // On se place dans la table de chaines
  fseek(f, offset, SEEK_SET);
      
  // On recupere les valeurs
  fread(symtable.stringtable.strings, totalsize,1,f);
  
  // On renvoie la table, qui est completee
  return symtable;
  
  
}

// Fonction permettant d'afficher la table de symboles stockee en memoire

void print_symbol_table_64(symbol_table_64 symtable){
  int i;
  int j;
  char name[NOMMAX];

    // On affiche le nom et le nombre d'entrees de la table
  printf("The symbol table contains %d entries:\n", symtable.nbsymbols);

  // On affiche le haut du tableau a afficher
  printf("   Num:    Value          Size Type    Bind   Vis      Ndx Name\n");
  
  for (i = 0; i < symtable.nbsymbols; i++){
    // NumÃ©ro
    printf("  %4d: ", i);

    // Valeur
    printf("%016x ", symtable.entries[i].st_value);

    // Taille
    printf("%5d ", symtable.entries[i].st_size);

    // Type
    switch (ELF64_ST_TYPE(symtable.entries[i].st_info))
    {
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
    switch (ELF64_ST_BIND(symtable.entries[i].st_info))
    {
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

    // Visibilite 
    switch (ELF64_ST_VISIBILITY(symtable.entries[i].st_other))
    {
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

    // Section concernee
    switch (symtable.entries[i].st_shndx)
    {
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
      printf("%3d ", symtable.entries[i].st_shndx);
    }
    
    // On recupere le nom
    
    
    
    j = 0;
    name[j] = symtable.stringtable.strings[symtable.entries[i].st_name + j];
    while (name[j] != '\0')
    {
      j++;
      name[j] = symtable.stringtable.strings[symtable.entries[i].st_name + j];
    }
    // Nom et fin de ligne
    printf("%s\n", name);
  }

}




                                                //                                //
                                                //                                //
                                                //                                //
                                                // Versions 32 bits des fonctions //
                                                //                                //
                                                //                                //






// Fonction permettant de recuperer la table des symboles du fichier en argument.
symbol_table_32 read_symbols_tables_32(FILE * f, Elf32_Ehdr header){
  symbol_table_32 symtable;
  Elf32_Word type;
  Elf32_Word totalsize;
  Elf32_Word entsize;
  Elf32_Off offset;
  int nb;
  int i;
  
  // Verifie si on  a trouve la table des symboles
  int test = 0;
  for (i=0;i<header.e_shnum && test == 0;i=i+1){
    fseek(f, header.e_shoff + i * header.e_shentsize + sizeof(Elf32_Word), SEEK_SET);
    
    // On verifie si on a bien une table de symboles
    fread(&type, sizeof(Elf32_Word),1,f);
    if (type==SHT_SYMTAB){
      // On indique qu'on a trouve la table des symboles
      test = 1;
      // On saute des valeurs sans importance
      fseek(f, sizeof(Elf32_Word) + sizeof(Elf32_Addr), SEEK_CUR);
      
      // On recupere l'offset, la taille totale de la section et l'addresse de la table de chaine correspondante
      fread(&offset, sizeof(Elf32_Off),1,f);
      fread(&totalsize, sizeof(Elf32_Word),1,f);
      fread(&symtable.indexstring, sizeof(Elf32_Word),1,f);
      
      // On saute des valeurs sans importance
      fseek(f, sizeof(Elf32_Word)  + sizeof(Elf32_Word), SEEK_CUR);
      
      // On recupere la taille d'une entree de la section
      fread(&entsize, sizeof(Elf32_Word),1,f);
      symtable.tailleentree = entsize;
      
      // On stocke l'index
      symtable.indextable = i;
      
      // Calcul du nobre d'entrees dans la table
      nb = totalsize/entsize;
      symtable.nbsymbols = nb;
      
      // On alloue la place necessaire 
      symtable.entries = malloc(nb * sizeof(Elf32_Sym));
      
      // On recupere toutes les entrees de la table des symboles
      for(int j=0; j < nb; j++){
        // On se place dans la table des symboles
        fseek(f, offset + j * entsize, SEEK_SET);
        
        // On recupere une entree
        fread(&symtable.entries[j].st_name, sizeof(Elf32_Word),1,f);
        fread(&symtable.entries[j].st_value, sizeof(Elf32_Addr),1,f);
        fread(&symtable.entries[j].st_size, sizeof(Elf32_Word),1,f);
        fread(&symtable.entries[j].st_info, sizeof(unsigned char),1,f);
        fread(&symtable.entries[j].st_other, sizeof(unsigned char),1,f);
        fread(&symtable.entries[j].st_shndx, sizeof(Elf32_Section),1,f);
      }
    }
    
  }
  // Ici, on a trouve la table des symboles (normalement), on recupere maintenant la table de chaine associee
  
  // On se place pour recuperer son offset et sa taille
  fseek(f, header.e_shoff + symtable.indexstring * header.e_shentsize + sizeof(Elf32_Word) * 2 + sizeof(Elf32_Word) + sizeof(Elf32_Addr) , SEEK_SET);
  fread(&offset, sizeof(Elf32_Off),1,f);
  fread(&totalsize, sizeof(Elf32_Word),1,f);
  symtable.stringtable.endoftable = totalsize;
  
  // On alloue la place necessaire 
  symtable.stringtable.strings = malloc(totalsize);
  
  // On se place dans la table de chaines
  fseek(f, offset, SEEK_SET);
      
  // On recupere les valeurs
  fread(symtable.stringtable.strings, totalsize,1,f);
  
  // On renvoie la table, qui est completee
  return symtable;
  
  
}

// Fonction permettant d'afficher la table de symboles stockee en memoire

void print_symbol_table_32(symbol_table_32 symtable){
  int i;
  int j;
  char name[NOMMAX];

    // On affiche le nom et le nombre d'entrees de la table
  printf("The symbol table contains %d entries:\n", symtable.nbsymbols);

  // On affiche le haut du tableau a afficher
  printf("   Num:    Value          Size Type    Bind   Vis      Ndx Name\n");
  
  for (i = 0; i < symtable.nbsymbols; i++){
    // NumÃ©ro
    printf("  %4d: ", i);

    // Valeur
    printf("%016x ", symtable.entries[i].st_value);

    // Taille
    printf("%5d ", symtable.entries[i].st_size);

    // Type
    switch (ELF32_ST_TYPE(symtable.entries[i].st_info))
    {
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
    switch (ELF32_ST_BIND(symtable.entries[i].st_info))
    {
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

    // Visibilite 
    switch (ELF32_ST_VISIBILITY(symtable.entries[i].st_other))
    {
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

    // Section concernee
    switch (symtable.entries[i].st_shndx)
    {
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
      printf("%3d ", symtable.entries[i].st_shndx);
    }
    
    // On recupere le nom
    
    
    
    j = 0;
    name[j] = symtable.stringtable.strings[symtable.entries[i].st_name + j];
    while (name[j] != '\0')
    {
      j++;
      name[j] = symtable.stringtable.strings[symtable.entries[i].st_name + j];
    }
    // Nom et fin de ligne
    printf("%s\n", name);
  }

}

