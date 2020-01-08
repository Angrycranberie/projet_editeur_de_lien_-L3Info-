#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include "tablesection.h"
#include "util.h"


section_list *read_tablesection(FILE *f, Elf64_Ehdr header)
{
	Elf64_Off offset;
	int i;

	// initialisationde la structure qui regroupe toutes les informations de chaque section du fichier et leur nom associé
	section_list *sections = (section_list *)malloc(sizeof(section_list));
	sections->sec_list = (Elf64_Shdr *)malloc(sizeof(Elf64_Shdr) * header.e_shnum);
	sections->names = (section_name *)malloc(sizeof(section_name) * header.e_shnum);
	sections->nb_section = header.e_shnum;

	// version 32(0) ou 64 bit(1) 
    int bits_version = header.e_ident[4] == ELFCLASS64;
    // indique si l'endianess est différent entre la machine et le fichier 1 -> oui 0 -> non
    int diff_endianess = (is_big_endian() != (int)(header.e_ident[EI_DATA] == ELFDATA2MSB));
	printf("diff_endianess %d\n",diff_endianess);

	if (header.e_shoff == 0)
		return NULL;

	//On se place au début de la table des sections
	//On fait une entrée pour chaque section
	for (i = 0; i < header.e_shnum; i++){
		// Se place à l'emplacement du fichier donné par l'offset et le numero de la section dans
		// la table.
		fseek(f, header.e_shoff + i * header.e_shentsize, SEEK_SET);

		

		// On regarde le header de la section
		fread(&(sections->sec_list[i].sh_name), sizeof(Elf64_Word), 1, f);
		fread(&sections->sec_list[i].sh_type, sizeof(Elf64_Word), 1, f);
		bits_version ? fread(&sections->sec_list[i].sh_flags, sizeof(Elf64_Xword), 1, f) : fread(&sections->sec_list[i].sh_flags, sizeof(Elf32_Word), 1, f),sections->sec_list[i].sh_flags &= 0xFFFFFFFF;
		bits_version ? fread(&sections->sec_list[i].sh_addr, sizeof(Elf64_Addr), 1, f) : fread(&sections->sec_list[i].sh_addr, sizeof(Elf32_Addr), 1, f),sections->sec_list[i].sh_addr &= 0xFFFFFFFF;
		bits_version ? fread(&sections->sec_list[i].sh_offset, sizeof(Elf64_Off), 1, f) : fread(&sections->sec_list[i].sh_offset, sizeof(Elf32_Off), 1, f),sections->sec_list[i].sh_offset &= 0xFFFFFFFF;
		bits_version ? fread(&sections->sec_list[i].sh_size, sizeof(Elf64_Xword), 1, f) : fread(&sections->sec_list[i].sh_size, sizeof(Elf32_Word), 1, f),sections->sec_list[i].sh_size &= 0xFFFFFFFF;
		fread(&sections->sec_list[i].sh_link, sizeof(Elf64_Word), 1, f);
		fread(&sections->sec_list[i].sh_info, sizeof(Elf64_Word), 1, f);
		bits_version ? fread(&sections->sec_list[i].sh_addralign, sizeof(Elf64_Xword), 1, f) : fread(&sections->sec_list[i].sh_addralign, sizeof(Elf32_Word), 1, f),sections->sec_list[i].sh_addralign &= 0xFFFFFFFF;
		bits_version ? fread(&sections->sec_list[i].sh_entsize, sizeof(Elf64_Xword), 1, f) : fread(&sections->sec_list[i].sh_entsize, sizeof(Elf32_Word), 1, f),sections->sec_list[i].sh_entsize &= 0xFFFFFFFF;

		if(diff_endianess){
			sections->sec_list[i].sh_name =   reverse_4(sections->sec_list[i].sh_name);
			sections->sec_list[i].sh_type =   reverse_4(sections->sec_list[i].sh_type);
			sections->sec_list[i].sh_flags =  reverse_8(sections->sec_list[i].sh_flags);
			sections->sec_list[i].sh_addr =   reverse_8(sections->sec_list[i].sh_addr);
			sections->sec_list[i].sh_offset = reverse_8(sections->sec_list[i].sh_offset);
			sections->sec_list[i].sh_size =   reverse_8(sections->sec_list[i].sh_size);
			sections->sec_list[i].sh_link =   reverse_4(sections->sec_list[i].sh_link);
			sections->sec_list[i].sh_info =   reverse_4(sections->sec_list[i].sh_info);
			sections->sec_list[i].sh_addralign = reverse_8(sections->sec_list[i].sh_addralign);
			sections->sec_list[i].sh_entsize =   reverse_8(sections->sec_list[i].sh_entsize);
		}

		// On regarde le nom de la section dans la table des chaînes
		bits_version ? fseek(f, header.e_shoff + header.e_shstrndx * header.e_shentsize + 2 * sizeof(Elf64_Word) + sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_SET) : 
		fseek(f, (Elf32_Off)header.e_shoff + header.e_shstrndx * header.e_shentsize + 3 * sizeof(Elf32_Word) + sizeof(Elf32_Addr), SEEK_SET);
		bits_version ? fread(&offset, sizeof(Elf64_Off), 1, f) : fread(&offset, sizeof(Elf32_Off), 1, f);
		fseek(f, offset + sections->sec_list[i].sh_name, SEEK_SET);
		
		int j = 0;
		fread(&sections->names[i].nom[j], sizeof(unsigned char), 1, f);
		while (sections->names[i].nom[j] != '\0'){
			j++;
			fread(&sections->names[i].nom[j], sizeof(unsigned char), 1, f);
		}

		
	}
	return sections;
}


void print_sectionTable(section_list *sections){
	for (int i = 0; i < sections->nb_section; i++){

		// On affiche les informations sur la section
		printf("Numero de section:             %d\n", i);
		printf("Addresse de section:           %016lx\n", sections->sec_list[i].sh_addr);
		printf("Nom de la section:             %s\n", sections->names[i].nom);
		printf("Taille de la section:          %016lx (bytes)\n", sections->sec_list[i].sh_size);

		// affichage du type de section
		switch (sections->sec_list[i].sh_type)
		{

		case SHT_NULL:
			printf("Type de section:               Section header table entry unused \n");
			break;

		case SHT_PROGBITS:
			printf("Type de section:               Program data \n");
			break;

		case SHT_SYMTAB:
			printf("Type de section:               Symbol table \n");
			break;

		case SHT_STRTAB:
			printf("Type de section:               String table \n");
			break;

		case SHT_RELA:
			printf("Type de section:               Relocation entries with addends \n");
			break;

		case SHT_HASH:
			printf("Type de section:               Symbol hash table \n");
			break;

		case SHT_DYNAMIC:
			printf("Type de section:               Dynamic linking information \n");
			break;

		case SHT_NOTE:
			printf("Type de section:               Notes \n");
			break;

		case SHT_NOBITS:
			printf("Type de section:               Program space with no data (bss) \n");
			break;

		case SHT_REL:
			printf("Type de section:               Relocation entries, no addends \n");
			break;

		case SHT_SHLIB:
			printf("Type de section:               Reserved \n");
			break;

		case SHT_DYNSYM:
			printf("Type de section:               Dynamic linker symbol table \n");
			break;

		case SHT_INIT_ARRAY:
			printf("Type de section:               Array of constructors \n");
			break;

		case SHT_FINI_ARRAY:
			printf("Type de section:               Array of destructors \n");
			break;

		case SHT_PREINIT_ARRAY:
			printf("Type de section:               Array of pre-constructors \n");
			break;

		case SHT_GROUP:
			printf("Type de section:               Section group \n");
			break;

		case SHT_SYMTAB_SHNDX:
			printf("Type de section:               Extended section indeces \n");
			break;

		case SHT_NUM:
			printf("Type de section:               Number of defined types.  \n");
			break;

		case SHT_LOOS:
			printf("Type de section:               Start OS-specific.  \n");
			break;

		case SHT_GNU_ATTRIBUTES:
			printf("Type de section:               Object attributes.  \n");
			break;

		case SHT_GNU_HASH:
			printf("Type de section:               GNU-style hash table.  \n");
			break;

		case SHT_GNU_LIBLIST:
			printf("Type de section:               Prelink library list \n");
			break;

		case SHT_CHECKSUM:
			printf("Type de section:               Checksum for DSO content.  \n");
			break;

		case SHT_LOSUNW:
			printf("Type de section:               Sun-specific low bound.  \n");
			break;
		case SHT_GNU_verdef:
			printf("Type de section:               Version definition section.  \n");
			break;

		case SHT_GNU_verneed:
			printf("Type de section:               Version needs section.  \n");
			break;

		case SHT_GNU_versym:
			printf("Type de section:               Version symbol table.  \n");
			break;

		case SHT_LOPROC:
			printf("Type de section:               Start of processor-specific \n");
			break;

		case SHT_HIPROC:
			printf("Type de section:               End of processor-specific \n");
			break;

		case SHT_LOUSER:
			printf("Type de section:               Start of application-specific \n");
			break;

		case SHT_HIUSER:
			printf("Type de section:               End of application-specific \n");
			break;

		default:
			printf("Type de section:               INVALIDE \n");
			break;
		}

		//Affichage des flags (principaux attributs pour le sujet)
		printf("FLAGS:\n");
		if (sections->sec_list[i].sh_flags & SHF_WRITE)
		{
			printf("  Writable\n");
		}
		if (sections->sec_list[i].sh_flags & SHF_ALLOC)
		{
			printf("  Occupies memory during execution\n");
		}
		if (sections->sec_list[i].sh_flags & SHF_EXECINSTR)
		{
			printf("  Executable\n");
		}
		if (sections->sec_list[i].sh_flags & SHF_MERGE)
		{
			printf("  Might be merged\n");
		}
		if (sections->sec_list[i].sh_flags & SHF_STRINGS)
		{
			printf("  Contains nul-terminated strings\n");
		}
		if (sections->sec_list[i].sh_flags & SHF_INFO_LINK)
		{
			printf("  sh_info' contains SHT index\n");
		}
		if (sections->sec_list[i].sh_flags & SHF_LINK_ORDER)
		{
			printf("  Preserve order after combining\n");
		}
		if (sections->sec_list[i].sh_flags & SHF_OS_NONCONFORMING)
		{
			printf("  Non-standard OS specific handling required\n");
		}
		if (sections->sec_list[i].sh_flags & SHF_GROUP)
		{
			printf("  Section is member of a group\n");
		}
		if (sections->sec_list[i].sh_flags & SHF_TLS)
		{
			printf("  Section hold thread-local data\n");
		}
		if (sections->sec_list[i].sh_flags & SHF_COMPRESSED)
		{
			printf("  Section with compressed data\n");
		}

		// Affichage de l'offset
		printf("Section file offset:           %08lx (bytes into file) \n", sections->sec_list[i].sh_offset);

		// Affichage de la taille de la table (s'il y en a une)
		printf("Table size (if any):           %016lx (bytes) \n", sections->sec_list[i].sh_entsize);

		// Affichage des liens(s'il y en a)
		printf("Link:                          %d \n", sections->sec_list[i].sh_link);

		// Affichage des infos (s'il y en a)
		printf("Additional infos:              %d \n", sections->sec_list[i].sh_info);

		// Affichage de l'alignement
		printf("Alignement:                    %ld \n", sections->sec_list[i].sh_addralign);

		printf("\n");
	}
}