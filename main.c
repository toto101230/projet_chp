#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>


// Fonction qui récupère les lignes d'un fichier et les stocke dans un tableau
char **read_file(char *filename, int *size_dict) {
    FILE *f = fopen(filename, "r");
    if (f == NULL)
        exit(EXIT_FAILURE);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int size = 0;
    while ((read = getline(&line, &len, f)) != -1) {
        size++;
    }
    char **dict = malloc(size * sizeof(char *));
    rewind(f);
    int i = 0;
    while ((read = getline(&line, &len, f)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        dict[i] = malloc((strlen(line) + 1) * sizeof(char));
        strcpy(dict[i], line);
        i++;
    }
    fclose(f);
    *size_dict = size;
    return dict;
}

// Fonction de recherche de mots de passe
int grep(char *shadow, char **dict, int size_dict, int id, int verbose) {
    if (shadow == NULL)
        return 0;

    for(int i = 0; i < size_dict; i++) {
        char* line = dict[i];
        if (strstr(line, shadow) != NULL) {
            line[strcspn(line, "\r\n")] = 0;
            if (verbose == 1)
                printf("%s found by %d\n", line, id);
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {

    struct timeval start, end;
    struct timeval start_while, end_while;
    char *dict_file = argv[1];
    char *shasum_file = argv[2];
    int verbose = 0;
    char **dict = NULL;
    int size_dict = 0;
    char **shadow = NULL;
    int size_shadow = 0;
    int id_thread;
    int nb_password_found = 0;

    // Récupère le nombre de thread depuis la variable d'environnement OMP_NUM_THREADS
    char *nb_thread = getenv("OMP_NUM_THREADS");
    if (nb_thread == NULL) {
        printf("La variable d'environnement OMP_NUM_THREADS n'est pas définie\n");
        exit(EXIT_FAILURE);
    }

    gettimeofday(&start, NULL);
    if (argc < 3) {
        fprintf(stderr, "Usage: '%s' nb_of_processus dictionnary_file shasum_file num_of_process\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 5) {
        if (strcmp(argv[4], "-v") == 0) {
            verbose = 1;
        }
    }

    // Récupération du contenu des fichiers dictionnaire et shadow dans des tableaux
    dict = read_file(dict_file, &size_dict);
    shadow = read_file(shasum_file, &size_shadow);

    FILE *results = fopen("exec_time.txt", "a");
    if (results == NULL)
        exit(EXIT_FAILURE);

    gettimeofday(&start_while, NULL);
    #pragma omp parallel default(none) shared(dict, size_dict, shadow, size_shadow, nb_password_found, verbose) private(id_thread)
    {
        #pragma omp for
        for(int i = 0; i < size_shadow; i++) {
            id_thread = omp_get_thread_num();
            if (grep(shadow[i], dict, size_dict, id_thread, verbose)==1) {
                #pragma omp atomic
                nb_password_found++;
            }
        }
    }
    #pragma omp barrier
    {
        gettimeofday(&end_while, NULL);
    }
    gettimeofday(&end, NULL);

    float parallel_exec_time = ((end_while.tv_sec * 1000000 + end_while.tv_usec) - (start_while.tv_sec * 1000000 + start_while.tv_usec)) / 1000000.0;
    float total_exec_time = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000.0;

    if(verbose == 1) {
        printf("Fin de l'analyse avec %d mots de passe trouvés\n", nb_password_found);
        printf("Temps d'execution total en s: %f\n", total_exec_time);
        printf("Temps d'execution des taches paralleles en s: %f\n", parallel_exec_time);
    }

    // Ecriture des résultats dans le fichier exec_time.txt
    fprintf(results, "%s\t%f\t%f\n", nb_thread, total_exec_time, parallel_exec_time);
    fclose(results);

    // Libération de la mémoire
    for(int i = 0; i < size_dict; i++) {
        free(dict[i]);
    }
    free(dict);

    for(int i = 0; i < size_shadow; i++) {
        free(shadow[i]);
    }
    free(shadow);
    return 0;
}
