import os
import matplotlib.pyplot as plt

OMP_NUM_THREADS = 1
NB_MAX_THREAD = 50
nb_exec = 50

# Compiler le fichier main.c
os.system('gcc -g -o main main.c -fopenmp -Wall')

# Créer un fichier exec_time.txt s'il n'existe pas, sinon le vider
open('exec_time.txt', 'w').close()

while OMP_NUM_THREADS <= NB_MAX_THREAD:
    # Changer le nombre de threads utilisés par openmp
    os.environ['OMP_NUM_THREADS'] = str(OMP_NUM_THREADS)
    for i in range(nb_exec):
        # Récupérer les temps d'exécution dans le fichier exec_time.txt
        os.system(f'./main projet_base/dictSortie.txt projet_base/shadowSmall.txt exec_time.txt')
    OMP_NUM_THREADS += 1

# Récupération des données du fichier exec_time.txt
with open('exec_time.txt', 'r') as f:
    data = f.readlines()
    f.close()

results = {}
nb = 1
result = []

for line in data:
    line = line.split('\t')
    line[-1] = line[-1].replace('\n', '') # enlever le caractère '\n' à la fin de la ligne
    nb_threads = int(line[0])
    total_time = float(line[1])
    parallel_time = float(line[2])
    # Calcul de la moyenne des runs
    if nb_threads != nb:
        results[nb] = [sum([result[i][0] for i in range(len(result))]) / len(result),
                       sum([result[i][1] for i in range(len(result))]) / len(result)]
        nb = nb_threads
        result = []
    result.append([total_time, parallel_time])

results[nb] = [sum([result[i][0] for i in range(len(result))]) / len(result),
               sum([result[i][1] for i in range(len(result))]) / len(result)]


# Affichage du graphique du nombre de threads en fonction du temps total d'exécution
plt.plot(range(1, NB_MAX_THREAD + 1), [results[i][0] for i in range(1, NB_MAX_THREAD + 1)], label='Total time')
plt.xlabel('Number of threads')
plt.ylabel('Total Execution Time (s)')
plt.title('Total Execution Time by Number of Threads')
plt.savefig('graph_exec_time.pdf')
plt.show()
plt.close()