### Installer les dépendances
- Pour avoir la fonction de hachage SHA1:
sudo apt-get install libssl-dev

- Pour tracer les graphiques :
avoir Python et installer matplotlib : pip install matplotlib

### Pour tracer la loi d'Amdahl du code parallélisé (main.c)
python script_amdahl.py

### Pour tracer le graphique du temps d'exécution en fonction du nombre de processus de multiattack.c
python script_graph_multi.py

### Pour tracer le graphique du temps d'exécution en fonction du nombre de processus du code parallélisé (main.c)
python script_exec_times.py

### Pour afficher le temps d'exécution des deux programmes multiattack et main.c
./script.sh

### Générer un fichier dictSortie.txt qui contient pour chaque ligne le mot et son hash
gcc -g -o mdg my_sha_dict_generator.c -lssl -lcrypto
./mdg dict.txt dictSortie.txt

### Génération d'un shadow de taille 20
gcc -g -o msg my_sha_shadow_generator.c -lssl -lcrypto
./msg dictSortie.txt shadowSmall.txt 20

### Pour faire tourner le code de base (non parallélisé) avec 8 processus
gcc -o multi multiattack.c
./multi 8 dictSortie.txt shadowSmall.txt

### Pour faire tourner le code optimisé (parrallélisé avec openmp)
gcc -g -o main main.c -fopenmp -Wall
./main projet_base/dictSortie.txt projet_base/shadowSmall.txt

### Pour changer le nombre de threads utilisés par openmp (8 par défaut)
export OMP_NUM_THREADS=4