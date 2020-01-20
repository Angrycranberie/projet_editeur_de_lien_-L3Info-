#ifndef _FUSIONSYMBOLE_H
#define	_FUSIONSYMBOLE_H
#include <elf.h>
#include "progbits.h"
#include "symboltable.h"
#include "values.h"

/* Fonction fusionnant les tables de symboles des deux fichiers ELF (64 bits) dont les tables de symboles
 sont passées en argument, ainsi que la table des sections progbits fusionnées.
 Renvoie la table des symboles fusionnée et la table des chaines associée (dans une structure symbol_table64). */
symbol_table_64 fusion_symbol_tables_64(Table_sections sections, symbol_table_64 symtab1, symbol_table_64 symtab2);


#endif