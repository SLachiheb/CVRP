#include "Data.h"
#include "PLNE_vrp.h"

int main()
{
    //std::string nom_fichier = "/home/cassandre/Projet_MAOA/instances/testn10.vrp";
    //std::string nom_fichier = "/home/cassandre/Projet_MAOA/instances/Vrp-Set-A/A\\A-n32-k5.vrp";
    //std::string nom_fichier = "/home/cassandre/Projet_MAOA/instances/Vrp-Set-P/P/P-n76-k4.vrp";
    //std::string nom_fichier = "/home/cassandre/Projet_MAOA/instances/Vrp-Set-P/P/P-n16-k8.vrp";
    std::string nom_fichier = "/home/cassandre/Projet_MAOA/instances/Vrp-Set-P/P/P-n22-k2.vrp";

    // Declaration + initialisation de la classe Data :
    Data data(nom_fichier);
    // Affichage des donnÈes de la classe Data :
    data.affichage();

    int choix;
    cout << endl << "Méthode de résolution exacte." << endl;
    cout << "Veuillez choisir le type de PLNE :" << endl;
    cout << "CVRP à 3 indices version non-orienté                                 (Tapez 1)" << endl;
    cout << "CVRP à 2 indices version orienté avec formulation par les coupes     (Tapez 2)" << endl;
    cout << "CVRP à 2 indices version orienté avec MTZ                            (Tapez 3)" << endl;
    cin >> choix;
    // Declaration d'un solveur
    PLNE_vrp solveur_vrp(data);
    // Execution du solveur
    solveur_vrp.run(choix);


    return 0;
}
