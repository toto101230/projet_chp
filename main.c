#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

int grep(char *analyse, char *file, int i, int verbose) {
    if (analyse == NULL)
        return 0;
    FILE *f = fopen(file, "r");
    if (f == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, f)) != -1) {
        if (strstr(line, analyse) != NULL) {
            line[strcspn(line, "\r\n")] = 0;
            if (verbose == 1)
                printf("%s found by %d\n", line, i);
            fclose(f);
            return 1;
        }
    }
    fclose(f);
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

    FILE *ds = fopen(shasum_file, "r");
    if (ds == NULL)
        exit(EXIT_FAILURE);
    char *current_password_to_analyse = readline(ds);


    FILE *results = fopen("exec_time.txt", "a");
    if (results == NULL)
        exit(EXIT_FAILURE);


    int id;
    int nb = 0;
    gettimeofday(&start_while, NULL);
    #pragma omp parallel default(none) shared(current_password_to_analyse, dict_file, ds, nb, verbose) private(id)
    {
        while (current_password_to_analyse != NULL) {
            id = omp_get_thread_num();
            current_password_to_analyse = readline(ds);
            if (grep(current_password_to_analyse, dict_file, id, verbose)==1) {
                nb++;
            }
        }
    }
    gettimeofday(&end_while, NULL);

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

