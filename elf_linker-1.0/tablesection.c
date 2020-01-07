#include <stdio.h>
#include <elf.h>
#include "tablesection.h"
#include "header.h"

void Tablesection64(FILE *f, Elf64_Ehdr header)
{
	Elf64_Shdr secheader;
	Elf64_Off offset;
	unsigned char tab[100];
	int i;
	if (header.e_shoff == 0)
		return;
	//On se place au début de la table des sections
	//On fait une entrée pour chaque section
	for (i = 0; i < header.e_shnum; i = i + 1)
	{

		// Se place à l'emplacement du fichier donné par l'offset et le numero de la section dans
		// la table.
		fseek(f, header.e_shoff + i * header.e_shentsize, SEEK_SET);
		printf("%lx\n", header.e_shoff + i * header.e_shentsize);

		// On regarde le header de la section
		fread(&secheader.sh_name, sizeof(Elf64_Word), 1, f);
		fread(&secheader.sh_type, sizeof(Elf64_Word), 1, f);
		fread(&secheader.sh_flags, sizeof(Elf64_Xword), 1, f);
		fread(&secheader.sh_addr, sizeof(Elf64_Addr), 1, f);
		fread(&secheader.sh_offset, sizeof(Elf64_Off), 1, f);
		fread(&secheader.sh_size, sizeof(Elf64_Xword), 1, f);
		fread(&secheader.sh_link, sizeof(Elf64_Word), 1, f);
		fread(&secheader.sh_info, sizeof(Elf64_Word), 1, f);
		fread(&secheader.sh_addralign, sizeof(Elf64_Xword), 1, f);
		fread(&secheader.sh_entsize, sizeof(Elf64_Xword), 1, f);

		// On regarde le nom de la section dans la table des chaînes
		fseek(f, header.e_shoff + header.e_shstrndx * header.e_shentsize + 2 * sizeof(Elf64_Word) + sizeof(Elf64_Xword) + sizeof(Elf64_Addr), SEEK_SET);
		fread(&offset, sizeof(Elf64_Off), 1, f);
		fseek(f, offset + secheader.sh_name, SEEK_SET);
		int j = 0;
		fread(&tab[j], sizeof(unsigned char), 1, f);
		while (tab[j] != '\0')
		{
			j++;
			fread(&tab[j], sizeof(unsigned char), 1, f);
		}

		// On affiche les informations sur la section
		printf("Numero de section:             %d\n", i);
		printf("Addresse de section:           %0161x\n", secheader.sh_addr);
		printf("Nom de la section:             %s\n", tab);
		printf("Taille de la section:          %0161x (bytes)\n", secheader.sh_size);

		// affichage du type de section
		switch (secheader.sh_type)
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
		if (secheader.sh_flags & SHF_WRITE)
		{
			printf("  Writable\n");
		}
		if (secheader.sh_flags & SHF_ALLOC)
		{
			printf("  Occupies memory during execution\n");
		}
		if (secheader.sh_flags & SHF_EXECINSTR)
		{
			printf("  Executable\n");
		}
		if (secheader.sh_flags & SHF_MERGE)
		{
			printf("  Might be merged\n");
		}
		if (secheader.sh_flags & SHF_STRINGS)
		{
			printf("  Contains nul-terminated strings\n");
		}
		if (secheader.sh_flags & SHF_INFO_LINK)
		{
			printf("  sh_info' contains SHT index\n");
		}
		if (secheader.sh_flags & SHF_LINK_ORDER)
		{
			printf("  Preserve order after combining\n");
		}
		if (secheader.sh_flags & SHF_OS_NONCONFORMING)
		{
			printf("  Non-standard OS specific handling required\n");
		}
		if (secheader.sh_flags & SHF_GROUP)
		{
			printf("  Section is member of a group\n");
		}
		if (secheader.sh_flags & SHF_TLS)
		{
			printf("  Section hold thread-local data\n");
		}
		if (secheader.sh_flags & SHF_COMPRESSED)
		{
			printf("  Section with compressed data\n");
		}

		// Affichage de l'offset
		printf("Section file offset:           %081x (bytes into file) \n", secheader.sh_offset);

		// Affichage de la taille de la table (s'il y en a une)
		printf("Table size (if any):           %0161x (bytes) \n", secheader.sh_entsize);

		// Affichage des liens(s'il y en a)
		printf("Link:                          %d \n", secheader.sh_link);

		// Affichage des infos (s'il y en a)
		printf("Additional infos:              %d \n", secheader.sh_info);

		// Affichage de l'alignement
		printf("Alignement:                    %ld \n", secheader.sh_addralign);

		printf("\n");
	}
}

void Tablesection32(FILE *f, Elf32_Ehdr header)
{
	Elf32_Shdr secheader;
	Elf32_Off offset;
	char tab[100];
	int i;
	if (header.e_shoff == 0)
		return;

	//On se place au début de la table des sections
	//On fait une entrée pour chaque section
	for (i = 0; i < header.e_shnum; i = i + 1)
	{

		// Se place à l'emplacement du fichier donné par l'offset et le numero de la section dans
		// la table.
		fseek(f, header.e_shoff + i * header.e_shentsize, SEEK_SET);

		// On regarde le header de la section
		fread(&secheader.sh_name, sizeof(Elf32_Word), 1, f);

		fread(&secheader.sh_type, sizeof(Elf32_Word), 1, f);

		fread(&secheader.sh_flags, sizeof(Elf32_Word), 1, f);

		fread(&secheader.sh_addr, sizeof(Elf32_Addr), 1, f);

		fread(&secheader.sh_offset, sizeof(Elf32_Off), 1, f);

		fread(&secheader.sh_size, sizeof(Elf32_Word), 1, f);

		fread(&secheader.sh_link, sizeof(Elf32_Word), 1, f);

		fread(&secheader.sh_info, sizeof(Elf32_Word), 1, f);

		fread(&secheader.sh_addralign, sizeof(Elf32_Word), 1, f);

		fread(&secheader.sh_entsize, sizeof(Elf32_Word), 1, f);

		// On regarde le nom de la section dans la table des chaînes
		fseek(f, header.e_shoff + header.e_shstrndx * header.e_shentsize + 3 * sizeof(Elf32_Word) + sizeof(Elf32_Addr), SEEK_SET);
		fread(&offset, sizeof(Elf32_Off), 1, f);
		fseek(f, offset + secheader.sh_name, SEEK_SET);
		int j = 0;
		fread(&tab[j], sizeof(unsigned char), 1, f);
		while (tab[j] != '\0')
		{
			j++;
			fread(&tab[j], sizeof(unsigned char), 1, f);
		}

		// On affiche les informations sur la section
		printf("Numero de section:             %d\n", i);
		printf("Addresse de section:           %016x\n", secheader.sh_addr);
		printf("Nom de la section:             %s\n", tab);
		printf("Taille de la section:          %016x (bytes)\n", secheader.sh_size);

		// affichage du type de section
		switch (secheader.sh_type)
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
		if (secheader.sh_flags & SHF_WRITE)
		{
			printf("  Writable\n");
		}
		if (secheader.sh_flags & SHF_ALLOC)
		{
			printf("  Occupies memory during execution\n");
		}
		if (secheader.sh_flags & SHF_EXECINSTR)
		{
			printf("  Executable\n");
		}
		if (secheader.sh_flags & SHF_MERGE)
		{
			printf("  Might be merged\n");
		}
		if (secheader.sh_flags & SHF_STRINGS)
		{
			printf("  Contains nul-terminated strings\n");
		}
		if (secheader.sh_flags & SHF_INFO_LINK)
		{
			printf("  sh_info' contains SHT index\n");
		}
		if (secheader.sh_flags & SHF_LINK_ORDER)
		{
			printf("  Preserve order after combining\n");
		}
		if (secheader.sh_flags & SHF_OS_NONCONFORMING)
		{
			printf("  Non-standard OS specific handling required\n");
		}
		if (secheader.sh_flags & SHF_GROUP)
		{
			printf("  Section is member of a group\n");
		}
		if (secheader.sh_flags & SHF_TLS)
		{
			printf("  Section hold thread-local data\n");
		}
		if (secheader.sh_flags & SHF_COMPRESSED)
		{
			printf("  Section with compressed data\n");
		}

		// Affichage de l'offset
		printf("Section file offset:           %08x (bytes into file) \n", secheader.sh_offset);

		// Affichage de la taille de la table (s'il y en a une)
		printf("Table size (if any):           %016x (bytes)\n", secheader.sh_entsize);

		// Affichage des liens(s'il y en a)
		printf("Link:                          %d \n", secheader.sh_link);

		// Affichage des infos (s'il y en a)
		printf("Additional infos:              %d \n", secheader.sh_info);

		// Affichage de l'alignement
		printf("Alignement:                    %d \n", secheader.sh_addralign);

		printf("\n");
	}
}