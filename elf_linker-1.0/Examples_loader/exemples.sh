#!/bin/bash

files=$(ls *.s)

for i in $files
do
	name=$(basename $i .s)
	arm-eabi-as -o tmp_$name.o $i
	arm-eabi-ld -r -o $name.o tmp_$name.o
	rm tmp_*.o
done
