#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include "symboltable.h"
#include "tablesection.h"
#include "values.h"
#include "util.h"


// seule la version 64 bits est vraiment utile
symbol_table_64 read_symbols_tables_64(FILE * f, Elf64_Ehdr header, section_list seclist){
  symbol_table_64 symtable;
  int nb;
  int i;
  // version 32(0) ou 64 bit(1) 
  int bits_version;
  // indique si l'endianess est différent entre la machine et le fichier 1 -> oui 0 -> non
  int diff_endianess;
  bits_version = header.e_ident[EI_CLASS] == ELFCLASS64;
  diff_endianess = (is_big_endian() != (int)(header.e_ident[EI_DATA] == ELFDATA2MSB));
  
  // Verifie si on a trouve la table des symboles
  int test = 0;
  for (i=0;i<seclist.nb_section && test == 0;i=i+1){
    
    // On verifie si on a bien une table de symboles
    if (seclist.sec_list[i].sh_type==SHT_SYMTAB){
      // On indique qu'on a trouve la table des symboles
      test = 1;
      
      // On recupere l'addresse de la table de chaine correspondante et la taille des entrees
      symtable.indexstring = seclist.sec_list[i].sh_link;
      symtable.tailleentree = seclist.sec_list[i].sh_entsize;
      
      // On stocke l'index
      symtable.indextable = i;
      
      // Calcul du nombre d'entrees dans la table
      nb = seclist.sec_list[i].sh_size/seclist.sec_list[i].sh_entsize;
      symtable.nbsymbols = nb;
      
      // On alloue la place necessaire 
      symtable.entries = malloc(nb * sizeof(Elf64_Sym));
      
      // On recupere toutes les entrees de la table des symboles
      int j;
      for(j=0; j < nb; j++){
        // On se place dans la table des symboles
        fseek(f, seclist.sec_list[i].sh_offset + j * seclist.sec_list[i].sh_entsize, SEEK_SET);
        
        // On recupere une entree (on separe le cas 32 et 64 bits)
        // cas 64 bits
        if (bits_version == 1){
          fread(&symtable.entries[j].st_name, sizeof(Elf64_Word),1,f);
          fread(&symtable.entries[j].st_info, sizeof(unsigned char),1,f);
          fread(&symtable.entries[j].st_other, sizeof(unsigned char),1,f);
          fread(&symtable.entries[j].st_shndx, sizeof(Elf64_Section),1,f);
          fread(&symtable.entries[j].st_value, sizeof(Elf64_Addr),1,f); 
          fread(&symtable.entries[j].st_size, sizeof(Elf64_Xword),1,f);
        }
        // cas 32 bits
        else {
          fread(&symtable.entries[j].st_name, sizeof(Elf64_Word),1,f);
          fread(&symtable.entries[j].st_value, sizeof(Elf32_Addr),1,f); symtable.entries[j].st_value &= 0xFFFFFFFF;
          fread(&symtable.entries[j].st_size, sizeof(Elf32_Word),1,f); symtable.entries[j].st_size &= 0xFFFFFFFF;
          fread(&symtable.entries[j].st_info, sizeof(unsigned char),1,f);
          fread(&symtable.entries[j].st_other, sizeof(unsigned char),1,f);
          fread(&symtable.entries[j].st_shndx, sizeof(Elf64_Section),1,f);
        }
        
        // On corrige son endianess
        if(diff_endianess){
        symtable.entries[j].st_name = reverse_4(symtable.entries[j].st_name);
        symtable.entries[j].st_shndx = reverse_2(symtable.entries[j].st_shndx);
        symtable.entries[j].st_value = reverse_8(symtable.entries[j].st_value) ;
        symtable.entries[j].st_size = reverse_8(symtable.entries[j].st_size) ;
        }
        
      }
    }
    
  }
  // Ici, on a trouve la table des symboles (normalement), on recupere maintenant la table de chaine associee
  
  
  
  

  // On recupere la taille totale
  symtable.stringtable.endoftable = seclist.sec_list[symtable.indexstring].sh_size;
  
  // On alloue la place necessaire 
  symtable.stringtable.strings = malloc(seclist.sec_list[symtable.indexstring].sh_size);
  
  // On se place dans la table de chaines
  fseek(f, seclist.sec_list[symtable.indexstring].sh_offset, SEEK_SET);
      
  // On recupere les valeurs
  fread(symtable.stringtable.strings, seclist.sec_list[symtable.indexstring].sh_size,1,f);
  
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
