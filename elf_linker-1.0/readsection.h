#ifndef _READSECTION_H
#define _READSECTION_H 1
#include <elf.h>
#include "tablesection.h"

void readsection_id(int id, FILE *f, section_list *sections);

void readsection_name(char *name, FILE *f,section_list *sections);

#endif