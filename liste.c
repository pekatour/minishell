#include "liste.h"
#include <stdlib.h>

struct cellule_t {
    int id;
    int pid;
    char *etat;
    char **cmd;
    struct cellule_t *suivant;
};

typedef struct cellule_t cellule_t;

struct liste_cellule_t {
    cellule_t *tete;
    cellule_t *queue;
};

liste_cellule_t *creer_liste() {
    liste_cellule_t *liste = malloc(sizeof(liste_cellule_t));
    liste->tete = NULL;
    liste->queue = NULL;
    return liste;
}

void detruire_liste(liste_cellule_t **liste_ptr) {
    cellule_t *curseur = (*liste_ptr)->tete;
    while (curseur != NULL) {
        cellule_t *tmp = curseur;
        curseur = curseur->suivant;
        free(tmp);
    }
    free(*liste_ptr);
    *liste_ptr = NULL;
}

bool est_vide_liste(const liste_cellule_t *liste) {
    return liste->tete == NULL;
}

bool contient_cellule_liste(const liste_cellule_t *liste, int id) {
    cellule_t *curseur = liste->tete;

    while (curseur != NULL && curseur->id != id) {
        curseur = curseur->suivant;
    }

    return curseur != NULL;
}

void inserer_cellule_liste(liste_cellule_t* liste, int id, char *etat, char **cmd) {
    cellule_t *nouvelle_cellule = (cellule_t* )malloc(sizeof(cellule_t));
    nouvelle_cellule->id = id;
    nouvelle_cellule->etat = etat;
    nouvelle_cellule->cmd = cmd;
    nouvelle_cellule->suivant = NULL;

    if (liste->queue == NULL) {
        liste->tete = nouvelle_cellule;
        liste->queue = nouvelle_cellule;
    } else {
        liste->queue->suivant = nouvelle_cellule;
        liste->queue = nouvelle_cellule;
    }

}

void changer_noeud_liste(liste_cellule_t* liste, int id, char *etat, char **cmd) {
    cellule_t *curseur = liste->tete;

    while (curseur != NULL && curseur->id != id) {
        curseur = curseur->suivant;
    }

    if (curseur != NULL) {
        curseur->id = id;
        curseur->etat = etat;
        curseur->cmd = cmd;
    }
    else {
        inserer_cellule_liste(liste, id, etat, cmd);
    }
}

void supprimer_cellule_liste(liste_cellule_t* liste, int id) {
    cellule_t *curseur = liste->tete;
    cellule_t *precedent = NULL;

    while (curseur != NULL && curseur->id != id) {
        precedent = curseur;
        curseur = curseur->suivant;
    }

    if (curseur != NULL) {
        if (precedent == NULL) {
            liste->tete = curseur->suivant;
        } else {
            precedent->suivant = curseur->suivant;
        }

        if (curseur == liste->queue) {
            liste->queue = precedent;
        }

        free(curseur);
    }
}


