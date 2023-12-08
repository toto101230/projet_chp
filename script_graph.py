import os

OMP_NUM_THREADS = 1
NB_MAX_THREAD = 100
os.system('gcc -g -o main main.c -fopenmp -Wall')

# Créer un fichier exec_time.txt s'il n'existe pas, sinon le vider
open('exec_time.txt', 'w').close()

while OMP_NUM_THREADS <= NB_MAX_THREAD:
    # Changer le nombre de threads utilisés par openmp
    os.environ['OMP_NUM_THREADS'] = str(OMP_NUM_THREADS)
    # Récupérer le temps d'exécution dans un fichier
    os.system(f'./main projet_base/dictSortie.txt projet_base/shadow.txt exec_time.txt')
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

f.close()

# Ouvrir le fichier exec_time.txt et récupérer les données dans un tableau
with open('exec_time.txt', 'r') as f:
    data = f.readlines()

results = {}

for line in data:
    line = line.split('\t')
    # enlever le caractère '\n' à la fin de la ligne
    line[-1] = line[-1].replace('\n', '')
    print(line)
    nb_threads = int(line[0])
    total_time = float(line[1])
    parallel_time = float(line[2])
    results[nb_threads] = [total_time, parallel_time]


print(results)

# Afficher le graphique ayant pour abscisse le nombre de threads et pour ordonnée le temps d'exécution
import matplotlib.pyplot as plt

plt.plot(range(1, NB_MAX_THREAD + 1), [results[i][1] for i in range(1, NB_MAX_THREAD + 1)], label='Total time')
plt.xlabel('Number of threads')
plt.ylabel('Execution Time (s)')
plt.title('Execution Time by Number of Threads')
plt.savefig('graph.pdf')
plt.show()
plt.close()









