#!/bin/bash

files=$(ls *.s)

for i in $files
do
	name=$(basename $i .s)
	if [ ! -f $name.o  ]
	then
		arm-eabi-as -o tmp_$name.o $i
		arm-eabi-ld -r -o $name.o tmp_$name.o
		rm tmp_*.o
	fi
done
