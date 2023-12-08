#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

char** read_file(char *filename) {
    FILE *f = fopen(filename, "r");
    if (f == NULL)
        exit(EXIT_FAILURE);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int i = 0;
    while ((read = getline(&line, &len, f)) != -1) {
        i++;
    }
    char *dict[i];
    rewind(f);
    i = 0;
    while ((read = getline(&line, &len, f)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        dict[i] = line;
        i++;
    }
    fclose(f);
    return dict;
}

int grep(char *shadow, char **dict, int size_dict, int i, int verbose) {

    if (shadow == NULL)
        return 0;

    for(int i = 0; i < size_dict; i++) {
        char* line = dict[i];
        if (strstr(line, shadow) == 0) {
            line[strcspn(line, "\r\n")] = 0;
            if (verbose == 1)
                printf("%s found by %d\n", line, i);
            return 1;
        }
    }
    return 0;
}

char *readline(FILE *f) {
    char *line = NULL;

    size_t len = 0;
    ssize_t read;
    if ((read = getline(&line, &len, f)) != -1) {
        line[read - 2] = '\0';
        return line;
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    // Récupérer le nombre de thread depuis la variable d'environnement OMP_NUM_THREADS
    char *nb_thread = getenv("OMP_NUM_THREADS");
    if (nb_thread == NULL) {
        printf("La variable d'environnement OMP_NUM_THREADS n'est pas définie\n");
        exit(EXIT_FAILURE);
    }

    struct timeval start, end;
    struct timeval start_while, end_while;
    gettimeofday(&start, NULL);
    if (argc < 3) {
        fprintf(stderr, "Usage: '%s' nb_of_processus dictionnary_file shasum_file num_of_process\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *dict_file = argv[1];
    char *shasum_file = argv[2];
    int verbose = 0;
    if (argc == 4) {
        if (strcmp(argv[3], "-v") == 0) {
            printf("Verbose mode activated\n");
            verbose = 1;
        }
    }

     
    char **dict = read_file(dict_file);
    int size_dict = sizeof(dict) / sizeof(dict[0]);
    char **shadow = read_file(shasum_file);
    int size_shadow = sizeof(shadow) / sizeof(shadow[0]);


    FILE *results = fopen("exec_time.txt", "a");
    if (results == NULL)
        exit(EXIT_FAILURE);

    printf("1\n");
    int id;
    int nb = 0;
    gettimeofday(&start_while, NULL);
    #pragma omp parallel default(none) shared(dict, size_dict, shadow, size_shadow, nb, verbose) private(id)
    {
        for(int i = 0; i < size_shadow; i++) {
            printf(shadow[i]);
            id = omp_get_thread_num();
            if (grep(shadow[i], dict, size_dict, id, verbose)==1) {
                nb++;
            }
        }
    }
    #pragma omp barrier
    {
        gettimeofday(&end_while, NULL);
    }
    

    printf("Fin de l'analyse avec %d mots de passe trouvés\n", nb);
    float parallel_exec_time = ((end_while.tv_sec * 1000000 + end_while.tv_usec) - (start_while.tv_sec * 1000000 + start_while.tv_usec)) / 1000000.0;
    printf("Temps d'execution des taches paralleles en s: %f\n", parallel_exec_time);
    gettimeofday(&end, NULL);
    float total_exec_time = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000.0;
    printf("Temps d'execution en s: %f\n", total_exec_time);
    printf("%f\n", total_exec_time);

    fprintf(results, "%s\t%f\t%f\n", nb_thread, total_exec_time, parallel_exec_time);
    fclose(results);
    return 0;
}

