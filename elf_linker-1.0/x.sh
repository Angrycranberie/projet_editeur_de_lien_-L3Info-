#!/bin/bash

for j in $2
do

#	echo TEST - recuperation nb section du fichier $2.o

	arm-eabi-readelf -S Examples_loader/$1.o > Resultat/x/tmp_${1}_section.out
	nb_section=$(head -n1 Resultat/x/tmp_${1}_section.out | cut -d ' ' -f3)
	rm Resultat/x/tmp_${1}_section.out
#	echo TEST - Il y a $nb_section section dans le fichier $name

	i=0	
	while [ $i -lt $nb_section ]
	do
#		echo TEST - section $i de $1
			
		# Création des résultats et mise au format de la sortie de arm-eabi-readelf
		echo Section $i >> Resultat/x/$1.out
		arm-eabi-readelf -x $i Examples_loader/$1.o >> Resultat/x/$1.out 
		echo "" >> Resultat/x/$1.out
		echo "" >> Resultat/x/$1.out

		# Creation et mise au format des resultats par notre methode
		echo Section $i >> Resultat/x/Our_$1.out
		./readelf_section_content Examples_loader/$1.o id $i >> Resultat/x/Our_$1.out
		echo "" >> Resultat/x/Our_$1.out
		echo "" >> Resultat/x/Our_$1.out

		let "i++"
	done
done
