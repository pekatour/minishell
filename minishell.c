#include "readcmd.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "liste.h"

/*Code de la commande avant-plan
Contient le pid de la commande en avant plan, 0 s'il n'y en a pas*/
int cmd_avant_plan;

/*Pipe précédent servant dans la gestion des commandes chaînées (avec | )*/
int pPred[2];

/*Liste chaînées des jobs*/
liste_cellule_t liste_jobs;

void erreur(int code, char* message) {
  /* Affiche le message fourni en entrée si code == -1 */
  if (code == -1) {
      printf("%s", message);
      exit(EXIT_FAILURE);
  }
}

void masquer_SIGINT_TSTP() {
  /* Masque SIGINT et SIGTSTP */
  
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGTSTP);
  sigaddset(&set, SIGINT);
  sigprocmask(SIG_SETMASK, &set, NULL);
}

void demasquer_SIGINT_TSTP() {
  /* Enlève le masque sur SIGINT et SIGTSTP */
  
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGTSTP);
  sigaddset(&set, SIGINT);
  sigprocmask(SIG_UNBLOCK, &set, NULL);
}

void gestion_interr_avp_arp(bool backgrounded) {
    /* Gestion des interruptions pour les commandes en avant-plan ou en arrière-plan */

    /* Version commentée ci-dessous avec SIG_DFL et SIGINT*/
    /*if (!backgrounded) {
      signal(SIGTSTP, SIG_DFL);
      signal(SIGINT, SIG_DFL);
    }*/

    /* Version masquage uniquement */
//    if (!backgrounded) {
//      demasquer_SIGINT_TSTP();
//    }

    /* Version avec groupes */
    demasquer_SIGINT_TSTP();
    if (backgrounded) {
        /* Changer de groupe les commande en arrière-plan les protège */
        setpgid(0,0);
    }
}

void entrees_sorties(char* in, char* out, int pIn, int pOut){
  /* Gestion des entrées sorties pour lancer_cmd */
  
  int f;
  if (pIn > 0) {
    dup2(pIn, 0);
  }
  else if (in != NULL) {
    f = open(in, O_RDONLY, NULL);
    erreur(f, "Fichier invalide après le < \n");
    dup2(f, 0);
    close(f);
  }
  if (pOut > 0) {
    dup2(pOut, 1);
  }
  else if (out != NULL) {
    f = open(out, O_TRUNC | O_WRONLY | O_CREAT, 0640);
    erreur(f , "Fichier invalide après le > \n");
    dup2(f , 1);
    close(f);
  }
}

void lancer_cmd(char **cmd, bool backgrounded, char* fIn, char* fOut, int pIn, int pOut ) {
  /* Lance une commande donnée par un tableau de mots cmd et un booléen qui
   * indique si la commande doit être lancée en arrière-plan */
  int retour = fork();
  erreur(retour, "Erreur fork\n");
  if (retour == 0) {

    gestion_interr_avp_arp(backgrounded);

    entrees_sorties(fIn, fOut, pIn, pOut);
    retour = execvp(cmd[0], cmd);

    printf("Erreur éxécution commande\n");
    exit(EXIT_FAILURE);

  } else if (!backgrounded) {
    cmd_avant_plan = retour;
    while (cmd_avant_plan) { // Attente de réception de SIGCHLD
      pause();
    }
  }
}

void lancer_cmd_pipe(char **cmd, int indexseq, bool dernier, struct cmdline *commande) {
  /* Arguments :
  - indexseq : index dans la liste chaînées des commandes en cours
  - dernier : si la commande est la dernière à éxécuter
  - commande : retour de readcmd 
  Lance la commande cmd avec pour arguments les valeurs des descripteurs de fichier des pipes où
  doit lire la commande, si c'est nécessaire (cas de commandes chaînées) */

  int p[2];
  int pIn = 0;
  int pOut = 0;
  erreur(pipe(p),"Erreur pipe");

  if (!dernier) {
    pOut = p[1];
  }

  if (indexseq > 0) { 
    close(pPred[1]);
    pIn = pPred[0];
  }

  pPred[0] = p[0];
  pPred[1] = p[1];

  lancer_cmd(cmd, commande->backgrounded, commande->in, commande->out, pIn, pOut);
}

void traitement_SIGCHLD(int numsig) {
  /* Traitement du signal SIGCHLD :
  Affiche le changement d'état du processus fils*/
  (void) numsig; // évite un warning
  int retour;
  int status;
  do {
    retour = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED);

    if (retour > 0) {
      if (WIFSTOPPED(status)) {
        printf("Processus %d mis en pause par SIGCONT\n", retour);
      }
      if (WIFCONTINUED(status)) {
        printf("Processus %d redémarré\n", retour);
      }
      if (WIFEXITED(status)) {
        printf("Processus %d terminé avec le statut %d\n", retour, WEXITSTATUS(status));
      }
      if (WIFSIGNALED(status)) {
        printf("Processus %d terminé par le signal %d\n", retour, WTERMSIG(status));
      }
    }

    if (retour == cmd_avant_plan) {
      cmd_avant_plan = 0;
    }
  } while (retour > 0);
}

void def_SIGCHLD_traitement() {
  /* Création du traitement du signal SIGCHLD */

  struct sigaction action;
  action.sa_handler = traitement_SIGCHLD;
  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_RESTART;

  sigaction(SIGCHLD, &action, NULL);
}

int main(void) {

  /* Créations des traitements des signaux */
  def_SIGCHLD_traitement();

  /* Version commentée ci-dessous avec SIG_DFL et SIGINT*/
  /*signal(SIGTSTP, SIG_IGN);
  signal(SIGINT, SIG_IGN);*/

  /* Version masquage uniquement + groupes */
  masquer_SIGINT_TSTP();

  bool fini = false;

  while (!fini) {
    printf("> ");
    struct cmdline *commande = readcmd();

    if (commande == 0) {
      // commande == NULL -> erreur readcmd()
      perror("erreur lecture commande \n");
      exit(EXIT_FAILURE);

    } else {

      if (commande->err) {
        // commande->err != NULL -> commande->seq == NULL
        printf("Erreur saisie de la commande : %s\n", commande->err);

      } else {
          int indexseq = 0;
          char **cmd;
          bool dernier;
          while ((cmd = commande->seq[indexseq])) {
            dernier = commande->seq[indexseq+1] == 0;
            if (cmd[0]) {
              if (strcmp(cmd[0], "exit") == 0) {
                fini = true;
                printf("Au revoir ...\n");
              } else {
                lancer_cmd_pipe(cmd, indexseq, dernier, commande);
              }
              indexseq++;
            }
          }
        }
      }
    }
  return EXIT_SUCCESS;
}
