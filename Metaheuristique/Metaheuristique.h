#ifndef METAHEURISTIQUE_H
#define METAHEURISTIQUE_H

#include "Heuristique.h"
#include <ctime>

class Metaheuristique {

    private :
    Heuristique                     heuristique_main;
    Heuristique                     best_heuristique;
    std::deque<Heuristique >        enfants;
    std::deque<Heuristique >        rcl_grasp;
    int                             k_proche_voisin;
    int                             nb_enfant_par_mut;
    int                             iteration_Grasp;
    int                             iteration_ELS;
    int                             index_iterations;
    std::list<double>               list_cost;
    std::list<double>                list_time;
    std::list<int>                  list_iterations;

    public :
    /*
     *Constructeur usuel tout en un :
     *   - Data
     *   - k_proche_voisin
     *   - nb_enfant_par_mutation
     *   - iteration_Grasp
     *   _ iteration_ELS;
     */
    Metaheuristique(Data, int, int, int, int);

    /*
     *Methode principale de Metaheuristique
     */
    void run ();

    /*
     *Partie Mutation de l'ELS
     */
    void mutation_enfants();

    /*
     *Partie Split et Recherche Locale via l'ELS
     */
    void split_rechercheLocale_enfants();

    /*
     *Partie selection du meilleurs enfant ELS avec mise a jour Heuristique_main
     */
    void selection_meilleur_enfant();

    /*
     *Sauvegarde des heuristiques_main
     */
    bool _add_rcl_GRASP(double);

    /*
     *Initialise l'attribut bestHeuristique avec la meilleur heuristique du sac RCL faisable du GRASP ????
     */
    void _initBestRclGrasp();

    /*
     *Rénitialisation de tout les attributs de l'heuristique main
     */
    void _reset_HeuristiqueMain ();

    /*
     *La faisailité de la solution revoie True si l'heuristique est conforme au contrainte sinon False
     */
    bool faisabilite(Heuristique &h);

    /*
     *Vérifie si chaque tournée respect la capacité du véhicule
     */
    bool check_capacity(Heuristique &h);

    /*
     *Affichage des tournées avec la distance parcourue;
     */
    void debbug_tournee(Heuristique &);

    /*
     *Accesseur
     */
    /*Retourne le cout de la meilleur heuristique*/
    double                          getBestCout();
    /*Retourne l'ensemble des tournées de la meilleure solution*/
    std::vector<std::vector<int>>   getBestSolution();

    /*
    *
    */
    void print_Cost_Time();

};

#endif //METAHEURISTIQUE_H

