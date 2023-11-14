#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void grep(char *analyse, char *file, int i) {
    if (analyse == NULL)
        return;
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
            printf("%s found by %d\n", line, i);
            break;
        }
    }
    fclose(f);
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
    if (argc < 2) {
        fprintf(stderr, "Usage: '%s' nb_of_processus dictionnary_file shasum_file num_of_process\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    time_t begin = time(NULL);

    char *dict_file = argv[2];
    char *shasum_file = argv[3];

    // opening file
    FILE *ds = fopen(shasum_file, "r");
    if (ds == NULL)
        exit(EXIT_FAILURE);
    char *current_password_to_analyse = readline(ds);


    int id;
#pragma omp parallel default(none) shared(current_password_to_analyse, dict_file, ds) private(id)
    {
        while (current_password_to_analyse != NULL) {
            id = omp_get_thread_num();
            current_password_to_analyse = readline(ds);
//            printf("\n[INFO] Started proc with id %d searching for password %s\n", id, current_password_to_analyse);
//            if (current_password_to_analyse != NULL) {
//                printf("%s est analysé par %d\n", current_password_to_analyse, id);
            grep(current_password_to_analyse, dict_file, id);
//            }
        }
    } // end while
    printf("Fin de l'analyse\n");
    time_t end = time(NULL);
    printf("Temps d'execution: %ld\n", end - begin);
    return 0;
}

