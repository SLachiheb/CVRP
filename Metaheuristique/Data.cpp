#include "Data.h"
#include <cmath>

Data::Data(const Data &data) {
    /*Ici, on ne vÈrifie pas que la clef est dÈj‡ dans la Map, car on suppose le fichier valide*/
    this->nom = data.nom;
    this->capacite = data.capacite;
    this->nb_vehicules = data.nb_vehicules;
    this->dimension = data.dimension;
    this->coordonnee = data.coordonnee;
    this->demande_client = data.demande_client;
    this->distance_ij = data.distance_ij;
    this->opt = data.opt;
}

Data::Data(std::string nom_fichier) {
    Data::lecture_fichier(nom_fichier);
    Data::matrice_distance_ij();
}

double Data::calcul_distance(const std::pair<int, int> &coord1, const std::pair<int,int> &coord2) {
    return sqrt(pow(coord2.first - coord1.first,2) + pow(coord2.second - coord1.second,2));
    //return sqrt((coord2.first - coord1.first)*(coord2.first - coord1.first) + (coord2.second - coord1.second)*(coord2.second - coord1.second));
}

void Data::matrice_distance_ij() {
    this->distance_ij = std::vector< std::vector<double>>(this->dimension+1);
    for (int i(0); i <= this->dimension; i++) {
        this->distance_ij[i] = std::vector<double>(this->dimension+1);
        for (int j(0); j <= this->dimension; j++) {
            if (j == 0) {
                this->distance_ij[i][j] = 0;
            } else {
                this->distance_ij[i][j] = calcul_distance(this->coordonnee[i], this->coordonnee[j]);
            }
        }
    }
}

void Data::affichage() {
    std::cout << "Nom fichier : " << this->nom << std::endl;
    std::cout << "Nombre de vehicule : " << this->nb_vehicules<< std::endl;
    std::cout << "L'optimum de la solution : " << this->opt << std::endl;
    std::cout << "Nombre de sommet (depot+client) : " << this->dimension << std::endl;
    std::cout << "Capacite : " << this->capacite << std::endl;
    std::cout << "Liste de sommet avec leurs coordonnees: " << std::endl;
    for (int i(1); i <= this->dimension; i++) {
        std::cout << i << " " << this->coordonnee[i].first << " " << this->coordonnee[i].second << std::endl;
    }

    std::cout << "Liste de demmande requise par sommet : " << std::endl;
    /*for (int i(1); i <= this->dimension; i++) {
     std::cout << i << " " << this->demande_client[i] << std::endl;
     }*/
    std::cout << "{";
    for (int i(1); i <= this->dimension; i++) {
        if (i == this->distance_ij[i].size()-1)
            std::cout << "{" << i << ", " << this->demande_client[i] << "}" << std::endl;
        else
            std::cout << "{" << i << ", " << this->demande_client[i] << "}, " << std::endl;
    }
    std::cout << "}";


    std::cout << "Matrice de distance : " << std::endl;
    std::cout << "{";
    for (int i(0); i < this->distance_ij.size(); i++) {
        std::cout << "{";
        for (int j(0); j < this->distance_ij[i].size(); j++) {
            if (j == this->distance_ij[i].size()-1)
                std::cout << this->distance_ij[i][j];
            else
                std::cout << this->distance_ij[i][j] << ", ";
        }
        if (i == this->distance_ij[i].size()-1)
            std::cout << "}";
        else
            std::cout << "}, " << std::endl;
    }
    std::cout << "};" << std::endl;
}

void Data::split(const std::string &chaine, char delimiteur, std::vector<std::string> &elements)
{
    std::stringstream ss(chaine);
    std::string sousChaine;
    while (std::getline(ss, sousChaine, delimiteur))
    {
        elements.push_back(sousChaine);
    }
}

std::vector<std::string> Data::split(const std::string &chaine, char delimiteur)
{
    std::vector<std::string> mots;
    split(chaine, delimiteur, mots);
    return mots;
}

Options Data::resolution_enum(std::string input) {
    if (input == "NAME") return NAME;
    if (input == "COMMENT") return COMMENT;
    if (input == "TYPE") return TYPE;
    if (input == "DIMENSION") return DIMENSION;
    if (input == "EDGE_WEIGHT_TYPE") return EDGE_WEIGHT_TYPE;
    if (input == "CAPACITY") return CAPACITY;
    if (input == "NODE_COORD_SECTION") return NODE_COORD_SECTION;
    if (input == "DEMAND_SECTION") return DEMAND_SECTION;
    return DEFAULT;
}

void  Data::lecture_fichier(std::string const nom_fichier) {
    std::ifstream fichier;

    fichier.open(nom_fichier.c_str());

    if (fichier.is_open()) {
        std::string ligne;

        while (std::getline(fichier, ligne)) {
            std::vector<std::string> mots = split(ligne, ' ');
            switch (resolution_enum(mots[0])) {
                case NAME:
                    this->nom = mots[2];
                    break;
                case COMMENT:
                    this->nb_vehicules = stoi(mots[8]);
                    this->opt = stoi(mots[11]);
                    break;
                case TYPE:
                    break;
                case DIMENSION:
                    this->dimension = stoi(mots[2]);
                    break;
                case EDGE_WEIGHT_TYPE:
                    break;
                case CAPACITY:
                    this->capacite = stoi(mots[2]);
                    break;
                case NODE_COORD_SECTION:
                    for (int i(1); i <= this->dimension; i++) {
                        getline(fichier, ligne);
                        std::vector<std::string> mots = split(ligne, ' ');
                        if (this->nom[0] !=  'A' ) {
                            this->coordonnee[stoi(mots[0])] = std::make_pair(stoi(mots[1]), stoi(mots[2]));
                        }
                        else {
                            this->coordonnee[stoi(mots[1])] = std::make_pair(stoi(mots[2]), stoi(mots[3]));
                        }
                    }
                    break;
                case DEMAND_SECTION:
                    for (int i(1); i <= this->dimension; i++) {
                        getline(fichier, ligne);
                        std::vector<std::string> mots = split(ligne, ' ');
                        this->demande_client[stoi(mots[0])] = stoi(mots[1]);
                    }
                    break;
                case DEFAULT:
                    break;
            }
        }
    }
    else {
        std::cout << "Erreur : Impossible d'ouvrir le fichier en lecture" << std::endl;
    }
}

/****************** Accesseur ******************/

int Data::getDimension() {
    return this->dimension;
}

double Data::getDistance_ij(int i, int j) {
    //std::cout << i << " Distance " << j <<std::endl;
    return this->distance_ij[i][j];
}

std::map<int, std::pair<int, int> > Data::getCoordonnee()
{
    return this->coordonnee;
}

std::map<int, int> Data::getDemande_client()
{
    return this->demande_client;
}

int Data::getDemande_client(int id) {
    return this->demande_client[id];
}

int Data::getCapacite()
{
    return this->capacite;
}

int Data::getNb_vehicules() {
    return this->nb_vehicules;
}

double Data::getOPT() {
    return this->opt;
}

int   Data::getIdDepot() {
    return 1; // Definit de maniere arbitraire
}
