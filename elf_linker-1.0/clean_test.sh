#!/bin/bash

# Suppression des .o
rm Examples_loader/*.o

# Suppression des resultats
if [ -d Resultat ]
then
	rm -r Resultat
fi
