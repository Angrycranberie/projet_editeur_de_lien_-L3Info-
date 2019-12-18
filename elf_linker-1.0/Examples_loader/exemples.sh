#!/bin/bash

files=$(ls *.s)

for i in $files
do
	name=$(basename $i .s)
	arm-eabi-as -o $name.o $i
	arm-eabi-ld -r -o resultat_$name.o $name.o
done
