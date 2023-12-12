// multiattack.c
// Starts multiple instances checking if a password can be found in the dictionnary.
// usage : nb_of_processus dictionnary_file shasum_file


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

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
    struct timeval start, end;
    struct timeval start_while, end_while;
    gettimeofday(&start, NULL);
    FILE *results = fopen("exec_time.txt", "a");
    if (results == NULL)
        exit(EXIT_FAILURE);

    if (argc < 2)
        fprintf(stderr, "Usage: '%s' nb_of_processus dictionnary_file shasum_file num_of_process\n", argv[0]), exit(
                EXIT_FAILURE);
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


    gettimeofday(&start_while, NULL);
    int n = 0;
    while (current_password_to_analyse != NULL) {
        for (; current_checker_running < MAX_FILS;) { // forking until reaching MAX_FILS sons
            if ((n = fork()) < 0) {
                perror("fork error");
                exit(1);
            }
            if (n != 0) {//Father code, so I'm couting one more son

                current_checker_running++;
                printf("\n[INFO] Started  %dth son searching for password %s\n", current_checker_running,
                       current_password_to_analyse);
                current_password_to_analyse = readline(ds);

            } else { // son code.
                execl("/bin/grep", "grep", current_password_to_analyse, dict_file, NULL);
                // There's no return from execl, remember it ?
            }

        } // for end. So I should have more or less MAX_FILS sons, until one's returning ...
        pid_t any_child;
        wait(&any_child);
        printf("One of my son has finished\n. I'll start another one if I need to...");
        // one son has terminated
        current_checker_running--;
    } // end while


    gettimeofday(&end_while, NULL);
    gettimeofday(&end, NULL);
    float parallel_exec_time =
            ((end_while.tv_sec * 1000000 + end_while.tv_usec) - (start_while.tv_sec * 1000000 + start_while.tv_usec)) /
            1000000.0;
    float total_exec_time =
            ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000.0;

    // Ecriture des résultats dans le fichier exec_time.txt
    fprintf(results, "%d\t%f\t%f\n", MAX_FILS, total_exec_time, parallel_exec_time);
    fclose(results);
    return 0;
}

