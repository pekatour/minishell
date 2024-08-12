
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
  int p[2];
  char buf[1] = {'1'};
  int nlus;

  pid_t pid = fork();

  if (pipe(p) == -1) {
    perror("erreur pipe");
    exit(2);
  }

  if (pid == -1) {
    perror("erreur fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0){
    close (p[1]);
    while(1){
      while ((nlus = read(p[0], buf, sizeof (char))>0)) {
        printf("%s\n", buf);
      }
    }
  }
  else {
    close(p[0]);
    while (1) {
      buf[0] = '2';
      int res = write(p[1], buf, sizeof (char));
      sleep(10);
    }
  }
}