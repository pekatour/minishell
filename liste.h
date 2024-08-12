#include <stdbool.h>

struct liste_cellule_t;

// typedef
typedef struct liste_cellule_t liste_cellule_t;

/**
 * creer_liste : crée une liste de cellules, initialement vide
 *
 * Post-conditions : `r = creer_liste()` => `r != NULL`, `est_vide_liste(r)`
 * @return liste nouvellement créée (de type liste_cellule_t)
 */
liste_cellule_t* creer_liste();

/**
 * detruire_liste : détruit la liste passée en paramètre
 *
 * Pré-conditions : liste_ptr != NULL
 * Post-conditions : *liste_ptr == NULL
 *
 * @param liste_ptr pointeur sur la liste à détruire
 */
void detruire_liste(liste_cellule_t** liste_ptr);

/**
 * est_vide_liste : test si la liste passée en paramètre est vide
 *
 * Pré-conditions : liste != NULL
 *
 * @param liste [in] liste à tester
 * @return vrai ssi la liste ne contient aucun élément
 */
bool est_vide_liste(const liste_cellule_t* liste);

/**
 * contient_cellule_liste : test si le cellule donné appartient à la liste donnée.
 * 
 * Pré-conditions : liste != NULL
 *
 * @param liste [in] liste à parcourir
 * @param cellule cellule à rechercher
 * @return vrai ssi cellule est dans liste
 */
bool contient_cellule_liste(const liste_cellule_t* liste, int cellule);

/**
 * inserer_cellule_liste : insère le cellule donné dans la liste
 *
 * Pré-conditions : liste != NULL
 *
 * @param liste [in,out] liste dans laquelle insérer l'élément
 * @param id identifiant à insérer
 * @param etat etat de la commande
 * @param cmd commande
 */
void inserer_cellule_liste(liste_cellule_t* liste, int id, char *etat, char **cmd);

/**
 * changer_noeud_liste : modifie les valeurs associées à la cellule donné dans la liste donnée.
 * Si la cellule n'est pas dans la liste, ellle est ajoutée.
 *
 * Pré-conditions : liste != NULL
 *
 * @param liste [in,out] liste à modifier
 * @param id
 * @param etat
 * @param cmd
 */
void changer_noeud_liste(liste_cellule_t* liste, int id, char *etat, char **cmd);

/** 
 * supprimer_cellule_liste : supprime le cellule donné de la liste. Si le cellule n'est pas dans la liste,
 * ne fait rien.
 *
 * Pré-conditions : liste != NULL
 * Post-conditions : après `supprimer_cellule_liste(liste, n)` : `!contient_cellule_liste(liste, n)`
 *
 * @param liste [in,out] liste à modifier
 * @param id id à supprimer de liste
 */
void supprimer_cellule_liste(liste_cellule_t* liste, int id);