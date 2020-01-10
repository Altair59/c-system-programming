#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include<signal.h>

int main(int argc, char **argv) {
    int i;
    int iterations;

    if (argc != 2) {
        fprintf(stderr, "Usage: forkloop <iterations>\n");
        exit(1);
    }

    iterations = strtol(argv[1], NULL, 10);

    int childCreated = 0;
    
    int stat = 0;

    for (i = 0; i < iterations; i++) {
        if(childCreated == 0) {
            childCreated = fork(); 
            
            int n = childCreated;
            if (n < 0) {
                perror("fork");
                exit(1);
            }
           if (n > 0) {
               wait(&stat);
           }
        }
        printf("ppid = %d, pid = %d, i = %d\n", getppid(), getpid(), i);
    }

    return 0;
}