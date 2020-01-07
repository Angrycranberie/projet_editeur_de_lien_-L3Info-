#ifndef _FUSIONSYMBOLE_H
#define	_FUSIONSYMBOLE_H
#include <elf.h>
#include "progbits.h"
#include "symboltable.h"
#include "values.h"




symbol_table_64 fusion_symbol_tables_64(Table_sections sections, symbol_table_64 symtab1, symbol_table_64 symtab2); // Calcule la fusion des tables de symboles des deux fichiers elf dont les tables de symboles sont passees
                                                                                                                    // en argument, ainsi que la tables des sections progbits fusionnees. Renvoie la table des symboles fusionnees,
                                                                                                                    // ainsi que la table des chaines associee (dans une structure symbol_table64).
                                                                                                                   
// Versions 32 bits //


symbol_table_32 fusion_symbol_tables_32(Table_sections sections, symbol_table_32 symtab1, symbol_table_32 symtab2); 
                              


#endif