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

    struct timeval start, end;
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


    int id;
    int nb = 0;
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
    printf("Fin de l'analyse avec %d mots de passe trouvés\n", nb);
    gettimeofday(&end, NULL);
    printf("Temps d'execution en ms: %ld\n", ((end.tv_sec * 1000000 + end.tv_usec)
                                        - (start.tv_sec * 1000000 + start.tv_usec)));
    // Temps en secondes avec 6 chiffres après la virgule
    printf("Temps d'execution en s: %f\n", ((end.tv_sec * 1000000 + end.tv_usec)
                                           - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000.0);
    return 0;
}

