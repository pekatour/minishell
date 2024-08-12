#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
  int p[2];
  int buf[1] = {0};
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
    close (p[1]);
      while ((nlus = read(p[0], buf, sizeof (int))>0)) {
        printf("Entier : %i. Code read : %i \n", buf[0], nlus);
      }
      printf("Sortie de boucle");
      /* Envoyer un signal kill au père, flemme de le faire */
      exit(0);
    }
  else {
    close(p[0]);
    int i = 0;
    while (i < 10) {
      buf[0] = i;
      int res = write(p[1], buf, sizeof (int));
      i++;
    }
    close(p[1]);
    pause();
  }
}