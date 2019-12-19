#include <stdio.h>
#include <elf.h>
#include "header.h"

//Lecture du fichier : vérification ELF, lecture en 64 pour changer ensuite si c'est un 32 (facilité)
Elf64_Ehdr read_header(FILE *f)
{
    Elf64_Ehdr header;
    int choix_version;

    fread(header.e_ident, sizeof(unsigned char), EI_NIDENT, f);

    // Vérifie les nombres magiques
    // retourne 1 si les "nombres magiques" ne correspondent pas à ceux d'un fichier elf.
    if (header.e_ident[EI_MAG0] != ELFMAG0)
    {
        fprintf(stderr, "  Nombre magique invalide ! Il ne s'agit pas d'un fichier elf\n");
        return header;
    }
    else if (header.e_ident[EI_MAG1] != ELFMAG1)
    {
        fprintf(stderr, "  Nombre magique invalide ! Il ne s'agit pas d'un fichier elf\n");
        return header;
    }
    else if (header.e_ident[EI_MAG2] != ELFMAG2)
    {
        fprintf(stderr, "  Nombre magique invalide ! Il ne s'agit pas d'un fichier elf\n");
        return header;
    }
    else if (header.e_ident[EI_MAG3] != ELFMAG3)
    {
        fprintf(stderr, "  Nombre magique invalide ! Il ne s'agit pas d'un fichier elf\n");
        return header;
    }
    if (header.e_ident[EI_CLASS] != ELFCLASS64 && header.e_ident[EI_CLASS] != ELFCLASS32)
    {
        fprintf(stderr, "  Classe invalide\n");
        return header;
    }

    choix_version = header.e_ident[EI_CLASS] == ELFCLASS64;

    // Lit le type du fichier elf
    fread(&(header.e_type), sizeof(Elf64_Half), 1, f);

    // Lit le type de machine
    fread(&(header.e_machine), sizeof(Elf64_Half), 1, f);

    // Lit la version du fichier
    fread(&(header.e_version), sizeof(Elf64_Word), 1, f);

    // Lit l'addresse de départ
    choix_version ? fread(&(header.e_entry), sizeof(Elf64_Addr), 1, f) : fread(&(header.e_entry), sizeof(Elf32_Addr), 1, f);

    // Lit l'offset permettant d'arriver sur les headers du programme
    choix_version ? fread(&(header.e_phoff), sizeof(Elf64_Off), 1, f) : fread(&(header.e_phoff), sizeof(Elf32_Off), 1, f);

    // Lit l'offset permettant d'arriver sur les headers des sections
    choix_version ? fread(&(header.e_shoff), sizeof(Elf64_Off), 1, f) : fread(&(header.e_shoff), sizeof(Elf32_Off), 1, f);

    // Lit les flags, n'affiche que la valeur en hexadecimal, comme readelf.
    fread(&(header.e_flags), sizeof(Elf64_Word), 1, f);

    // Lit la taille du header ( celui en cours de lecture)
    fread(&(header.e_ehsize), sizeof(Elf64_Half), 1, f);

    // Lit la taille des headers du programme
    fread(&(header.e_phentsize), sizeof(Elf64_Half), 1, f);

    // Lit le nombre de headers du programme
    fread(&(header.e_phnum), sizeof(Elf64_Half), 1, f);

    // Lit et affiche la taille des headers de section
    fread(&(header.e_shentsize), sizeof(Elf64_Half), 1, f);

    // Lit le nombre de headers de section
    fread(&(header.e_shnum), sizeof(Elf64_Half), 1, f);

    // Lit e_shstrndx
    fread(&(header.e_shstrndx), sizeof(Elf64_Half), 1, f);

    return header;
}

void print_header(Elf64_Ehdr header)
{
    int i;
    int choix_version = header.e_ident[4] == ELFCLASS64;
    printf("\n\nELF Header:\n");
    printf("  Magic:   ");
    for (i = 0; i < EI_NIDENT; i++)
    {
        printf("%02x ", header.e_ident[i]);
    }
    printf("  \n");

    switch (header.e_ident[4])
    {
    case 0:
        printf("  Class:                             INVALIDE\n");
        break;

    case 1:
        printf("  Class:                             ELF32\n");
        break;

    case 2:
        printf("  Class:                             ELF64\n");
        break;

    default:
        printf("  Class:                             INVALIDE\n");
        break;
    }

    // Recupère le type de data
    switch (header.e_ident[5])
    {
    case 0:
        printf("  Data:                              INVALIDE\n");
        break;

    case 1:
        printf("  Data:                              2's complement, little endian\n");
        break;

    case 2:
        printf("  Data:                              2's complement, big endian\n");
        break;

    default:
        printf("  Data:                              INVALIDE\n");
        break;
    }

    // Verifie la version
    switch (header.e_ident[6])
    {
    case EV_CURRENT:
        printf("  Version:                           %d (current)\n", header.e_ident[6]);
        break;

    default:
        printf("  Version:                           INVALIDE\n");
        break;
    }

    // Verifie l'OS utilisé (on ne connait pas les autres codes, mais 0 signifie linux sans
    // extension. (cette partie et la suivante sont sans doute sans importance, mais permettent
    // l'imitation de l'affichage de readelf -h.
    switch (header.e_ident[7])
    {
    case ELFOSABI_NONE:
        printf("  OS/ABI:                            UNIX System V \n");
        break;

    case ELFOSABI_HPUX:
        printf("  OS/ABI:                            HP-UX \n");
        break;

    case ELFOSABI_NETBSD:
        printf("  OS/ABI:                            NetBSD.  \n");
        break;

    case ELFOSABI_GNU:
        printf("  OS/ABI:                            GNU ELF extensions.  \n");
        break;

    case ELFOSABI_SOLARIS:
        printf("  OS/ABI:                            Sun Solaris.  \n");
        break;

    case ELFOSABI_AIX:
        printf("  OS/ABI:                            IBM AIX.  \n");
        break;

    case ELFOSABI_IRIX:
        printf("  OS/ABI:                            SGI Irix.  \n");
        break;

    case ELFOSABI_FREEBSD:
        printf("  OS/ABI:                            FreeBSD.  \n");
        break;

    case ELFOSABI_TRU64:
        printf("  OS/ABI:                            Compaq TRU64 UNIX.  \n");
        break;

    case ELFOSABI_MODESTO:
        printf("  OS/ABI:                            Novell Modesto.  \n");
        break;

    case ELFOSABI_OPENBSD:
        printf("  OS/ABI:                            OpenBSD.  \n");
        break;

    case ELFOSABI_ARM_AEABI:
        printf("  OS/ABI:                            ARM EABI \n");
        break;

    case ELFOSABI_ARM:
        printf("  OS/ABI:                            ARM \n");
        break;

    case ELFOSABI_STANDALONE:
        printf("  OS/ABI:                            Standalone (embedded) application \n");
        break;

    default:
        printf("  OS/ABI:                            Code Inconnu\n");
        break;
    }

    // Verifie la version de l'ABI
    printf("  ABI Version:                       %d\n", header.e_ident[8]);

    switch (header.e_type)
    {
    case 0:
        printf("  Type:                              NONE (No file type)\n");
        break;

    case 1:
        printf("  Type:                              REL (Relocatable file )\n");
        break;

    case 2:
        printf("  Type:                              EXEC (Executable file )\n");
        break;

    case 3:
        printf("  Type:                              DYN (Shared object file )\n");
        break;

    case 4:
        printf("  Type:                              CORE (Core file )\n");
        break;

    case 0xff00:
        printf("  Type:                              LOPROC (Processor-specific )\n");
        break;

    case 0xffff:
        printf("  Type:                              HIPROC (Processor-specific )\n");
        break;

    default:
        printf("  Type:                              Inconnu\n");
        break;
    }

    // Problème: il y a beaucoup de machines différentes dans elf.h. Le switch a donc �t� r�alis� directement � partir de elf.h, avec des replace.
    switch (header.e_machine)
    {
    case 0:
        printf("  Machine:                           No machine \n");
        break;

    case 1:
        printf("  Machine:                           AT&T WE 32100 \n");
        break;

    case 2:
        printf("  Machine:                           SUN SPARC \n");
        break;

    case 3:
        printf("  Machine:                           Intel 80386 \n");
        break;

    case 4:
        printf("  Machine:                           Motorola m68k family \n");
        break;

    case 5:
        printf("  Machine:                           Motorola m88k family \n");
        break;

    case 6:
        printf("  Machine:                           Intel MCU \n");
        break;

    case 7:
        printf("  Machine:                           Intel 80860 \n");
        break;

    case 8:
        printf("  Machine:                           MIPS R3000 big-endian \n");
        break;

    case 9:
        printf("  Machine:                           IBM System/370 \n");
        break;

    case 10:
        printf("  Machine:                           MIPS R3000 little-endian \n");
        break;

    case 15:
        printf("  Machine:                           HPPA \n");
        break;

    case 17:
        printf("  Machine:                           Fujitsu VPP500 \n");
        break;

    case 18:
        printf("  Machine:                           Sun's v8plus \n");
        break;

    case 19:
        printf("  Machine:                           Intel 80960 \n");
        break;

    case 20:
        printf("  Machine:                           PowerPC \n");
        break;

    case 21:
        printf("  Machine:                           PowerPC 64-bit \n");
        break;

    case 22:
        printf("  Machine:                           IBM S390 \n");
        break;

    case 23:
        printf("  Machine:                           IBM SPU/SPC \n");
        break;

    case 36:
        printf("  Machine:                           NEC V800 series \n");
        break;

    case 37:
        printf("  Machine:                           Fujitsu FR20 \n");
        break;

    case 38:
        printf("  Machine:                           TRW RH-32 \n");
        break;

    case 39:
        printf("  Machine:                           Motorola RCE \n");
        break;

    case 40:
        printf("  Machine:                           ARM \n");
        break;

    case 41:
        printf("  Machine:                           Digital Alpha \n");
        break;

    case 42:
        printf("  Machine:                           Hitachi SH \n");
        break;

    case 43:
        printf("  Machine:                           SPARC v9 64-bit \n");
        break;

    case 44:
        printf("  Machine:                           Siemens Tricore \n");
        break;

    case 45:
        printf("  Machine:                           Argonaut RISC Core \n");
        break;

    case 46:
        printf("  Machine:                           Hitachi H8/300 \n");
        break;

    case 47:
        printf("  Machine:                           Hitachi H8/300H \n");
        break;

    case 48:
        printf("  Machine:                           Hitachi H8S \n");
        break;

    case 49:
        printf("  Machine:                           Hitachi H8/500 \n");
        break;

    case 50:
        printf("  Machine:                           Intel Merced \n");
        break;

    case 51:
        printf("  Machine:                           Stanford MIPS-X \n");
        break;

    case 52:
        printf("  Machine:                           Motorola Coldfire \n");
        break;

    case 53:
        printf("  Machine:                           Motorola M68HC12 \n");
        break;

    case 54:
        printf("  Machine:                           Fujitsu MMA Multimedia Accelerator \n");
        break;

    case 55:
        printf("  Machine:                           Siemens PCP \n");
        break;

    case 56:
        printf("  Machine:                           Sony nCPU embeeded RISC \n");
        break;

    case 57:
        printf("  Machine:                           Denso NDR1 microprocessor \n");
        break;

    case 58:
        printf("  Machine:                           Motorola Start*Core processor \n");
        break;

    case 59:
        printf("  Machine:                           Toyota ME16 processor \n");
        break;

    case 60:
        printf("  Machine:                           STMicroelectronic ST100 processor \n");
        break;

    case 61:
        printf("  Machine:                           Advanced Logic Corp. Tinyj emb.fam \n");
        break;

    case 62:
        printf("  Machine:                           AMD x86-64 architecture \n");
        break;

    case 63:
        printf("  Machine:                           Sony DSP Processor \n");
        break;

    case 64:
        printf("  Machine:                           Digital PDP-10 \n");
        break;

    case 65:
        printf("  Machine:                           Digital PDP-11 \n");
        break;

    case 66:
        printf("  Machine:                           Siemens FX66 microcontroller \n");
        break;

    case 67:
        printf("  Machine:                           STMicroelectronics ST9+ 8/16 mc \n");
        break;

    case 68:
        printf("  Machine:                           STmicroelectronics ST7 8 bit mc \n");
        break;

    case 69:
        printf("  Machine:                           Motorola MC68HC16 microcontroller \n");
        break;

    case 70:
        printf("  Machine:                           Motorola MC68HC11 microcontroller \n");
        break;

    case 71:
        printf("  Machine:                           Motorola MC68HC08 microcontroller \n");
        break;

    case 72:
        printf("  Machine:                           Motorola MC68HC05 microcontroller \n");
        break;

    case 73:
        printf("  Machine:                           Silicon Graphics SVx \n");
        break;

    case 74:
        printf("  Machine:                           STMicroelectronics ST19 8 bit mc \n");
        break;

    case 75:
        printf("  Machine:                           Digital VAX \n");
        break;

    case 76:
        printf("  Machine:                           Axis Communications 32-bit emb.proc \n");
        break;

    case 77:
        printf("  Machine:                           Infineon Technologies 32-bit emb.proc \n");
        break;

    case 78:
        printf("  Machine:                           Element 14 64-bit DSP Processor \n");
        break;

    case 79:
        printf("  Machine:                           LSI Logic 16-bit DSP Processor \n");
        break;

    case 80:
        printf("  Machine:                           Donald Knuth's educational 64-bit proc \n");
        break;

    case 81:
        printf("  Machine:                           Harvard University machine-independent object files \n");
        break;

    case 82:
        printf("  Machine:                           SiTera Prism \n");
        break;

    case 83:
        printf("  Machine:                           Atmel AVR 8-bit microcontroller \n");
        break;

    case 84:
        printf("  Machine:                           Fujitsu FR30 \n");
        break;

    case 85:
        printf("  Machine:                           Mitsubishi D10V \n");
        break;

    case 86:
        printf("  Machine:                           Mitsubishi D30V \n");
        break;

    case 87:
        printf("  Machine:                           NEC v850 \n");
        break;

    case 88:
        printf("  Machine:                           Mitsubishi M32R \n");
        break;

    case 89:
        printf("  Machine:                           Matsushita MN10300 \n");
        break;

    case 90:
        printf("  Machine:                           Matsushita MN10200 \n");
        break;

    case 91:
        printf("  Machine:                           picoJava \n");
        break;

    case 92:
        printf("  Machine:                           OpenRISC 32-bit embedded processor \n");
        break;

    case 93:
        printf("  Machine:                           ARC International ARCompact \n");
        break;

    case 94:
        printf("  Machine:                           Tensilica Xtensa Architecture \n");
        break;

    case 95:
        printf("  Machine:                           Alphamosaic VideoCore \n");
        break;

    case 96:
        printf("  Machine:                           Thompson Multimedia General Purpose Proc \n");
        break;

    case 97:
        printf("  Machine:                           National Semi. 32000 \n");
        break;

    case 98:
        printf("  Machine:                           Tenor Network TPC \n");
        break;

    case 99:
        printf("  Machine:                           Trebia SNP 1000 \n");
        break;

    case 100:
        printf("  Machine:                           STMicroelectronics ST200 \n");
        break;

    case 101:
        printf("  Machine:                           Ubicom IP2xxx \n");
        break;

    case 102:
        printf("  Machine:                           MAX processor \n");
        break;

    case 103:
        printf("  Machine:                           National Semi. CompactRISC \n");
        break;

    case 104:
        printf("  Machine:                           Fujitsu F2MC16 \n");
        break;

    case 105:
        printf("  Machine:                           Texas Instruments msp430 \n");
        break;

    case 106:
        printf("  Machine:                           Analog Devices Blackfin DSP \n");
        break;

    case 107:
        printf("  Machine:                           Seiko Epson S1C33 family \n");
        break;

    case 108:
        printf("  Machine:                           Sharp embedded microprocessor \n");
        break;

    case 109:
        printf("  Machine:                           Arca RISC \n");
        break;

    case 110:
        printf("  Machine:                           PKU-Unity & MPRC Peking Uni. mc series \n");
        break;

    case 111:
        printf("  Machine:                           eXcess configurable cpu \n");
        break;

    case 112:
        printf("  Machine:                           Icera Semi. Deep Execution Processor \n");
        break;

    case 113:
        printf("  Machine:                           Altera Nios II \n");
        break;

    case 114:
        printf("  Machine:                           National Semi. CompactRISC CRX \n");
        break;

    case 115:
        printf("  Machine:                           Motorola XGATE \n");
        break;

    case 116:
        printf("  Machine:                           Infineon C16x/XC16x \n");
        break;

    case 117:
        printf("  Machine:                           Renesas M16C \n");
        break;

    case 118:
        printf("  Machine:                           Microchip Technology dsPIC30F \n");
        break;

    case 119:
        printf("  Machine:                           Freescale Communication Engine RISC \n");
        break;

    case 120:
        printf("  Machine:                           Renesas M32C \n");
        break;

    case 131:
        printf("  Machine:                           Altium TSK3000 \n");
        break;

    case 132:
        printf("  Machine:                           Freescale RS08 \n");
        break;

    case 133:
        printf("  Machine:                           Analog Devices SHARC family \n");
        break;

    case 134:
        printf("  Machine:                           Cyan Technology eCOG2 \n");
        break;

    case 135:
        printf("  Machine:                           Sunplus S+core7 RISC \n");
        break;

    case 136:
        printf("  Machine:                           New Japan Radio (NJR) 24-bit DSP \n");
        break;

    case 137:
        printf("  Machine:                           Broadcom VideoCore III \n");
        break;

    case 138:
        printf("  Machine:                           RISC for Lattice FPGA \n");
        break;

    case 139:
        printf("  Machine:                           Seiko Epson C17 \n");
        break;

    case 140:
        printf("  Machine:                           Texas Instruments TMS320C6000 DSP \n");
        break;

    case 141:
        printf("  Machine:                           Texas Instruments TMS320C2000 DSP \n");
        break;

    case 142:
        printf("  Machine:                           Texas Instruments TMS320C55x DSP \n");
        break;

    case 143:
        printf("  Machine:                           Texas Instruments App. Specific RISC \n");
        break;

    case 144:
        printf("  Machine:                           Texas Instruments Prog. Realtime Unit \n");
        break;

    case 160:
        printf("  Machine:                           STMicroelectronics 64bit VLIW DSP \n");
        break;

    case 161:
        printf("  Machine:                           Cypress M8C \n");
        break;

    case 162:
        printf("  Machine:                           Renesas R32C \n");
        break;

    case 163:
        printf("  Machine:                           NXP Semi. TriMedia \n");
        break;

    case 164:
        printf("  Machine:                           QUALCOMM DSP6 \n");
        break;

    case 165:
        printf("  Machine:                           Intel 8051 and variants \n");
        break;

    case 166:
        printf("  Machine:                           STMicroelectronics STxP7x \n");
        break;

    case 167:
        printf("  Machine:                           Andes Tech. compact code emb. RISC \n");
        break;

    case 168:
        printf("  Machine:                           Cyan Technology eCOG1X \n");
        break;

    case 169:
        printf("  Machine:                           Dallas Semi. MAXQ30 mc \n");
        break;

    case 170:
        printf("  Machine:                           New Japan Radio (NJR) 16-bit DSP \n");
        break;

    case 171:
        printf("  Machine:                           M2000 Reconfigurable RISC \n");
        break;

    case 172:
        printf("  Machine:                           Cray NV2 vector architecture \n");
        break;

    case 173:
        printf("  Machine:                           Renesas RX \n");
        break;

    case 174:
        printf("  Machine:                           Imagination Tech. META \n");
        break;

    case 175:
        printf("  Machine:                           MCST Elbrus \n");
        break;

    case 176:
        printf("  Machine:                           Cyan Technology eCOG16 \n");
        break;

    case 177:
        printf("  Machine:                           National Semi. CompactRISC CR16 \n");
        break;

    case 178:
        printf("  Machine:                           Freescale Extended Time Processing Unit \n");
        break;

    case 179:
        printf("  Machine:                           Infineon Tech. SLE9X \n");
        break;

    case 180:
        printf("  Machine:                           Intel L10M \n");
        break;

    case 181:
        printf("  Machine:                           Intel K10M \n");
        break;

    case 183:
        printf("  Machine:                           ARM AARCH64 \n");
        break;

    case 185:
        printf("  Machine:                           Amtel 32-bit microprocessor \n");
        break;

    case 186:
        printf("  Machine:                           STMicroelectronics STM8 \n");
        break;

    case 187:
        printf("  Machine:                           Tileta TILE64 \n");
        break;

    case 188:
        printf("  Machine:                           Tilera TILEPro \n");
        break;

    case 189:
        printf("  Machine:                           Xilinx MicroBlaze \n");
        break;

    case 190:
        printf("  Machine:                           NVIDIA CUDA \n");
        break;

    case 191:
        printf("  Machine:                           Tilera TILE-Gx \n");
        break;

    case 192:
        printf("  Machine:                           CloudShield \n");
        break;

    case 193:
        printf("  Machine:                           KIPO-KAIST Core-A 1st gen. \n");
        break;

    case 194:
        printf("  Machine:                           KIPO-KAIST Core-A 2nd gen. \n");
        break;

    case 195:
        printf("  Machine:                           Synopsys ARCompact V2 \n");
        break;

    case 196:
        printf("  Machine:                           Open8 RISC \n");
        break;

    case 197:
        printf("  Machine:                           Renesas RL78 \n");
        break;

    case 198:
        printf("  Machine:                           Broadcom VideoCore V \n");
        break;

    case 199:
        printf("  Machine:                           Renesas 78KOR \n");
        break;

    case 200:
        printf("  Machine:                           Freescale 56800EX DSC \n");
        break;

    case 201:
        printf("  Machine:                           Beyond BA1 \n");
        break;

    case 202:
        printf("  Machine:                           Beyond BA2 \n");
        break;

    case 203:
        printf("  Machine:                           XMOS xCORE \n");
        break;

    case 204:
        printf("  Machine:                           Microchip 8-bit PIC(r) \n");
        break;

    case 210:
        printf("  Machine:                           KM211 KM32 \n");
        break;

    case 211:
        printf("  Machine:                           KM211 KMX32 \n");
        break;

    case 212:
        printf("  Machine:                           KM211 KMX16 \n");
        break;

    case 213:
        printf("  Machine:                           KM211 KMX8 \n");
        break;

    case 214:
        printf("  Machine:                           KM211 KVARC \n");
        break;

    case 215:
        printf("  Machine:                           Paneve CDP \n");
        break;

    case 216:
        printf("  Machine:                           Cognitive Smart Memory Processor \n");
        break;

    case 217:
        printf("  Machine:                           Bluechip CoolEngine \n");
        break;

    case 218:
        printf("  Machine:                           Nanoradio Optimized RISC \n");
        break;

    case 219:
        printf("  Machine:                           CSR Kalimba \n");
        break;

    case 220:
        printf("  Machine:                           Zilog Z80 \n");
        break;

    case 221:
        printf("  Machine:                           Controls and Data Services VISIUMcore \n");
        break;

    case 222:
        printf("  Machine:                           FTDI Chip FT32 \n");
        break;

    case 223:
        printf("  Machine:                           Moxie processor \n");
        break;

    case 224:
        printf("  Machine:                           AMD GPU \n");
        break;

    case 243:
        printf("  Machine:                           RISC-V \n");
        break;

    case 247:
        printf("  Machine:                           Linux BPF -- in-kernel virtual machine \n");
        break;

    default:
        printf("  Machine:                           Inconnue\n");
        break;
    }

    // Lit et affiche la version du fichier
    printf("  Version:                           0x%x\n", header.e_version);

    // Lit et affiche l'addresse de départ
    printf("  Entry point address:               ");
    choix_version ? printf("0x%lx\n", header.e_entry) : printf("0x%x\n", (Elf32_Addr)header.e_entry);

    // Lit et affiche l'offset permettant d'arriver sur les headers du programme
    printf("  Start of program headers:          ");
    choix_version ? printf("%lu (bytes into file)\n", header.e_phoff) : printf("%d (bytes into file)\n", (Elf32_Off)header.e_phoff);

    // Lit et affiche l'offset permettant d'arriver sur les headers des sections
    printf("  Start of section headers:          ");
    choix_version ? printf("%lu (bytes into file)\n", header.e_shoff) : printf("%d (bytes into file)\n", (Elf32_Off)header.e_shoff);

    // Lit et affiche les flags, n'affiche que la valeur en hexadecimal, comme readelf.
    printf("  Flags:                             0x%x \n", header.e_flags);

    // Lit et affiche la taille du header ( celui en cours de lecture)
    printf("  Size of this header:               %d (bytes) \n", header.e_ehsize);

    // Lit et affiche la taille des headers du programme
    printf("  Size of program headers:           %d (bytes) \n", header.e_phentsize);

    // Lit et affiche le nombre de headers du programme
    printf("  Number of program headers:         %d \n", header.e_phnum);

    // Lit et affiche la taille des headers de section
    printf("  Size of section headers:           %d (bytes) \n", header.e_shentsize);

    // Lit et affiche le nombre de headers de section
    printf("  Number of section headers:         %d \n", header.e_shnum);

    // Lit et affiche e_shstrndx
    printf("  Section header string table index: %d  \n", header.e_shstrndx);

    printf("  End of Header\n\n");
}
