#include <stdio.h>
#include <omp.h>

//int main() {
//    int id;
//#pragma omp parallel private(id)
//    {
//        id = omp_get_thread_num();
//        printf("%d: Hello World!\n", id);
//    }
//    return 0;
//}

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

// Default maximum number of simultaneous process
int MAX_FILS = 5;

void grep(char *analyse, char *file, int i) {
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
            printf("%s found by %d\n", line, i);
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
    if (argc < 2)
        fprintf(stderr, "Usage: '%s' nb_of_processus dictionnary_file shasum_file num_of_process\n", argv[0]),
                exit(EXIT_FAILURE);


    char *p;
    MAX_FILS = strtol(argv[1], &p, 10);
    char *dict_file = argv[2];
    char *shasum_file = argv[3];

// opening file
    FILE *ds = fopen(shasum_file, "r");
    if (ds == NULL)
        exit(EXIT_FAILURE);
//    int current_checker_running = 0;
    char *current_password_to_analyse = readline(ds);


    int id;
    while (current_password_to_analyse != NULL) {
#pragma omp parallel default(none) shared(dict_file, ds) private(current_password_to_analyse, id)
        {
            id = omp_get_thread_num();
//#pragma omp critical
//            {
            current_password_to_analyse = readline(ds);
//            }
//            printf("\n[INFO] Started proc with id %d searching for password %s\n", id, current_password_to_analyse);
            if (current_password_to_analyse != NULL) {
//                printf("%s est analysé par %d\n", current_password_to_analyse, id);
                grep(current_password_to_analyse, dict_file, id);
            } else {
                printf("Fin de l'analyse\n");
                exit(EXIT_SUCCESS);
            }
        }
    } // end while
#pragma omp barrier
    {
        printf("Fin de l'analyse\n");
    }
    return 0;
}

