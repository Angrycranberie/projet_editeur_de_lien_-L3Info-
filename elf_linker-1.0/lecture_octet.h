#ifndef __LECTURE_OCTET_H__
#define __LECTURE_OCTET_H__

FILE *fichier;
char byte;


// ouvre le fichier donné en argument et initialise la lecture
//du premier octet
int start_read(char *nom_fich);


// retourne l'octet courant
char current_byte();

// avance la lecture du fichier
int next();

// renvoi 1 si le fichier a été lu entierement
int End_Of_File();

// cloture la lecture du fichier
void end_read();

#endif



