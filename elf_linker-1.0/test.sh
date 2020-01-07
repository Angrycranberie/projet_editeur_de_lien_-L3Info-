#!/bin/bash

# On vérifie le nombre d'argument
if [ $# -eq 0 ]
then
	echo Erreur - Donnez en argument les options à tester
fi

#Creation des exemples
cd Examples_loader/
./exemples.sh
cd ..
echo "Exemples crees"

# Creation du repertoire recevant les resultats des tests
if [ ! -d Resultat ]
then
	mkdir Resultat
fi

# On verifie l'existence des executables
if [ ! -f readelf_header -o ! -f readelf_section_list -o ! -f readelf_symbols_table -o ! -f readelf_relocation ]
then
	make
fi

files=$(ls Examples_loader/resultat_*.o)

# Creation des resultats
for i in $@
do
	if [ ! -d Resultat/$i ]
	then
		mkdir Resultat/$i
	fi
	for j in $files
	do
		name=$(basename $j .o)
		arm-eabi-readelf -$i Examples_loader/$name.o > Resultat/$i/$name.out
		if [ "$i" = "h" ]
		then
			m=header
		elif [ "$i" = "S" ]
		then
			m=section_list
		elif [ "$i" = "s" ]
		then
			m=symbols_table
		elif [ "$i" = "r" ]
		then
			m=relocation
		fi
		./readelf_$m Examples_loader/$name.o > Resultat/$i/Our_$name.out
	done
done
echo "Resultats disponibles"
