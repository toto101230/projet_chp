#!/bin/bash

echo "Lancement du script de référence"
gcc -o multi multiattack.c
time ./projet_base/multi 8 projet_base/dictSortie.txt projet_base/shadow.txt

echo "Lancement de notre script"
gcc -g -o main main.c -fopenmp -Wall
time ./main 8 projet_base/dictSortie.txt projet_base/shadow.txt