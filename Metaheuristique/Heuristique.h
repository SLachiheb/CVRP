#ifndef HEURISTIQUE_H
#define HEURISTIQUE_H

#include "Data.h"
#include <cmath>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <list>
#include <deque>


class Heuristique {

private:    // ATTRIBUT
    Data                                data;                        // Donnee du probleme.
    std::vector<int>                    tour_geant_T;               // conteneur de tour geant T (=Solution partielle)
    std::vector<std::vector<int>>       tournee_S;                    // conteneur de tournee S (=Solution complÀte).
    double                              cout_;                      // distance minimum du parcours des vehicules.

public:     // METHODE PUBLIC
    /*
     *Constructeur usuel prend un argument de type Data.
     *    - initialisation de la variable Data avec le parametre du constructeur de type Data.
     *    - tour_geant_T reste vide.
     *    - tournee_S reste vide.
     */
    Heuristique(Data);

    /*
     *Constructeur de recopie.
     */
    Heuristique(const Heuristique &);

    /*
     Fonction route_first_cluster_second qui permet d'initialiser les attributs de la classe
     Heuristique :
     - tour_geant_T : resolution approche du probleme du TSP.
     - tournee_S    : resolution approche du probleme du CVRP.
     */
    void route_first_cluster_second(const int &);

    /*
     * Fonction de concatenation des tournees dans l'attribut de tournee_S :
     */
    void concatenation_tournee_S();

    /*
     Fonction algorithmique Split permet de couper un tour geant en plusieurs tournees.
     */
    void split();

    /*
     Fonction recherche Locale :
     */
    void rechercheLocal();

    /*
     Fonction mutation
     */
    void mutation ();

    /*
     ASSESSEUR :
     */
    double getCout_();
    std::vector<std::vector<int>>  getTournee_S ();
    std::vector<int> getTour_geant_T();
    Data getData();

    /*
     MODIFICATEUR :
     */
    void setCout(double);
    void setTournee_S_clear();
    void setTour_geant_clear();

    private : // METHODE PRIVATE
    /*
     Type de transformation de recherche locale :
     - 2-opt intra, inter et swap_externe.
     */
    void rechercheLocale_2_opt_intra_tournee();
    void rechercheLocale_2_opt_inter_tournee();
    void rechercheLocale_swap_externe();

    /*
     Fonction recherche locale permet de visit» le vosinage de la solution S.
     */
    bool voisinage_2_opt_intra(std::vector<int> &);
    int mod ( int x , int y );
    bool voisinage_2_opt_inter(int , int );
    bool voisinage_swap_externe(std::deque<int> &, std::deque<int> &);

    /*
     La fonction distance_sommet_pivot retourne une liste de client qui se trouve le plus proche du client pivot,
     puis met a jour la liste de client en le supprimant de la liste.
     - un sommet pivot a partir du quel on souhaite calculer la distante aux autres points.
     - une liste de client non traite.

     */
    std::vector<std::pair<int, double>> distance_sommet_pivot(const int &, std::list<int> &);

    /*
     La fonction cinq_plus_proche_voisin qui construit un TSP a partir des k plus proche voisin.
     */
    void k_plus_proche_voisin(const int &);

    /*
     Fonction extraction_split permet de mettre a jour la solution complete (ensemble de tournees)
     */
    void extraction_split(const std::vector<std::pair<int, int>> &);

    /*
     La fonction calcul permet de calculer la distance entre deux coordonnees de sommet.
     - coord1   :   sommet 1 contenant le x et le y.
     - coord2   :   sommet 2 contenant le y et le y.
     */
    double calcul_distance(const std::pair<int, int> &, const std::pair<int,int> &);

    /*
     La fonction swap_arc est utilise par la recherche locale 2-opt inter ???
     */
    void swap_arc(std::deque<int> &, int &, int &);

    /*
     La fonction removeTourneeVide supprimer les tournees vide a la fin de la serie de recherche locale
     */
    void removeTourneeVide();

    /*
     -------------------------------------FONCTION DE DEBBUG :
     */
    /*
     Fonction de debbug pour lister une liste de type entier
     */
    void debbug_tour_geant_T(const std::vector<int> &liste);

    /*
     Fonction de debbug pour afficher tout les distantes entre chaque sommet client + depot
     */
    void debbug_distance();

    /*
     Fonction de debbug affichage : les couts des arc (i,j) de c_ij.
     */
    void debbug_cij(const std::map<std::pair<int, int>, double> &);

    /*
     Fonction de debbug affichage : identifiant de sommet avec la distance de plus court chemin entre le depot et client i.
     */
    void debbug_v_i(const std::vector<std::pair<int, double>> &);

    /*
     Fonction de debbug affichage : correspond au d»but de la tourn»e á laquelle appartient le client dÌindice i du tour g»ant.
     */
    void debbug_p_i(const std::vector<std::pair<int, int>> &);

    /*
     Fonction debbug_s_i affichage : correspondant a l'affichage du tour geant decoupe en tournee.
     */
    void debbug_s_i(const std::vector<std::vector<int>> &);

    /*

     */
    void debbug_tournee_S(const std::vector<std::vector<int>> &);

    /*

     */
    void debbug_deque(const int &i, std::deque<int> &tab);

    /*

     */
    void debbug_heuristique ();

};

#endif //HEURISTIQUEGRASP_H

