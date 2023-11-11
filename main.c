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

// Default maximum number of simultaneous process
int MAX_FILS = 5;

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
    char *current_password_to_analyse = readline(ds);
    int current_checker_running = 0;


    int n = 0;
    int id, nbrun = 0;
    while (current_password_to_analyse != NULL) {
#pragma omp parallel default(none) shared(current_checker_running, current_password_to_analyse, dict_file, ds) private(id, nbrun)
        {
            id = omp_get_thread_num();
            nbrun ++;
//            printf("\n[INFO] Started  %dth son with id %d searching for password %s\n", nbrun, id, current_password_to_analyse);
            current_password_to_analyse = readline(ds);
            printf("%s\n", current_password_to_analyse);
//            execl("/bin/grep", "grep", current_password_to_analyse, dict_file, NULL);
            int n = fork();
            if (n < 0) {
                perror("fork error");
                exit(1);
            }
            if (n != 0) {
//                printf("\n[INFO] Started  %dth son with id %d searching for password %s\n", nbrun, id, current_password_to_analyse);
            } else {
                execl("/bin/grep", "grep", current_password_to_analyse, dict_file, NULL);
            }
            pid_t any_child;
            wait(&any_child);

        }
    } // end while
    return 0;
}

