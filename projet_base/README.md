# Installer les dependances
sudo apt-get install libssl-dev

# Generer un fichier dictSortie.txt qui contient pour chaque ligne le mot et son hash
gcc -g -o mdg my_sha_dict_generator.c -lssl -lcrypto
./mdg dict.txt dictSortie.txt

# Generation d'un shadow
gcc -g -o msg my_sha_shadow_generator.c -lssl -lcrypto
./msg dictSortie.txt shadowSmall.txt 20

# Faire tourner le code de base (non parallélisé)
gcc -o multi multiattack.c
./multi  8 dictSortie.txt shadowSmall.txt
# Pour écupérer le temps d'execution
time ./multi  8 dictSortie.txt shadow.txt

# Faire tourner le code optimisé (parrallélisé)
gcc -g -o main main.c -fopenmp -Wall
./main projet_base/dictSortie.txt projet_base/shadowSmall.txt
# Pour écupérer le temps d'execution
time ./main projet_base/dictSortie.txt projet_base/shadow.txt

# Pour changer le nombre de threads utilisés par openmp (8 par défaut)
export OMP_NUM_THREADS=4

