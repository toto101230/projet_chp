import os

OMP_NUM_THREADS = 1
NB_MAX_THREAD = 10
os.system('gcc -g -o main main.c -fopenmp -Wall')

# Créer un fichier exec_time.txt s'il n'existe pas, sinon le vider
open('exec_time.txt', 'w').close()

while OMP_NUM_THREADS <= NB_MAX_THREAD:
    # Changer le nombre de threads utilisés par openmp
    os.environ['OMP_NUM_THREADS'] = str(OMP_NUM_THREADS)
    # Récupérer le temps d'exécution dans un fichier
    os.system(f'./main projet_base/dictSortie.txt projet_base/shadowSmall.txt exec_time.txt')
    OMP_NUM_THREADS += 1


# Ouvrir le fichier exec_time.txt et récupérer les données dans un tableau
with open('exec_time.txt', 'r') as f:
    data = f.readlines()

exec_times = {}
for line in data:
    # split la ligne avec comme séparateur le caractère '\t'
    values = line.split('\t')
    # récupère le nombre de threads
    nb_threads = int(values[0])
    # récupère le temps d'exécution
    total_time = float(values[1])
    parallel_time = float(values[2])
    times = [total_time, parallel_time]
    exec_times[nb_threads] = times

print(exec_times)









