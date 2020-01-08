#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include "progbits.h"
#include "symboltable.h"
#include "fusionsymbole.h"

/*********************************/
/* Version 64 bits des fonctions */
/*********************************/

// Réalise la fusion des tables de symboles et de leurs tables de chaines associées
symbol_table_64 fusion_symbol_tables_64(Table_sections sections, symbol_table_64 symtab1, symbol_table_64 symtab2)
{
  int i, j, k;
  int test;
  int indice2;
  symbol_table_64 fusionsym;
  char name[NOMMAX];
  char namefich2[NOMMAX];
  int avancement[TMAX];
  // Cette table permet de se rappeler quels symboles du fichier 2 ont déjà été traités. Une entrée vaut
  // 1 si le sybole d'indice correspondant est traité, 0 sinon.
  // On la remplit ici:
  for (i = 0; i < symtab2.nbsymbols; i++) avancement[i] = 0;

  // On initialise la table de symboles à rendre.
  fusionsym.indextable = symtab1.indextable;     // On garde l'index de la section du fichier 1.
  fusionsym.indexstring = symtab1.indexstring;   // Idem.
  fusionsym.tailleentree = symtab1.tailleentree; // C'est la même pour toute les tables de symboles.
  fusionsym.nbsymbols = 0;                       // On n'a pas encore mis de symboles dans cette table.
  fusionsym.stringtable.endoftable = 1;          // De même pour les chaines.
  fusionsym.stringtable.strings = malloc(1);
  fusionsym.stringtable.strings[0] = '\0';       // On met le '\0' initial de la table des chaines.
  fusionsym.entries = malloc(sizeof(Elf64_Sym)); // On donne une place initiale pour les entrées.

  // On parcourt les symboles du fichier 1.
  for (i = 0; i < symtab1.nbsymbols; i++)
  {
    // On alloue de la place dans la structure.
    fusionsym.entries = realloc(fusionsym.entries, (fusionsym.nbsymbols + 1) * sizeof(Elf64_Sym));

    // Premier cas: c'est une variable locale ; on copie l'entrée entière.
    if (ELF64_ST_BIND(symtab1.entries[i].st_info) == STB_LOCAL)
    {
      fusionsym.entries[fusionsym.nbsymbols] = symtab1.entries[i];
      // On récupère le nom pour plus tard.
      j = 0;
      name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
      while (name[j] != '\0')
      {
        j++;
        name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
      }
    }

    // Second cas: les variables globales.
    if (ELF64_ST_BIND(symtab1.entries[i].st_info) == STB_GLOBAL)
    {
      // On récupère le nom.
      j = 0;
      name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
      while (name[j] != '\0')
      {
        j++;
        name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
      }

      // On vérifie si un symbole du fichier2 est de même nom.
      test = 0; // Permet de vérifier si on a trouvé un nom correspondant.

      for (k = 0; k < symtab2.nbsymbols && test == 0; k++)
      {
        // On récupère le nom du symbole du fichier 2 si c'est pertinent (entrée globale encore non traitée).
        if (ELF64_ST_BIND(symtab2.entries[k].st_info) == STB_GLOBAL && avancement[k] == 0)
        {
          j = 0;
          namefich2[j] = symtab2.stringtable.strings[symtab2.entries[k].st_name + j];
          while (namefich2[j] != '\0')
          {
            j++;
            namefich2[j] = symtab2.stringtable.strings[symtab2.entries[k].st_name + j];
          }
          // Si les noms correspondent : on met le test à 1 et on retient l'indice du symbole. On se rappelle aussi que cette entrée est désormais déjà traitée.
          if (strcmp(namefich2, name) == 0)
          {
            indice2 = k;
            test = 1;
            avancement[k] = 1;
          }
        }
      }

      // Plusieurs possibilités :
      // Si on n'a rien trouvé, on copie l'entrée entière.
      if (test == 0) { fusionsym.entries[fusionsym.nbsymbols] = symtab1.entries[i]; }
      // Si on a trouvé un nom correspondant :
      else
      {
        // Si les deux sont définis, on retourne une erreur.
        if (symtab1.entries[i].st_shndx != SHN_UNDEF && symtab2.entries[indice2].st_shndx != SHN_UNDEF)
        {
          printf("ERREUR: UN MEME SYMBOLE GLOBAL EST DEFINI DANS DEUX FICHIERS DIFFERENTS !\n");
          return;
        }

        // Si les deux symboles sont indéfinis ou que seul celui du fichier 1 l'est, on copie l'entrée entière.
        else if (symtab2.entries[indice2].st_shndx == SHN_UNDEF)
        {
          fusionsym.entries[fusionsym.nbsymbols] = symtab1.entries[i];
        }

        // Si seule l'entrée du fichier 2 est définie, on doit copier celle-ci, ce qui implique de respecter les changements de la partie précédente (offsets).
        else if (symtab2.entries[indice2].st_shndx != SHN_UNDEF && symtab1.entries[i].st_shndx == SHN_UNDEF)
        {
          // On commence par copier les informations de l'entrée.
          fusionsym.entries[fusionsym.nbsymbols] = symtab2.entries[indice2];

          // Si il s'agit d'une partie fusionnée...
          if (sections.Mtable.id_section_merge[symtab2.entries[indice2].st_shndx] != -1)
          {
            // On pointe vers le nouvel index de la table des sections.
            fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.Mtable.id_section_merge[symtab2.entries[indice2].st_shndx];
            /* On retrouve la section correspondante (on rappelle que, par construction, dans la table des sections en entrée,
            les index sont en ordre croissant, mais ne correspondent pas à l'indice du tableau mais sont stockés dans la structure. */
            k = 0;
            while (sections.index_section[k] != sections.Mtable.id_section_merge[symtab2.entries[indice2].st_shndx])
              k++;

            // On ajoute l'offset pour corriger l'emplacement dans les donnees pointées.
            fusionsym.entries[fusionsym.nbsymbols].st_value += sections.sections[k].offset;
          }

          // Si il ne s'agit pas d'une partie fusionnée, on donne juste son indice dans le nouveau fichier.
          else
          {
            // On retrouve d'abord de quelle section il s'agit.
            k = 0;
            while (sections.sections[k].oldindexfich2 != indice2)
              k++;

            // On donne son nouvel index dans la structure de sortie.
            fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.index_section[k];
          }
        }
      }
    }

    // On rajoute l'entrée dans la table des chaines (si le nom n'est pas vide).
    if (name[0] == '\0')
    {
      fusionsym.entries[fusionsym.nbsymbols].st_name = 0;
    }
    else
    {
      fusionsym.entries[fusionsym.nbsymbols].st_name = fusionsym.stringtable.endoftable;
      j = 0;
      while (name[j] != '\0')
      {
        fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable + 1);
        fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = name[j];
        fusionsym.stringtable.endoftable++;
        j++;
      }
      // On rajoute la fin de mot ('\0').
      fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable + 1);
      fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = '\0';
      fusionsym.stringtable.endoftable++;
    }

    // On incrémente le nombre de symboles.
    fusionsym.nbsymbols++;
  }

  // On parcourt désormais les symboles du fichier 2.
  for (i = 0; i < symtab2.nbsymbols; i++)
  {
    // Condition importante : on ne traite pas deux fois les symboles !
    if (avancement[i] == 0)
    {

      // On alloue de la place dans la structure.
      fusionsym.entries = realloc(fusionsym.entries, (fusionsym.nbsymbols + 1) * sizeof(Elf64_Sym));

      /* Ici, au vu des cas déjà traités, que la variable soit globale ou locale, elle ne figure que dans le fichier 2,
      donc on recopie les entrées entières dans la structure et on doit traiter les offsets. */

      // Récuperation du contenu.
      fusionsym.entries[fusionsym.nbsymbols] = symtab2.entries[i];

      // Correction des offsets si il s'agit d'une partie fusionnée,
      // on ne le fait pas si c'est de type ABS ou UNDEF.
      if (symtab2.entries[i].st_shndx != SHN_UNDEF && symtab2.entries[i].st_shndx != SHN_ABS)
      {
        if (sections.Mtable.id_section_merge[symtab2.entries[i].st_shndx] != -1)
        {
          // On pointe vers le nouvel index de la table des sections.
          fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.Mtable.id_section_merge[symtab2.entries[i].st_shndx];
          /* On retrouve la section correspondante (on rappelle que, par construction, dans la table des sections en entrée,
          les index sont en ordre croissant, mais ne correspondent pas à l'indice du tableau mais sont stockés dans la structure. */
          k = 0;
          while (sections.index_section[k] != sections.Mtable.id_section_merge[symtab2.entries[i].st_shndx])
            k++;

          // On ajoute l'offset pour corriger l'emplacement dans les donnees pointées.
          fusionsym.entries[fusionsym.nbsymbols].st_value += sections.sections[k].offset;
        }
      }

      // Si il ne s'agit pas d'une partie fusionnée, on donne juste son indice dans le nouveau fichier.
      else
      {
        // On vérifie si ce n'est pas un type spécial d'affectation.
        if (symtab2.entries[i].st_shndx != SHN_UNDEF && symtab2.entries[i].st_shndx != SHN_ABS)
        {
          // On retrouve d'abord de quelle section il s'agit.
          k = 0;
          while (sections.sections[k].oldindexfich2 != i)
            k++;

          // On donne son nouvel index dans la structure de sortie.
          fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.index_section[k];
        }
      }

      // On récupère le nom pour la table des chaines.
      j = 0;
      name[j] = symtab2.stringtable.strings[symtab2.entries[i].st_name + j];
      while (namefich2[j] != '\0')
      {
        j++;
        name[j] = symtab2.stringtable.strings[symtab2.entries[i].st_name + j];
      }
      // On rajoute l'entrée dans la table des chaines (si le nom n'est pas vide).
      if (name[0] == '\0')
      {
        fusionsym.entries[fusionsym.nbsymbols].st_name = 0;
      }
      else
      {
        fusionsym.entries[fusionsym.nbsymbols].st_name = fusionsym.stringtable.endoftable;
        j = 0;
        while (name[j] != '\0')
        {
          fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable + 1);
          fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = name[j];
          fusionsym.stringtable.endoftable++;
          j++;
        }
        // On rajoute la fin de mot ('\0').
        fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable + 1);
        fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = '\0';
        fusionsym.stringtable.endoftable++;
      }

      // On incrémente le nombre de symboles.
      fusionsym.nbsymbols++;
    }
  }
  return fusionsym;
}


/*********************************/
/* Version 32 bits des fonctions */
/*********************************/

// Réalise la fusion des tables de symboles et de leurs tables de chaines associées.
symbol_table_32 fusion_symbol_tables_32(Table_sections sections, symbol_table_32 symtab1, symbol_table_32 symtab2)
{
  int i, j, k;
  int test;
  int indice2;
  symbol_table_32 fusionsym;
  char name[NOMMAX];
  char namefich2[NOMMAX];
  int avancement[TMAX];
  /* Cette table permet de se rappeler quels symboles du fichier 2 sont déjà traités. Une entrée vaut
  1 si le symbole d'indice correspondant est traité, 0 sinon. */
  // On la remplit ici :
  for (i = 0; i < symtab2.nbsymbols; i++) avancement[i] = 0;

  // On initialise la table de symboles à rendre.
  fusionsym.indextable = symtab1.indextable;     // On garde l'index de la section du fichier 1.
  fusionsym.indexstring = symtab1.indexstring;   // Idem.
  fusionsym.tailleentree = symtab1.tailleentree; // C'est la même pour toute les tables de symboles.
  fusionsym.nbsymbols = 0;                       // On n'a pas encore mis de symboles dans cette table.
  fusionsym.stringtable.endoftable = 1;          // De même pour les chaines.
  fusionsym.stringtable.strings = malloc(1);
  fusionsym.stringtable.strings[0] = '\0';       // On met le \0 initial de la table des chaines.
  fusionsym.entries = malloc(sizeof(Elf32_Sym)); // On donne une place initiale pour les entrées.

  // On parcourt les symboles du fichier 1.
  for (i = 0; i < symtab1.nbsymbols; i++)
  {
    // On alloue de la place dans la structure.
    fusionsym.entries = realloc(fusionsym.entries, (fusionsym.nbsymbols + 1) * sizeof(Elf32_Sym));

    // Premier cas : c'est une variable locale, on copie l'entrée entière.
    if (ELF32_ST_BIND(symtab1.entries[i].st_info) == STB_LOCAL)
    {
      fusionsym.entries[fusionsym.nbsymbols] = symtab1.entries[i];

      // On récupère le nom pour plus tard.
      j = 0;
      name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
      while (name[j] != '\0')
      {
        j++;
        name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
      }
    }

    // Second cas : les variables globales.
    if (ELF32_ST_BIND(symtab1.entries[i].st_info) == STB_GLOBAL)
    {
      // On récupère le nom.
      j = 0;
      name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
      while (name[j] != '\0')
      {
        j++;
        name[j] = symtab1.stringtable.strings[symtab1.entries[i].st_name + j];
      }

      // On vérifie si un symbole du fichier 2 est de même nom.
      test = 0; // Permet de vérifier si on a trouvé un nom correspondant.

      for (k = 0; k < symtab2.nbsymbols && test == 0; k++)
      {
        // On récupère le nom du symbole du fichier 2 si c'est pertinent (entrée globale et pas encore traitée).
        if (ELF32_ST_BIND(symtab2.entries[k].st_info) == STB_GLOBAL && avancement[k] == 0)
        {
          j = 0;
          namefich2[j] = symtab2.stringtable.strings[symtab2.entries[k].st_name + j];
          while (namefich2[j] != '\0')
          {
            j++;
            namefich2[j] = symtab2.stringtable.strings[symtab2.entries[k].st_name + j];
          }
          // Si les noms correspondent : on met le test à 1 et on retient l'indice du symbole.
          // On se rappelle aussi que cette entrée est désormais déjà traitée.
          if (strcmp(namefich2, name) == 0)
          {
            indice2 = k;
            test = 1;
            avancement[k] = 1;
          }
        }
      }

      // Plusieurs possibilités :
      // Si on n'a rien trouvé, on copie l'entrée entière.
      if (test == 0)
      {
        fusionsym.entries[fusionsym.nbsymbols] = symtab1.entries[i];
      }

      // Si on a trouvé un nom correspondant...
      else
      {
        // Si les deux sont définis, on retourne une erreur.
        if (symtab1.entries[i].st_shndx != SHN_UNDEF && symtab2.entries[indice2].st_shndx != SHN_UNDEF)
        {
          printf(" ERREUR: UN MEME SYMBOL GLOBAL EST DEFINI DANS DEUX FICHIERS DIFFERENTS\n");
          return;
        }

        // Si les deux symboles sont indéfinis ou que seul celui du fichier 1 l'est, on copie l'entrée entière.
        else if (symtab2.entries[indice2].st_shndx == SHN_UNDEF)
        {
          fusionsym.entries[fusionsym.nbsymbols] = symtab1.entries[i];
        }

        // Si seul l'entrée du fichier 2 est définie, on doit copier celle-ci, ce qui implique de respecter les changements de la partie précédente (offsets).
        else if (symtab2.entries[indice2].st_shndx != SHN_UNDEF && symtab1.entries[i].st_shndx == SHN_UNDEF)
        {
          // On commence par copier les informations de l'entrée.
          fusionsym.entries[fusionsym.nbsymbols] = symtab2.entries[indice2];

          // Si il s'agit d'une partie fusionnée...
          if (sections.Mtable.id_section_merge[symtab2.entries[indice2].st_shndx] != -1)
          {
            // On pointe vers le nouvel index de la table des sections
            fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.Mtable.id_section_merge[symtab2.entries[indice2].st_shndx];
            // On retrouve la section correspondante (on rappelle que, par construction, dans la table des sections en entrée,
            // les index sont en ordre croissant, mais ne correspondent pas à l'indice du tableau mais
            // sont stockés dans la structure.
            k = 0;
            while (sections.index_section[k] != sections.Mtable.id_section_merge[symtab2.entries[indice2].st_shndx])
              k++;

            // On ajoute l'offset pour corriger l'emplacement dans les données pointées.
            fusionsym.entries[fusionsym.nbsymbols].st_value += sections.sections[k].offset;
          }

          // Si il ne s'agit pas d'une partie fusionnée, on donne juste son indice dans le nouveau fichier.
          else
          {
            // On retrouve d'abord de quelle section il s'agit.
            k = 0;
            while (sections.sections[k].oldindexfich2 != indice2)
              k++;

            // On donne son nouvel index dans la structure de sortie.
            fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.index_section[k];
          }
        }
      }
    }

    // On rajoute l'entrée dans la table des chaines (si le nom n'est pas vide).
    if (name[0] == '\0') { fusionsym.entries[fusionsym.nbsymbols].st_name = 0; }
    else
    {
      fusionsym.entries[fusionsym.nbsymbols].st_name = fusionsym.stringtable.endoftable;
      j = 0;
      while (name[j] != '\0')
      {
        fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable + 1);
        fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = name[j];
        fusionsym.stringtable.endoftable++;
        j++;
      }
      // On rajoute la fin de mot ('\0').
      fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable + 1);
      fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = '\0';
      fusionsym.stringtable.endoftable++;
    }

    // On incrémente le nombre de symboles.
    fusionsym.nbsymbols++;
  }

  // On parcourt désormais les symboles du fichier 2 désormais.
  for (i = 0; i < symtab2.nbsymbols; i++)
  {
    // Condition importante : on ne traite pas deux fois les symboles !
    if (avancement[i] == 0)
    {
      // On alloue de la place dans la structure
      fusionsym.entries = realloc(fusionsym.entries, (fusionsym.nbsymbols + 1) * sizeof(Elf32_Sym));

      // Ici, au vu des cas déjà traités, que la variable soit globale ou locale, elle ne figure que dans le fichier 2,
      // donc on recopie les entrées entières dans la structure et on doit traiter les offsets.

      // Récuperation du contenu.
      fusionsym.entries[fusionsym.nbsymbols] = symtab2.entries[i];

      // Correction des offsets si il s'agit d'une partie fusionnée,
      // on ne le fait pas si c'est de type ABS ou UNDEF.
      if (symtab2.entries[i].st_shndx != SHN_UNDEF && symtab2.entries[i].st_shndx != SHN_ABS)
      {
        if (sections.Mtable.id_section_merge[symtab2.entries[i].st_shndx] != -1)
        {
          // On pointe vers le nouvel index de la table des sections.
          fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.Mtable.id_section_merge[symtab2.entries[i].st_shndx];
          // On retrouve la section correspondante (on rappelle que, par construction, dans la table des sections en entrée,
          // les index sont en ordre croissant, mais ne correspondent pas à l'indice du tableau mais
          // sont stockés dans la structure.
          k = 0;
          while (sections.index_section[k] != sections.Mtable.id_section_merge[symtab2.entries[i].st_shndx])
            k++;

          // On ajoute l'offset pour corriger l'emplacement dans les données pointées.
          fusionsym.entries[fusionsym.nbsymbols].st_value += sections.sections[k].offset;
        }
      }

      // Si il ne s'agit pas d'une partie fusionnée, on donne juste son indice dans le nouveau fichier.
      else
      {
        // On vérifie si ce n'est pas un type spécial d'affectation.
        if (symtab2.entries[i].st_shndx != SHN_UNDEF && symtab2.entries[i].st_shndx != SHN_ABS)
        {
          // On retrouve d'abord de quelle section il s'agit
          k = 0;
          while (sections.sections[k].oldindexfich2 != i)
            k++;

          // On donne son nouvel index dans la structure de sortie.
          fusionsym.entries[fusionsym.nbsymbols].st_shndx = sections.index_section[k];
        }
      }

      // On récupère le nom pour la table des chaines.
      j = 0;
      name[j] = symtab2.stringtable.strings[symtab2.entries[i].st_name + j];
      while (namefich2[j] != '\0')
      {
        j++;
        name[j] = symtab2.stringtable.strings[symtab2.entries[i].st_name + j];
      }
      // On rajoute l'entrée dans la table des chaines (si le nom n'est pas vide).
      if (name[0] == '\0')
      {
        fusionsym.entries[fusionsym.nbsymbols].st_name = 0;
      }
      else
      {
        fusionsym.entries[fusionsym.nbsymbols].st_name = fusionsym.stringtable.endoftable;
        j = 0;
        while (name[j] != '\0')
        {
          fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable + 1);
          fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = name[j];
          fusionsym.stringtable.endoftable++;
          j++;
        }
        // On rajoute la fin de mot ('\0').
        fusionsym.stringtable.strings = realloc(fusionsym.stringtable.strings, fusionsym.stringtable.endoftable + 1);
        fusionsym.stringtable.strings[fusionsym.stringtable.endoftable] = '\0';
        fusionsym.stringtable.endoftable++;
      }

      // On incrémente le nombre de symboles.
      fusionsym.nbsymbols++;
    }
  }
  return fusionsym;
}
