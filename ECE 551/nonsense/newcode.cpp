#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

int main(int argc, char * argv[]) {
  pid_t pid;
  int pipefd[2];
  int status;

  if (argc != 3)
    std::cerr << "usage: cmd need 3 paraments" << std::endl;

  if (pipe(pipefd) < 0) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  if ((pid = fork()) < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  else if (pid == 0) {
    close(pipefd[0]);
    if (dup2(pipefd[1], 1) == -1) {  //
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    close(pipefd[1]);

    execlp(argv[1], argv[1], NULL);
    perror("execlp");
    exit(EXIT_FAILURE);
  }

  else {
    waitpid(pid, &status, 0);

    close(pipefd[1]);
    if (dup2(pipefd[0], 0) == -1) {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    close(pipefd[0]);

    execlp(argv[2], argv[2], NULL);
    perror("execlp");
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);
}
