#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
  int p[2];
  int N = 2;
  char buf[N];
  int nlus;

  if (pipe(p) == -1) {
    perror("erreur pipe");
    exit(2);
  }

  pid_t pid = fork();

  if (pid == -1) {
    perror("erreur fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0){
    char tab[10*N];
    close (p[1]);
    signal(SIGTSTP, SIG_DFL);
    pause();
    nlus = read(p[0], tab, 10 * N * sizeof (char));
    exit(0);
  }
  else {
    close(p[0]);
    while (1) {
      int res = write(p[1], buf, N * sizeof(char));
      sleep(1);
      printf("%i",res);
    }
  }
}