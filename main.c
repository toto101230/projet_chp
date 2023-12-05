#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

//wraup for readline
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
    if (argc < 3) {
        fprintf(stderr, "Usage: '%s' nb_of_processus dictionnary_file shasum_file num_of_process\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    time_t begin = time(NULL);

    char *dict_file = argv[2];
    char *shasum_file = argv[3];
    int verbose = 0;
    if (argc == 5) {
        if (strcmp(argv[4], "-v") == 0) {
            printf("Verbose mode activated\n");
            verbose = 1;
        }
    }

    // opening file
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
//            printf("\n[INFO] Started proc with id %d searching for password %s\n", id, current_password_to_analyse);
//            if (current_password_to_analyse != NULL) {
//                printf("%s est analysé par %d\n", current_password_to_analyse, id);
            if (grep(current_password_to_analyse, dict_file, id, verbose)==1) {
                nb++;
            }
//            }
        }
    } // end while
    printf("Fin de l'analyse avec %d mots de passe trouvés\n", nb);
    time_t end = time(NULL);
    printf("Temps d'execution: %ld\n", end - begin);
    return 0;
}

