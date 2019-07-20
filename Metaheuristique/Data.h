#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <vector>


/*
 L'enumeration Options est utilis»e lors de la lecture d'un fichier.vrp.
 Chaque enumeration correspondant á une information pertinente du fichier.vrp.
 */
enum Options {
    NAME,                    /*nom du fichier.vrp*/
    COMMENT,                /*contient le nombre de vehicule*/
    TYPE,                    /*type du fichier lu, ici vrp*/
    DIMENSION,                /*nombre de client et d'entrepot confondu (Dans le projet, il y a seulement 1 depot)*/
    EDGE_WEIGHT_TYPE,        /*precise qu'on est dans un graphe 2D*/
    CAPACITY,                /*quantite max de commande transpot»e par un v»hicule*/
    NODE_COORD_SECTION,        /*coordonnee de chaque client et du depot*/
    DEMAND_SECTION,            /*quantit» de chaque commande du client et du depot ici 0*/
    DEFAULT                    /*information pr»sente dans le fichier non n»cessaire dans le projet*/
};

class Data {
    /*
     La structure Donnee contient les attributs cl»s extraits du fichier VRP :
     - nom_fichier    : nom du fichier absolu avec chemin.
     - nom             : nom du fichier VRP.
     - nb_vehicule    : nombre de vehicule disponible pour les tournees.
     - dimension      : le nombre de sommet du graphe complet (depot + clients).
     - capacite       : capacite de chaque vehicule.
     - coordonnes     : contient pour chaque client ses coordonnees x et y;
     - demande_client : contient pour chaque client, la quantite de produit command» par celui-ci au transporteur d'un vehicule.
     */
    private :    // Attribut
    std::string                            nom;
    int                                    nb_vehicules;
    int                                    dimension;
    int                                    capacite;
    std::map<int, std::pair<int, int> >    coordonnee;            /*numero client : (coordonnee x, coordonne y)*/
    std::map<int, int>                     demande_client;        /*numero client : quantite command»*/
    std::vector<std::vector<double> >      distance_ij;        /*matrice de distance a 2d entre les differents sommets*/
    double                                 opt;

    public :    // Methode

    /*
     Contrusteur par recopie.
     */
    Data(const Data &);

    /*
     Constructeur usuel, initialisant les attributs avec les donnees du nom fichier passer en parametre.
     - nom_fichier    : nom du fichier á lire.
     */
    Data(std::string nom_fichier);

    /*
     Fonction matrice_distance_ij permet d'initialise distance_ij.
     */
    void matrice_distance_ij();

    /*
     La fonction affichage permet d'afficher le contenu de tout les attributs de la classe Data.
     */
    void affichage();

    /*Accesseur*/
    int                                     getDimension();
    std::map<int, std::pair<int, int> >     getCoordonnee();
    std::map<int, int>                      getDemande_client();
    int                                     getCapacite();
    double                                  getDistance_ij(int, int);
    int                                     getDemande_client(int id);
    int                                     getNb_vehicules();
    double                                  getOPT();
    int                                     getIndiceDepot();

    /*
     Fonction calcul_distance fonctionne avec matrice_distance_ij comme base de calcule pour calculer la distance.
     */
    double calcul_distance(const std::pair<int, int> &, const std::pair<int,int> &);

    /*
     * Retourne le numÈro du sommet du depot
     */
    int getIdDepot();

    private :    // Methode
    /*
     La fonction lecture fichier permet de lire un ficher de type fichier.vrp et d'initialiser la structure de donn»e du ficher.vrp.
     - nom_fichier     : correspondant au fichier á lire.
     - donnee_fichier : une structure de type Donnee á initialiser.
     */
    void  lecture_fichier(std::string const nom_fichier);

    /*
     La fonction split permet de prendre une chaine de caractÀre en paramÀtre et de la d»couper en fonction d'un d»limitateur,
     pour initialiser un tableau avec les mots de la chaine d»coup»e.
     - chaine     : correspondant á la phrase qu'on souhaite d»coup»e.
     - delimiteur : caractÀre de s»paration de mot d'une phrase.
     - elements     : correspondant á un tableau dynamique initialis» avec des mots de type string.
     */
    void split(const std::string &chaine, char delimiteur, std::vector<std::string> &elements);

    /*
     La fonction split retourne un tableau dynamique vector contenant dans chaque case un mot de type string d'une chaine.
     - chaine     : chaine de la phrase á d»couper.
     - delimiteur : caractÀre de s»paration de mot d'une chaine.
     */
    std::vector<std::string> split(const std::string &chaine, char delimiteur);

    /*
     La fonction resolution_enum permet de faire la conversion d'une chaine de caractÀre en une »num»ration.
     - input    : chaine de caractÀre á transformer en une »num»ration.
     */
    Options resolution_enum(std::string input);
};


#endif //DATA_H
