ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS=-Wall -g
AM_CFLAGS+=-D DEBUG
AM_CFLAGS+=-D WARNING
# Décommenter cette ligne pour simuler un comportement en big endian
#AM_CFLAGS+=-D BIG_ENDIAN_SIMULATOR
# Uncomment if performance when running with -DDEBUG is an issue
# Warning, if uncommented, issuing calls to debug functions during options
# parsing might result in debug flag incorrectly set to 0 for some files
#AM_CFLAGS+=-D CACHE_DEBUG_FLAG

LDADD=-lpthread


SUBDIRS=
SUBDIRS+=. Examples_loader

bin_PROGRAMS=
# bin_PROGRAMS+=ARM_runner_example
bin_PROGRAMS+=readelf_header
bin_PROGRAMS+=readelf_section_list
bin_PROGRAMS+=readelf_section_content
bin_PROGRAMS+=readelf_symbols_table
bin_PROGRAMS+=readelf_relocation
bin_PROGRAMS+=etape6
bin_PROGRAMS+=etape7

DEBUG=debug.h debug.c
UTIL=util.h util.c
HEADER=header.h header.c
COMMON=$(DEBUG) $(UTIL) $(HEADER)
READSECTION=readsection.h readsection.c
TABLESECTION=tablesection.h tablesection.c
SYMBOLTABLE=symboltable.h symboltable.c
RELOCA=reloca.h reloca.c
PROGBITS=progbits.h progbits.c
FUSIONSYMBOLE=fusionsymbole.h fusionsymbole.c

EXTRA_DIST=filtre.pl
# SIMULATOR_INTERFACE=csapp.h csapp.c scanner.h scanner.l gdb_protocol.h gdb_protocol.c arm_simulator_interface.h arm_simulator_interface.c

# ARM_runner_example_SOURCES=$(COMMON) $(SIMULATOR_INTERFACE) ARM_runner_example.c
readelf_header_SOURCES=$(COMMON) readelf_header.c
readelf_section_list_SOURCES=$(COMMON) $(TABLESECTION) readelf_section_list.c
readelf_section_content_SOURCES=$(COMMON) $(READSECTION) $(TABLESECTION) readelf_section_content.c
readelf_symbols_table_SOURCES=$(COMMON) $(SYMBOLTABLE) $(TABLESECTION) readelf_symbols_table.c
readelf_relocation_SOURCES=$(COMMON) $(TABLESECTION) $(RELOCA) readelf_relocation.c
etape6_SOURCES=$(COMMON) $(PROGBITS) $(TABLESECTION) etape6.c
etape7_SOURCES=$(COMMON) $(PROGBITS) $(TABLESECTION) $(SYMBOLTABLE) $(FUSIONSYMBOLE) etape7.c

EXTRA_DIST+=sample_run.sh

