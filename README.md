Compilation et Lancement

Pour lancer le projet, il suffit simplement de suivre une suite d’étapes : 

1 - Cloner le projet depuis GIT,
2 - Utiliser la commande ‘autoreconf’ dans le répertoire ‘elf_linker 1.0’,
3 - Utiliser la commande ‘./configure’ pour faire l’Automake du projet,
4 - Utiliser la commande ‘make’ afin de réaliser les exécutables du projet,
5 - Lancer l’exécutable correspondant à l’étape que l’on veut.		

Liste des étapes et de leur exécutable : 


ETAPES
Exécutable
arguments

ETAPES 1
Exécutable : readelf_header
arguments : <fichier elf>

ETAPES 2
Exécutable : readelf_section_list
arguments : <fichier elf>

ETAPES 3
Exécutable : readelf_section_content
arguments : <fichier elf>, “name” et le nom de la section OU “id” et l’id de la section

ETAPES 4
Exécutable : readelf_symbols_table
arguments : <fichier elf>

ETAPES 5
Exécutable : readelf_relocation
arguments : <fichier elf>

ETAPES 6
Exécutable : etape6
arguments : <fichier elf> <fichier elf>

ETAPES 7
Exécutable : etape7
arguments : 

ETAPES 8
Exécutable : 
arguments : 


ETAPES 9
Exécutable : 
arguments : 


