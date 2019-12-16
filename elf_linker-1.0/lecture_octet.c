#include <stdio.h>
#include <stdlib.h>
#include "lecture_octet.h"

int start_read(char *nom_fich){
	fichier = fopen(nom_fich,"r");
	if(fichier == NULL){
		return 0;
	} else{
		fscanf(fichier,"%c",&byte);
		return 1;
	}
}

char current_byte(){
	return byte;
}

int next(){
	if(!feof(fichier)){
		fscanf(fichier,"%c",&byte);
		return 1;
	}else {
		return 0;
	}
}

int End_Of_File(){
	return feof(fichier);
}

void end_read(){
	fclose(fichier);
}

int main(int argc, char *argv[]){
	if(argc != 2){
		fprintf(stderr,"usage : lecture_octet fichier");
		return 1;
	}
	if(!start_read(argv[1])){
		fprintf(stderr,"file not found");
		return 2;
	}
	while(!End_Of_File()){
		printf("%c",current_byte());
		next();
	}
	end_read();
	return 0;
}
