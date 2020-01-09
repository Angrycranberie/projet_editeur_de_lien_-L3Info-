#!/bin/bash

# On vérifie le nombre d'argument
if [ $# -gt 0 ]
then

	# Creation des exemples
	cd Examples_loader
	./exemples.sh >/dev/null
	cd ..
	echo Exemples crees
	
	
	# Creation des executables
	autoreconf
	./configure >/dev/null
	make >/dev/null
	echo Executables crees

	# Creation du repertoire recevant les resultats des tests
	if [ ! -d Resultat ]
	then
		mkdir Resultat
	fi

	files=$(ls Examples_loader/*.o)

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
		
			# On sépare le traitement de l'executable readelf_section_content des autres
			if [ "$i" = "x" ]
			then

				./x.sh $name $files		

			else

				arm-eabi-readelf -$i Examples_loader/$name.o > Resultat/$i/$name.out					
				# On sélectionne l'exécutable à tester
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
				# Création et rangement du fichier résultat
				./readelf_$m Examples_loader/$name.o > Resultat/$i/Our_$name.out
			fi
		done
	done
	echo "Resultats disponibles"
else
	echo Erreur - Veuillez donner les options à tester en argument
fi
