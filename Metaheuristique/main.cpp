#include "Data.h"
#include "Metaheuristique.h"

int main()
{
    std::cout << std::endl << "<----------Pre-execution : veuillez mettre le chemin des instances dans le code---------->"  << std::endl;

    std::string nom_fichier = "C:\\Users\\cassandre\\Desktop\\P\\P-n22-k2.vrp"; /*A-n80-k10 : 1763*/ /*A-n32-k5 : 784*/

    // Declaration + initialisation de la classe Data :
    Data data(nom_fichier);
    // Affichage des donnÈes de la classe Data :
    data.affichage();

    // Paramètre de la Métaheuristique :
    int k_proche_voisin;
    int nb_enfant;
    int iteration_Grasp;
    int iteration_ELS;
    int choix;

    std::cout << std::endl << "Vous avez choisi la methode de resolution par meta-heuristique." << std::endl;
    std::cout << "Veuillez entrer les parametres du Grasp/ELS :" << std::endl;
    std::cout << std::endl <<"Nombre de k plus proche voisin en type entier (preference entre 1-5) : ";
    std::cin >> k_proche_voisin ;
    std::cout << std::endl << "Nombre d'iteration de GRASP en type entier (preference entre 50-100) : ";
    std::cin >> iteration_Grasp;
    std::cout << std::endl << "Nombre d'iteration de ELS en type entier : (preference entre 50-++) ";
    std::cin >> iteration_ELS;
    std::cout << std::endl << "Nombre d'enfant generes par ELS en type entier (preference entre 50-++) : ";
    std::cin >> nb_enfant;
    Metaheuristique meta(data, k_proche_voisin, nb_enfant, iteration_Grasp, iteration_ELS);
    meta.run();

    return 0;
}
