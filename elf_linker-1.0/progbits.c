#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include "progbits.h"

// Donne le nom de section numero id dans le fichier elf et place sa valeur dans nom. On assume que suffisamment de place y est alloué
// Attention: cela change la position du descripteur f
void getname_section_64(FILE* f, Elf64_Ehdr header, int id, char* nom){
  Elf64_Off offset;
  Elf64_Word sh_name;
  // On recupère l'index du nom de la section
  fseek(f, header.e_shoff + id * header.e_shentsize , SEEK_SET);
  fread(&sh_name,sizeof(Elf64_Word),1,f);
  // On récupère le nom de la section
  fseek(f, header.e_shoff + header.e_shstrndx * header.e_shentsize + 2 * sizeof(Elf64_Word) + sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_SET);
  fread(&offset,sizeof(Elf64_Off),1,f);
  fseek(f, offset +  sh_name, SEEK_SET);  
  int j=0;
  fread(&nom[j],sizeof(unsigned char),1,f);
  while(nom[j]!='\0'){
    j++;
  fread(&nom[j],sizeof(unsigned char),1,f);
    }
}

// Initialise la table de Merge décrite dans progbits.h avec des -1
Merge_table_progbits initmerge_64(FILE* f2, Elf64_Ehdr header1, Elf64_Ehdr header2){
  Merge_table_progbits Mtable;
  Mtable.nbmerge = 0;
  for (int i = 0; i< header2.e_shnum;i++){
    Mtable.id_section_merge[i] = -1;
  }
  return Mtable;

}

// Cherche les sections progbits dans le fichier 2, puis regarde si il existe une section de même nom dans le fichier 1. Renvoie le nombre de merge effectués, et desquelles il s'agit.
Merge_table_progbits search_progbits_f2_64(FILE* f1, FILE* f2, Elf64_Ehdr header1, Elf64_Ehdr header2){
  int i;
  int j;
  Elf64_Word type1;
  Elf64_Word type2;
  char nom1[100];
  char nom2[100];
  int test;
  
  // On crée la table 
  Merge_table_progbits Mtable = initmerge_64(f2,header1,header2);
  
  // On parcourt les sections du fichier 2
  for (i=0;i<header2.e_shnum;i=i+1){
    // On verifie si la section actuelle du fichier 2 est de type progbits
    fseek(f2, header2.e_shoff + i * header2.e_shentsize + sizeof(Elf64_Word) , SEEK_SET);
    fread(&type2, sizeof(Elf64_Word),1,f2);
    
    // Si la section actuelle du fichier 2 est de type progbits, on cherche son nom
    if (type2==SHT_PROGBITS){
       getname_section_64(f2,header2,i,nom2);
       printf("On trouve la section %s dans le fichier 2 qui est de type progbits\n", nom2);
       // On regarde alors si son nom correspond à une section du fichier 1 de type progbits en parcourant les sections du fichier 1
       // test verifie si on a trouvé une section de même nom.
       test = 0;
       for (j=0;j<header1.e_shnum && test == 0;j=j+1){
       
         // On verifie si la section actuelle du fichier 1 est de type progbits
         fseek(f1, header1.e_shoff + j * header1.e_shentsize + sizeof(Elf64_Word) , SEEK_SET);
         fread(&type1, sizeof(Elf64_Word),1,f1);
         // Si la section actuelle du fichier 1 est de type progbits, on cherche son nom
         if (type1==SHT_PROGBITS){
           getname_section_64(f1,header1,j,nom1);
           
           // Si son nom est le même que celui de la section actuelle du fichier2, on doit merge
           if (strcmp(nom1,nom2)==0){
             printf("Il faut merge les sections %s dans le numero %d\n", nom1, j);
             Mtable.id_section_merge[i] = j;
             Mtable.nbmerge++;
             test=1;
           }
         }
         
       }
       if (test == 0) printf("Il faut ajouter la section %s dans le numero %d\n", nom2, header1.e_shnum + i - Mtable.nbmerge);
    }

  }
  return Mtable;
}


// Vérifie si une fusion (de progbits) est à faire sur cette section. Retourne le numero avec lequel fusionner si il y a une fusion, -1 sinon
int verif_fusion_progbits_64 (int id, Merge_table_progbits Mtable, Elf64_Ehdr header){
  for (int i = 0; i < header.e_shnum; i++){
    if (Mtable.id_section_merge[i] == id) return i;
  }
  return -1;
}


// initialise la section en allouant la bonne taille (pour pouvoir y ranger toutes les donnees)
Section_progbits init_section_size(int taille){
  Section_progbits Section;
  Section.taille = taille;
  Section.content = (unsigned char*) malloc(taille * sizeof(unsigned char*));
  
  // On initialise l'offset à 0, on le mettra à jour en cas de fusion;
  Section.offset = 0;
  return Section;
}






// Effectue la fusion des sections PROGBITS des deux fichiers
Table_sections get_merged_progbits_64 (FILE* f1, FILE* f2, Elf64_Ehdr header1, Elf64_Ehdr header2, Merge_table_progbits Mtable){
  int i;
  int verif;
  Elf64_Xword size1;
  Elf64_Xword size2;
  Elf64_Word type;
  unsigned char temp;
  int index;  // header1.e_shnum + header2.e_shnum - Mtable.nbmerge -2
  Elf64_Word name;
  
  
  // Structure permettant de stocker les informations utiles pour l'etape 6,  c'est a dire les progbits fusionnes et  
  Table_sections Tablesec;
  Tablesec.nbSections = 0;
  
  // On stocke Mtable (pour le retourner avec le reste) dans Tablesec
  
  
  //Stocke les offsets des entrees de la table de sections dans leurs fichiers respectifs
  Elf64_Off offset1;
  Elf64_Off offset2;
  
  // On commence par les progbits du fichier 1, on saute donc le 0 qui n'en est jamais 1
  for (i = 1; i<header1.e_shnum; i++){
    // On recupere le nom de la section
    fseek(f1, header1.e_shoff + i * header1.e_shentsize , SEEK_SET);
    fread(&name,sizeof(Elf64_Word),1,f1);
    fread(&type,sizeof(Elf64_Word),1,f1);
    if (type == SHT_PROGBITS){
        // On saute les champs qui ne nous interessent pas
      fseek(f1, sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_CUR);
      fread(&offset1,sizeof(Elf64_Off),1,f1);
      fread(&size1,sizeof(Elf64_Xword),1,f1);
      
      
      // On vérifie si on a une fusion à faire
      verif = verif_fusion_progbits_64(i, Mtable, header2);
      // Cas de la fusion 
      if (verif != -1 ){  
        Tablesec.fusion[Tablesec.nbSections] = 1;
        // On se déplace dans le fichier 2 pour recuperer la taille de la section correspondante
        fseek(f2, header2.e_shoff + verif * header2.e_shentsize + sizeof(Elf64_Addr) + sizeof(Elf64_Xword)  + sizeof(Elf64_Word) * 2 , SEEK_SET);
        
        // On lit le size pour le rajouter (l'offset est gardé pour plus tard, voir l'impression du contenu)
        fread(&offset2,sizeof(Elf64_Off),1,f2);
        fread(&size2,sizeof(Elf64_Xword),1,f2); 
        
      } 
      // On note sinon l'absence de fusion
      else   Tablesec.fusion[Tablesec.nbSections] = 0;
      
      // On met à jour notre structure avec les informations apportees
      Tablesec.index_section[Tablesec.nbSections] = i;  
      if (verif == -1 ){
        Tablesec.sections[Tablesec.nbSections] = init_section_size(size1);
      }
      else {
        Tablesec.sections[Tablesec.nbSections] = init_section_size(size1 + size2);
        Tablesec.sections[Tablesec.nbSections].offset = size1; 
      }
      getname_section_64(f1, header1, i, Tablesec.sections[Tablesec.nbSections].name);
      
      
      // On imprime maintenant le contenu des sections:
      // On commence par se placer correctement dans le fichier 1
      fseek(f1, offset1, SEEK_SET);
      
      // On écrit le contenu dans la structure
      fread(Tablesec.sections[Tablesec.nbSections].content,sizeof(unsigned char),size1,f1);

      
      // Si on fait une fusion, on concatene la section contenue dans le fichier 2
      if (verif != -1 ){
        fseek(f2, offset2, SEEK_SET);
        
        fread(Tablesec.sections[Tablesec.nbSections].content + Tablesec.sections[Tablesec.nbSections].offset,sizeof(unsigned char),size2,f2);
      }
      
      // On incremente la taille du tableau des sections progbits
      Tablesec.nbSections++;
    }
    
  }
  
  // On doit maintenant rajouter les sections de la partie 2
  // On maintient a jour l'index du prochain element a rajouter
  index  = header1.e_shnum;
  // On ignore le 0, qui est vide
  for (i = 1; i<header2.e_shnum; i++){
    if (Mtable.id_section_merge[i]==-1){
      
        // On se place puis on recupere les donnees du header de la fonction.
      fseek(f2, header2.e_shoff + i * header2.e_shentsize , SEEK_SET);
      fread(&name,sizeof(Elf64_Word),1,f2);
      fread(&type,sizeof(Elf64_Word),1,f2);
      if (type == SHT_PROGBITS){
        // On saute les champs qui ne nous interessent pas
        fseek(f2, sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_CUR);
        fread(&offset2,sizeof(Elf64_Off),1,f2);
        fread(&size2,sizeof(Elf64_Xword),1,f2);
        
        // On met à jour notre structure avec les informations apportees
        Tablesec.index_section[Tablesec.nbSections] = index;  
        Tablesec.fusion[Tablesec.nbSections] = 0;
        Tablesec.sections[Tablesec.nbSections] = init_section_size(size2);
        getname_section_64(f2, header2, i, Tablesec.sections[Tablesec.nbSections].name);
        
        // On imprime maintenant le contenu de la section:
        // On commence par se placer correctement dans le fichier 2
        fseek(f2, offset2, SEEK_SET);
        
        // On écrit le contenu de la section dans la structure
        fread(Tablesec.sections[Tablesec.nbSections].content,sizeof(unsigned char),size2,f2);
          
        // On incremente l'index
        index ++;
        Tablesec.nbSections++;
      }
    }
  }
  return Tablesec;
}


void affiche_table_section(Table_sections Tablesec){
  unsigned char temp;
  
  printf("Liste des sections de type PROGBITS dans le fichier final :\n");
  // On affiche chacune des sections individuellement
  for (int i = 0; i <Tablesec.nbSections; i++){
    // Permet de reconnaitre si il y a eu fusion
    if (Tablesec.fusion[i] == 1) {
      // Cas de la fusion
      printf("  On a fusionne les deux parties nommees %s, a l'index %d\n",Tablesec.sections[i].name, Tablesec.index_section[i]);
      printf("  La taille de la section est %016x\n  L'offset ou on trouve les elements du fichier2 est %016x\n", Tablesec.sections[i].taille, Tablesec.sections[i].offset);
      
      
    
    }
    else {
      printf("  La partie %s est stockee a l'index %d\n",Tablesec.sections[i].name, Tablesec.index_section[i]);
      printf("  La taille de la section est %016x\n", Tablesec.sections[i].taille);
    }
    
    printf("  Contenu hexadecimal de la section :\n");
    for (int j = 0; j < Tablesec.sections[i].taille; j++){
      if (j%16==0) printf("    ");
      temp = Tablesec.sections[i].content[j];
      printf("%02x",temp);
      if (j%4==3) printf(" ");
      if (j%16==15) printf("\n");
    }
    printf("\n");
  
  
  }


}


                                    //                                          //
                                    //                                          //
                                    //                                          //
                                    // Versions 32 bits des fonctions ci-dessus // 
                                    //                                          //
                                    //                                          //
                                    //                                          //


// Donne le nom de section numero id dans le fichier elf et place sa valeur dans nom. On assume que suffisamment de place y est alloué
// Attention: cela change la position du descripteur f
void getname_section_32(FILE* f, Elf32_Ehdr header, int id, char* nom){
  Elf32_Off offset;
  Elf32_Word sh_name;
  // On recupère l'index du nom de la section
  fseek(f, header.e_shoff + id * header.e_shentsize , SEEK_SET);
  fread(&sh_name,sizeof(Elf32_Word),1,f);
  // On récupère le nom de la section
  fseek(f, header.e_shoff + header.e_shstrndx * header.e_shentsize + 2 * sizeof(Elf32_Word) + sizeof(Elf32_Word) + sizeof(Elf32_Addr), SEEK_SET);
  fread(&offset,sizeof(Elf32_Off),1,f);
  fseek(f, offset +  sh_name, SEEK_SET);  
  int j=0;
  fread(&nom[j],sizeof(unsigned char),1,f);
  while(nom[j]!='\0'){
    j++;
  fread(&nom[j],sizeof(unsigned char),1,f);
    }
}

// Initialise la table de Merge décrite dans progbits.h avec des -1
Merge_table_progbits initmerge_32(FILE* f2, Elf32_Ehdr header1, Elf32_Ehdr header2){
  Merge_table_progbits Mtable;
  Mtable.nbmerge = 0;
  for (int i = 0; i< header2.e_shnum;i++){
    Mtable.id_section_merge[i] = -1;
  }
  return Mtable;

}

// Cherche les sections progbits dans le fichier 2, puis regarde si il existe une section de même nom dans le fichier 1. Renvoie le nombre de merge effectués, et desquelles il s'agit.
Merge_table_progbits search_progbits_f2_32(FILE* f1, FILE* f2, Elf32_Ehdr header1, Elf32_Ehdr header2){
  int i;
  int j;
  Elf32_Word type1;
  Elf32_Word type2;
  char nom1[100];
  char nom2[100];
  int test;
  
  // On crée la table 
  Merge_table_progbits Mtable = initmerge_32(f2,header1,header2);
  
  // On parcourt les sections du fichier 2
  for (i=0;i<header2.e_shnum;i=i+1){
    // On verifie si la section actuelle du fichier 2 est de type progbits
    fseek(f2, header2.e_shoff + i * header2.e_shentsize + sizeof(Elf32_Word) , SEEK_SET);
    fread(&type2, sizeof(Elf32_Word),1,f2);
    
    // Si la section actuelle du fichier 2 est de type progbits, on cherche son nom
    if (type2==SHT_PROGBITS){
       getname_section_32(f2,header2,i,nom2);
       printf("On trouve la section %s dans le fichier 2 qui est de type progbits\n", nom2);
       // On regarde alors si son nom correspond à une section du fichier 1 de type progbits en parcourant les sections du fichier 1
       // test verifie si on a trouvé une section de même nom.
       test = 0;
       for (j=0;j<header1.e_shnum && test == 0;j=j+1){
       
         // On verifie si la section actuelle du fichier 1 est de type progbits
         fseek(f1, header1.e_shoff + j * header1.e_shentsize + sizeof(Elf32_Word) , SEEK_SET);
         fread(&type1, sizeof(Elf32_Word),1,f1);
         // Si la section actuelle du fichier 1 est de type progbits, on cherche son nom
         if (type1==SHT_PROGBITS){
           getname_section_32(f1,header1,j,nom1);
           
           // Si son nom est le même que celui de la section actuelle du fichier2, on doit merge
           if (strcmp(nom1,nom2)==0){
             printf("Il faut merge les sections %s dans le numero %d\n", nom1, j);
             Mtable.id_section_merge[i] = j;
             Mtable.nbmerge++;
             test=1;
           }
         }
         
       }
       if (test == 0) printf("Il faut ajouter la section %s dans le numero %d\n", nom2, header1.e_shnum + i - Mtable.nbmerge);
    }

  }
  return Mtable;
}


// Vérifie si une fusion (de progbits) est à faire sur cette section. Retourne le numero avec lequel fusionner si il y a une fusion, -1 sinon
int verif_fusion_progbits_32 (int id, Merge_table_progbits Mtable, Elf32_Ehdr header){
  for (int i = 0; i < header.e_shnum; i++){
    if (Mtable.id_section_merge[i] == id) return i;
  }
  return -1;
}






// Effectue la fusion des sections PROGBITS des deux fichiers
Table_sections get_merged_progbits_32 (FILE* f1, FILE* f2, Elf32_Ehdr header1, Elf32_Ehdr header2, Merge_table_progbits Mtable){
  int i;
  int verif;
  Elf32_Word size1;
  Elf32_Word size2;
  Elf32_Word type;
  unsigned char temp;
  int index;  // header1.e_shnum + header2.e_shnum - Mtable.nbmerge -2
  Elf32_Word name;
  
  
  // Structure permettant de stocker les informations utiles pour l'etape 6,  c'est a dire les progbits fusionnes et  
  Table_sections Tablesec;
  Tablesec.nbSections = 0;
  
  // On stocke Mtable (pour le retourner avec le reste) dans Tablesec
  
  
  //Stocke les offsets des entrees de la table de sections dans leurs fichiers respectifs
  Elf32_Off offset1;
  Elf32_Off offset2;
  
  // On commence par les progbits du fichier 1, on saute donc le 0 qui n'en est jamais 1
  for (i = 1; i<header1.e_shnum; i++){
    // On recupere le nom de la section
    fseek(f1, header1.e_shoff + i * header1.e_shentsize , SEEK_SET);
    fread(&name,sizeof(Elf32_Word),1,f1);
    fread(&type,sizeof(Elf32_Word),1,f1);
    if (type == SHT_PROGBITS){
        // On saute les champs qui ne nous interessent pas
      fseek(f1, sizeof(Elf32_Word) + sizeof(Elf32_Addr), SEEK_CUR);
      fread(&offset1,sizeof(Elf32_Off),1,f1);
      fread(&size1,sizeof(Elf32_Word),1,f1);
      
      
      // On vérifie si on a une fusion à faire
      verif = verif_fusion_progbits_32(i, Mtable, header2);
      // Cas de la fusion 
      if (verif != -1 ){  
        Tablesec.fusion[Tablesec.nbSections] = 1;
        // On se déplace dans le fichier 2 pour recuperer la taille de la section correspondante
        fseek(f2, header2.e_shoff + verif * header2.e_shentsize + sizeof(Elf32_Addr) + sizeof(Elf32_Word)  + sizeof(Elf32_Word) * 2 , SEEK_SET);
        
        // On lit le size pour le rajouter (l'offset est gardé pour plus tard, voir l'impression du contenu)
        fread(&offset2,sizeof(Elf32_Off),1,f2);
        fread(&size2,sizeof(Elf32_Word),1,f2); 
        
      } 
      // On note sinon l'absence de fusion
      else   Tablesec.fusion[Tablesec.nbSections] = 0;
      
      // On met à jour notre structure avec les informations apportees
      Tablesec.index_section[Tablesec.nbSections] = i;  
      if (verif == -1 ){
        Tablesec.sections[Tablesec.nbSections] = init_section_size(size1);
      }
      else {
        Tablesec.sections[Tablesec.nbSections] = init_section_size(size1 + size2);
        Tablesec.sections[Tablesec.nbSections].offset = size1; 
      }
      getname_section_32(f1, header1, i, Tablesec.sections[Tablesec.nbSections].name);
      
      
      // On imprime maintenant le contenu des sections:
      // On commence par se placer correctement dans le fichier 1
      fseek(f1, offset1, SEEK_SET);
      
      // On écrit le contenu dans la structure
      fread(Tablesec.sections[Tablesec.nbSections].content,sizeof(unsigned char),size1,f1);

      
      // Si on fait une fusion, on concatene la section contenue dans le fichier 2
      if (verif != -1 ){
        fseek(f2, offset2, SEEK_SET);
        
        fread(Tablesec.sections[Tablesec.nbSections].content + Tablesec.sections[Tablesec.nbSections].offset,sizeof(unsigned char),size2,f2);
      }
      
      // On incremente la taille du tableau des sections progbits
      Tablesec.nbSections++;
    }
    
  }
  
  // On doit maintenant rajouter les sections de la partie 2
  // On maintient a jour l'index du prochain element a rajouter
  index  = header1.e_shnum;
  // On ignore le 0, qui est vide
  for (i = 1; i<header2.e_shnum; i++){
    if (Mtable.id_section_merge[i]==-1){
      
        // On se place puis on recupere les donnees du header de la fonction.
      fseek(f2, header2.e_shoff + i * header2.e_shentsize , SEEK_SET);
      fread(&name,sizeof(Elf32_Word),1,f2);
      fread(&type,sizeof(Elf32_Word),1,f2);
      if (type == SHT_PROGBITS){
        // On saute les champs qui ne nous interessent pas
        fseek(f2, sizeof(Elf32_Word) + sizeof(Elf32_Addr), SEEK_CUR);
        fread(&offset2,sizeof(Elf32_Off),1,f2);
        fread(&size2,sizeof(Elf32_Word),1,f2);
        
        // On met à jour notre structure avec les informations apportees
        Tablesec.index_section[Tablesec.nbSections] = index;  
        Tablesec.fusion[Tablesec.nbSections] = 0;
        Tablesec.sections[Tablesec.nbSections] = init_section_size(size2);
        getname_section_32(f2, header2, i, Tablesec.sections[Tablesec.nbSections].name);
        
        // On imprime maintenant le contenu de la section:
        // On commence par se placer correctement dans le fichier 2
        fseek(f2, offset2, SEEK_SET);
        
        // On écrit le contenu de la section dans la structure
        fread(Tablesec.sections[Tablesec.nbSections].content,sizeof(unsigned char),size2,f2);
          
        // On incremente l'index
        index ++;
        Tablesec.nbSections++;
      }
    }
  }
  return Tablesec;
}

















