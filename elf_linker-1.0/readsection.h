#ifndef _READSECTION_H
#define _READSECTION_H 1
#include <elf.h>

void readsection_id_64(int id, FILE *f, Elf64_Ehdr header);

void readsection_name_64(char *name, FILE *f, Elf64_Ehdr header);

void readsection_id_32(int id, FILE *f, Elf32_Ehdr header);

void readsection_name_32(char *name, FILE *f, Elf32_Ehdr header);

#endif