#!/bin/bash

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
if [ ! -f readelf_h -o ! -f readelf_S -o ! -f readelf_x -o ! -f readelf_s -o ! -f readelf_r ]
then
	make
fi

# Creation des resultats
files=$(ls Examples_loader/resultat_*.o)
mods=$(echo "h S s r")

for i in $mods
do
	mkdir Resultat/$i
	for j in $files
	do
		name=$(basename $j .o)
		arm-eabi-readelf -$i Examples_loader/$name.o > Resultat/$i/$name.out
		./readelf_$i Examples_loader/$name.o > Resultat/$i/Our_$name.out
	done
done
echo "Resultats disponibles"
