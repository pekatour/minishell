//
// Created by pekatour on 29/04/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

void erreur(int code, char* message) {
    if (code == -1) {
        printf("%s", message);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
    if (argc !=3) {
        erreur(-1, "Erreur \n Appeler sous la forme: copier source destination");
    }
    else {

        char tab[BUFSIZ];

        int source = open(argv[1],O_RDONLY);
        erreur(source, "Fichier introuvable");

        int dest = open(argv[2], O_TRUNC | O_CREAT | O_WRONLY, 0640);
        erreur(dest, "Fichier introuvable");

        ssize_t lus;
        while ((lus = read(source, tab, BUFSIZ)) > 0) {
            write(dest, tab, lus);
            printf("coucou");
        }
        close(source);
        close(dest);
    }
}
