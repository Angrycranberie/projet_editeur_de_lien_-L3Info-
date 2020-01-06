#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include "progbits.h"
#include "fusionsymbole.h"



// Versions 64 bits des fonctions

// Fonction permettant de recuperer la table des symboles du fichier en argument.
symbol_table_64 get_symbol_table_64(FILE * f, Elf64_Ehdr header){
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

// Réalise la fusion des tables de symboles et de leurs tables de chaines associées
symbol_table_64 fusion_symbol_tables_64(Table_sections sections, symbol_table_64 symtab1, symbol_table_64 symtab2){
  int i, j, k;
  int test;
  int indice2;
  symbol_table_64 fusionsym;
  char name[NOMMAX];
  char namefich2[NOMMAX];
  int avancement[TMAX]; // Cette table permet de se rappeler quels symboles du fichier 2 sont déjà traités. Une entrée vaut :
    // 1 si le sybole d'indice correspondant est traité
    // 0 sinon
  // On la remplit ici:
  
  for (i=0;i<symtab2.nbsymbols;i++) avancement[i] = 0;
  
  // On initialise la table de symbole à rendre
  fusionsym.indextable = symtab1.indextable; // On garde l'index de la section du fichier 1
  fusionsym.indexstring = symtab1.indexstring; // même chose
  fusionsym.tailleentree = symtab1.tailleentree; // c'est la même pour toute les tables de symboles
  fusionsym.nbsymbols = 0; // On n'a pas encore mis de symboles dans cette table.
  fusionsym.stringtable.endoftable = 1; // de même pour les chaines.
  fusionsym.stringtable.strings = malloc(1);
  fusionsym.stringtable.strings[0] = '\0'; // On met le \0 initial de la table des chaines
  fusionsym.entries = malloc(sizeof(Elf64_Sym)); // On donne une place initiale pour les entrees
  
  // On parcours les symboles du fichier 1
  
  for (i = 0; i < symtab1.nbsymbols; i++){
    // On alloue de la place dans la structure
    fusionsym.entries = realloc(fusionsym.entries,(fusionsym.nbsymbols+1) * sizeof(Elf64_Sym));
    // Premier cas: c'est une variable locale: On copie l'entrée entière
    if (ELF64_ST_BIND(symtab1.entries[i].st_info) == STB_LOCAL){
      fusionsym.entries[fusionsym.nbsymbols] = symtab1.entries[i];
      
      // On récupère le nom pour plus tard
      
      j = 0;
      name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
      while (name[j] != '\0')
        {
          j++;
          name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
        }

    }
    
    //Second cas: les variables globales
    if (ELF64_ST_BIND(symtab1.entries[i].st_info) == STB_GLOBAL){
      // On récupère le nom
      j = 0;
      name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
      while (name[j] != '\0')
        {
          j++;
          name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
        }
      
      // On vérifie si un symbole du fichier2 est de même nom
      
      test = 0; // Permet de vérifier si on a trouvé un nom correspondant
      
      for (k = 0; k < symtab2.nbsymbols && test == 0; k++){
        // On récupère le nom du symbole du fichier 2 si c'est pertinent (entree globale et pas encore traitée)
        if (ELF64_ST_BIND(symtab2.entries[k].st_info) == STB_GLOBAL && avancement[k]==0){
          j = 0;
          namefich2[j] = symtab2.stringtable.strings[symtab2.entries[k].st_name + j];
          while (namefich2[j] != '\0')
            {
              j++;
              namefich2[j] = symtab2.stringtable.strings[symtab2.entries[k].st_name + j];
            }
          // Si les noms correspondent : On met le test à 1 et on retient l'indice du symbole. On se rappelle aussi que cette entree est désormais deja traitée
          if (strcmp(namefich2,name) == 0){
            indice2 = k;
            test = 1;
            avancement[k]=1;
          }
          
        }
      }
      
      // Plusieurs possibilités:
      // Si on n'a rien trouvé: On copie l'entree entiere
      if (test == 0){
      fusionsym.entries[fusionsym.nbsymbols] = symtab1.entries[i];
      }
      
      // Si on a trouvé un nom correspondant:
      else {
        // Si les deux sont définis on retourne une erreur
        if (symtab1.entries[i].st_shndx!=SHN_UNDEF && symtab2.entries[indice2].st_shndx!=SHN_UNDEF){
          printf(" ERREUR: UN MEME SYMBOL GLOBAL EST DEFINI DANS DEUX FICHIERS DIFFERENTS\n");
          return;
        }
        
        //Si les deux symboles sont indéfinis où que seul celui du fichier 1 l'est, on copie l'entree entiere
        else if(symtab2.entries[indice2].st_shndx==SHN_UNDEF){
          fusionsym.entries[fusionsym.nbsymbols] = symtab1.entries[i];
        }
        
        // Si seul l'entree du fichier 2 est definie: On doit copier l'entree du fichier 2, ce qui inclue de respecter les changements de la partie précédente (offsets)
        else if (symtab2.entries[indice2].st_shndx!=SHN_UNDEF && symtab1.entries[i].st_shndx==SHN_UNDEF){
          // On commence par copier les informations de l'entree
          fusionsym.entries[fusionsym.nbsymbols] = symtab2.entries[indice2];
          
          // Si il s'agit d'une partie fusionnee
          if (sections.Mtable.id_section_merge[symtab2.entries[indice2].st_shndx]!=-1){
            // On pointe vers le nouvel index de la table des sections
            fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.Mtable.id_section_merge[symtab2.entries[indice2].st_shndx];
            // On retrouve la section correspondante (on rappelle que, par construction, dans la table des sections en entree, les index sont en ordre croissant, mais ne correspondent pas à l'indice du tableau mais
            // sont stockés dans la structure.
            k = 0;
            while(sections.index_section[k]!=sections.Mtable.id_section_merge[symtab2.entries[indice2].st_shndx]) k++;
            
            // On ajoute l'offset pour corriger l'emplacement dans les donnees pointees
            fusionsym.entries[fusionsym.nbsymbols].st_value += sections.sections[k].offset;
            
          }
          
          // Si il ne s'agit pas d'une partie fusionnee, on donne juste son indice dans le nouveau fichier
          else {
            
            //On retrouve d'abord de quelle section il s'agit
            k = 0;
            while(sections.sections[k].oldindexfich2 != indice2) k++;
            
            //On donne son nouvel index dans la structure de sortie
            fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.index_section[k];
          }
        }
      }
    }
    
    
    
    // on rajoute l'entree dans la table des chaines (si le nom n'est pas vide)
    if (name[0]=='\0'){
      fusionsym.entries[fusionsym.nbsymbols].st_name = 0;
    }
    else {
      fusionsym.entries[fusionsym.nbsymbols].st_name = fusionsym.stringtable.endoftable;
      j = 0;
      while (name[j] != '\0')
        {
          fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable+1);
          fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = name[j];
          fusionsym.stringtable.endoftable++;
          j++;
        }
        // On rajoute la fin de mot ('\0')
      fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable+1);
      fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = '\0';
      fusionsym.stringtable.endoftable++;
    }
    
    // On incrémente le nombre de symboles
    fusionsym.nbsymbols++;
  }
  
  // On parcours les symboles du fichier 2 désormais
  
  for (i = 0; i < symtab2.nbsymbols; i++){
    // Conditions importante: on ne traite pas deux fois les symboles
    if (avancement[i]==0){
        
      // On alloue de la place dans la structure
      fusionsym.entries = realloc(fusionsym.entries,(fusionsym.nbsymbols+1) * sizeof(Elf64_Sym));
      
      // Ici, au vu des cas deja traités, que la variable soit globale ou locale, elle ne figure que dans le fichier 2, donc on recopie les entrees entieres dans la structure, et on doit traiter les offsets
      
      // Recuperation du contenu
      fusionsym.entries[fusionsym.nbsymbols] = symtab2.entries[i];
      
      
      // Correction des offsets
      // Si il s'agit d'une partie fusionnee
      // On ne le fait pas si c'est de type ABS ou UND
      if (symtab2.entries[i].st_shndx != SHN_UNDEF && symtab2.entries[i].st_shndx != SHN_ABS){
      if (sections.Mtable.id_section_merge[symtab2.entries[i].st_shndx]!=-1){
        // On pointe vers le nouvel index de la table des sections
        fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.Mtable.id_section_merge[symtab2.entries[i].st_shndx];
        // On retrouve la section correspondante (on rappelle que, par construction, dans la table des sections en entree, les index sont en ordre croissant, mais ne correspondent pas à l'indice du tableau mais
        // sont stockés dans la structure.
        k = 0;
        while(sections.index_section[k]!=sections.Mtable.id_section_merge[symtab2.entries[i].st_shndx])k++;
        
        // On ajoute l'offset pour corriger l'emplacement dans les donnees pointees
        fusionsym.entries[fusionsym.nbsymbols].st_value += sections.sections[k].offset;
        
      }
      }
      
      // Si il ne s'agit pas d'une partie fusionnee, on donne juste son indice dans le nouveau fichier
      else {
        
        // On verifie si ce n'est pas un type special d'affectation
        if (symtab2.entries[i].st_shndx != SHN_UNDEF && symtab2.entries[i].st_shndx != SHN_ABS){
          //On retrouve d'abord de quelle section il s'agit
          k = 0;
          while(sections.sections[k].oldindexfich2 != i) k++;
          
          //On donne son nouvel index dans la structure de sortie
          fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.index_section[k];
        }
      }

      // On recupere le nom pour la table des chaines
      j = 0;
            name[j] = symtab2.stringtable.strings[symtab2.entries[i].st_name + j];
            while (namefich2[j] != '\0')
              {
                j++;
                name[j] = symtab2.stringtable.strings[symtab2.entries[i].st_name + j];
              }
      // on rajoute l'entree dans la table des chaines (si le nom n'est pas vide)
      if (name[0]=='\0'){
        fusionsym.entries[fusionsym.nbsymbols].st_name = 0;
      }
      else {
        fusionsym.entries[fusionsym.nbsymbols].st_name = fusionsym.stringtable.endoftable;
        j = 0;
        while (name[j] != '\0')
          {
            fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable+1);
            fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = name[j];
            fusionsym.stringtable.endoftable++;
            j++;
          }
          // On rajoute la fin de mot ('\0')
        fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable+1);
        fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = '\0';
        fusionsym.stringtable.endoftable++;
      }
      
      // On incrémente le nombre de symboles
      fusionsym.nbsymbols++;
    }
  }
  return fusionsym;

}



                                                //                                //
                                                //                                //
                                                //                                //
                                                // Versions 32 bits des fonctions //
                                                //                                //
                                                //                                //






// Fonction permettant de recuperer la table des symboles du fichier en argument.
symbol_table_32 get_symbol_table_32(FILE * f, Elf32_Ehdr header){
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




// Réalise la fusion des tables de symboles et de leurs tables de chaines associées
symbol_table_32 fusion_symbol_tables_32(Table_sections sections, symbol_table_32 symtab1, symbol_table_32 symtab2){
  int i, j, k;
  int test;
  int indice2;
  symbol_table_32 fusionsym;
  char name[NOMMAX];
  char namefich2[NOMMAX];
  int avancement[TMAX]; // Cette table permet de se rappeler quels symboles du fichier 2 sont déjà traités. Une entrée vaut :
    // 1 si le sybole d'indice correspondant est traité
    // 0 sinon
  // On la remplit ici:
  
  for (i=0;i<symtab2.nbsymbols;i++) avancement[i] = 0;
  
  // On initialise la table de symbole à rendre
  fusionsym.indextable = symtab1.indextable; // On garde l'index de la section du fichier 1
  fusionsym.indexstring = symtab1.indexstring; // même chose
  fusionsym.tailleentree = symtab1.tailleentree; // c'est la même pour toute les tables de symboles
  fusionsym.nbsymbols = 0; // On n'a pas encore mis de symboles dans cette table.
  fusionsym.stringtable.endoftable = 1; // de même pour les chaines.
  fusionsym.stringtable.strings = malloc(1);
  fusionsym.stringtable.strings[0] = '\0'; // On met le \0 initial de la table des chaines
  fusionsym.entries = malloc(sizeof(Elf32_Sym)); // On donne une place initiale pour les entrees
  
  // On parcours les symboles du fichier 1
  
  for (i = 0; i < symtab1.nbsymbols; i++){
    // On alloue de la place dans la structure
    fusionsym.entries = realloc(fusionsym.entries,(fusionsym.nbsymbols+1) * sizeof(Elf32_Sym));
    // Premier cas: c'est une variable locale: On copie l'entrée entière
    if (ELF32_ST_BIND(symtab1.entries[i].st_info) == STB_LOCAL){
      fusionsym.entries[fusionsym.nbsymbols] = symtab1.entries[i];
      
      // On récupère le nom pour plus tard
      
      j = 0;
      name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
      while (name[j] != '\0')
        {
          j++;
          name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
        }

    }
    
    //Second cas: les variables globales
    if (ELF32_ST_BIND(symtab1.entries[i].st_info) == STB_GLOBAL){
      // On récupère le nom
      j = 0;
      name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
      while (name[j] != '\0')
        {
          j++;
          name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
        }
      
      // On vérifie si un symbole du fichier2 est de même nom
      
      test = 0; // Permet de vérifier si on a trouvé un nom correspondant
      
      for (k = 0; k < symtab2.nbsymbols && test == 0; k++){
        // On récupère le nom du symbole du fichier 2 si c'est pertinent (entree globale et pas encore traitée)
        if (ELF32_ST_BIND(symtab2.entries[k].st_info) == STB_GLOBAL && avancement[k]==0){
          j = 0;
          namefich2[j] = symtab2.stringtable.strings[symtab2.entries[k].st_name + j];
          while (namefich2[j] != '\0')
            {
              j++;
              namefich2[j] = symtab2.stringtable.strings[symtab2.entries[k].st_name + j];
            }
          // Si les noms correspondent : On met le test à 1 et on retient l'indice du symbole. On se rappelle aussi que cette entree est désormais deja traitée
          if (strcmp(namefich2,name) == 0){
            indice2 = k;
            test = 1;
            avancement[k]=1;
          }
          
        }
      }
      
      // Plusieurs possibilités:
      // Si on n'a rien trouvé: On copie l'entree entiere
      if (test == 0){
      fusionsym.entries[fusionsym.nbsymbols] = symtab1.entries[i];
      }
      
      // Si on a trouvé un nom correspondant:
      else {
        // Si les deux sont définis on retourne une erreur
        if (symtab1.entries[i].st_shndx!=SHN_UNDEF && symtab2.entries[indice2].st_shndx!=SHN_UNDEF){
          printf(" ERREUR: UN MEME SYMBOL GLOBAL EST DEFINI DANS DEUX FICHIERS DIFFERENTS\n");
          return;
        }
        
        //Si les deux symboles sont indéfinis où que seul celui du fichier 1 l'est, on copie l'entree entiere
        else if(symtab2.entries[indice2].st_shndx==SHN_UNDEF){
          fusionsym.entries[fusionsym.nbsymbols] = symtab1.entries[i];
        }
        
        // Si seul l'entree du fichier 2 est definie: On doit copier l'entree du fichier 2, ce qui inclue de respecter les changements de la partie précédente (offsets)
        else if (symtab2.entries[indice2].st_shndx!=SHN_UNDEF && symtab1.entries[i].st_shndx==SHN_UNDEF){
          // On commence par copier les informations de l'entree
          fusionsym.entries[fusionsym.nbsymbols] = symtab2.entries[indice2];
          
          // Si il s'agit d'une partie fusionnee
          if (sections.Mtable.id_section_merge[symtab2.entries[indice2].st_shndx]!=-1){
            // On pointe vers le nouvel index de la table des sections
            fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.Mtable.id_section_merge[symtab2.entries[indice2].st_shndx];
            // On retrouve la section correspondante (on rappelle que, par construction, dans la table des sections en entree, les index sont en ordre croissant, mais ne correspondent pas à l'indice du tableau mais
            // sont stockés dans la structure.
            k = 0;
            while(sections.index_section[k]!=sections.Mtable.id_section_merge[symtab2.entries[indice2].st_shndx]) k++;
            
            // On ajoute l'offset pour corriger l'emplacement dans les donnees pointees
            fusionsym.entries[fusionsym.nbsymbols].st_value += sections.sections[k].offset;
            
          }
          
          // Si il ne s'agit pas d'une partie fusionnee, on donne juste son indice dans le nouveau fichier
          else {
            
            //On retrouve d'abord de quelle section il s'agit
            k = 0;
            while(sections.sections[k].oldindexfich2 != indice2) k++;
            
            //On donne son nouvel index dans la structure de sortie
            fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.index_section[k];
          }
        }
      }
    }
    
    
    
    // on rajoute l'entree dans la table des chaines (si le nom n'est pas vide)
    if (name[0]=='\0'){
      fusionsym.entries[fusionsym.nbsymbols].st_name = 0;
    }
    else {
      fusionsym.entries[fusionsym.nbsymbols].st_name = fusionsym.stringtable.endoftable;
      j = 0;
      while (name[j] != '\0')
        {
          fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable+1);
          fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = name[j];
          fusionsym.stringtable.endoftable++;
          j++;
        }
        // On rajoute la fin de mot ('\0')
      fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable+1);
      fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = '\0';
      fusionsym.stringtable.endoftable++;
    }
    
    // On incrémente le nombre de symboles
    fusionsym.nbsymbols++;
  }
  
  // On parcours les symboles du fichier 2 désormais
  
  for (i = 0; i < symtab2.nbsymbols; i++){
    // Conditions importante: on ne traite pas deux fois les symboles
    if (avancement[i]==0){
        
      // On alloue de la place dans la structure
      fusionsym.entries = realloc(fusionsym.entries,(fusionsym.nbsymbols+1) * sizeof(Elf32_Sym));
      
      // Ici, au vu des cas deja traités, que la variable soit globale ou locale, elle ne figure que dans le fichier 2, donc on recopie les entrees entieres dans la structure, et on doit traiter les offsets
      
      // Recuperation du contenu
      fusionsym.entries[fusionsym.nbsymbols] = symtab2.entries[i];
      
      
      // Correction des offsets
      // Si il s'agit d'une partie fusionnee
      // On ne le fait pas si c'est de type ABS ou UND
      if (symtab2.entries[i].st_shndx != SHN_UNDEF && symtab2.entries[i].st_shndx != SHN_ABS){
      if (sections.Mtable.id_section_merge[symtab2.entries[i].st_shndx]!=-1){
        // On pointe vers le nouvel index de la table des sections
        fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.Mtable.id_section_merge[symtab2.entries[i].st_shndx];
        // On retrouve la section correspondante (on rappelle que, par construction, dans la table des sections en entree, les index sont en ordre croissant, mais ne correspondent pas à l'indice du tableau mais
        // sont stockés dans la structure.
        k = 0;
        while(sections.index_section[k]!=sections.Mtable.id_section_merge[symtab2.entries[i].st_shndx])k++;
        
        // On ajoute l'offset pour corriger l'emplacement dans les donnees pointees
        fusionsym.entries[fusionsym.nbsymbols].st_value += sections.sections[k].offset;
        
      }
      }
      
      // Si il ne s'agit pas d'une partie fusionnee, on donne juste son indice dans le nouveau fichier
      else {
        
        // On verifie si ce n'est pas un type special d'affectation
        if (symtab2.entries[i].st_shndx != SHN_UNDEF && symtab2.entries[i].st_shndx != SHN_ABS){
          //On retrouve d'abord de quelle section il s'agit
          k = 0;
          while(sections.sections[k].oldindexfich2 != i) k++;
          
          //On donne son nouvel index dans la structure de sortie
          fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.index_section[k];
        }
      }

      // On recupere le nom pour la table des chaines
      j = 0;
            name[j] = symtab2.stringtable.strings[symtab2.entries[i].st_name + j];
            while (namefich2[j] != '\0')
              {
                j++;
                name[j] = symtab2.stringtable.strings[symtab2.entries[i].st_name + j];
              }
      // on rajoute l'entree dans la table des chaines (si le nom n'est pas vide)
      if (name[0]=='\0'){
        fusionsym.entries[fusionsym.nbsymbols].st_name = 0;
      }
      else {
        fusionsym.entries[fusionsym.nbsymbols].st_name = fusionsym.stringtable.endoftable;
        j = 0;
        while (name[j] != '\0')
          {
            fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable+1);
            fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = name[j];
            fusionsym.stringtable.endoftable++;
            j++;
          }
          // On rajoute la fin de mot ('\0')
        fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable+1);
        fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = '\0';
        fusionsym.stringtable.endoftable++;
      }
      
      // On incrémente le nombre de symboles
      fusionsym.nbsymbols++;
    }
  }
  return fusionsym;

}




