#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256
#define MAX_PASSWORD 10

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];

  /* The user will type in a user name on one line followed by a password 
     on the next.
     DO NOT add any prompts.  The only output of this program will be one 
	 of the messages defined above.
   */

  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  if(fgets(password, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  
  // create pipe
  int fd[2];
  if (pipe(fd) == -1){
    perror("pipe");
    exit(1);
  }

  // create child
  int r = fork();
  if (r == -1){ // system error call
    perror("fork");
    exit(1);
  } else if (r == 0){ // child process
    dup2(fd[0],fileno(stdin));
    // error closing pipe
    if (close(fd[0]) == -1){
      perror("child close read");
      exit(1);
    }
    if (close(fd[1]) == -1){
      perror("child close write");
      exit(1);
    }
    // read from stdin
    execl("./validate", "validate", NULL);
    perror("execl");
    exit(1);
  } else if (r > 0){
    int status;
    if (close(fd[0]) == -1){
      perror("parent close read");
      exit(1);
    }
    // parent process
    if(write(fd[1], user_id, MAX_PASSWORD) == -1) {
        perror("write");
    }
    if(write(fd[1], password, MAX_PASSWORD) == -1) {
        perror("write");
    }
    if(close(fd[1]) == -1) {
        perror("parent close write");
        exit(1);
    }
    //check exit status and print message
    if (wait(&status) == -1){
      perror("wait");
      exit(1);
    } else {
      if(WIFEXITED(status)) {
        if(WEXITSTATUS(status) == 0) {
           printf(SUCCESS);
        } else if(WEXITSTATUS(status) == 1) {
            exit(1);
        } else if(WEXITSTATUS(status) == 2) {
            printf(INVALID);
        } else if(WEXITSTATUS(status) == 3) {
            printf(NO_USER);
        }
      } else if (WIFSIGNALED(status)){
        fprintf(stderr, "%s\n", "child process terminated due to signal");
      }
    }
  }

  return 0;
}
