
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

  if (pipe(p) == -1) {
    perror("erreur pipe");
    exit(2);
  }

  buf[0] = '2';
  int res = write(p[1], buf, sizeof(char));

  pid_t pid = fork();

  if (pid == -1) {
    perror("erreur fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    int res = read(p[0], buf, sizeof (char));
    printf("%s", buf);
  } else {

  }
}