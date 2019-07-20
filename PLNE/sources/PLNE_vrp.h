//
//  PLNE_vrp.h
//  cplex_xcode_linking
//
//  Created by Cassandre Leroy on 09/12/2018.
//  Copyright © 2018 Cassandre Leroy. All rights reserved.
//

#ifndef PLNE_vrp_h
#define PLNE_vrp_h

#include "Data.h"

#include "Graph_lemon.h"
#include "Graph_lemon_directed.h"
#include "DetecteCycle.h"
#include "DetecteCycle_directed.h"

#include <ilcplex/ilocplex.h>

typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<NumVarMatrix>   NumVar3Matrix;

enum Type_oriente {
    CVRP_UNDIRECTED,
    CVRP_ORIENTE_WITHOUT_MTZ,
    CVRP_DIRECTED_WITHOUT_MTZ_3_INDEX,
    CVRP_ORIENTE_WITH_MTZ,
    DEFAULTS
};

class PLNE_vrp {

    private :

    Data                      _data;
    //Metaheuristique           _meta_init;
    int                       _id_depot, _depot_inclus, _depot_exclus;
    /*Undirected*/
    vector<vector <int>>      _indexEdge;
    vector<vector <int>>      _delta_i;
    map<int, double>          _ce;
    map<int, pair<int, int>>  _indexNode;
    int                       _taille_xe;
    Graph_lemon_directed      _graph_undirected;

    public :
    /*
     * Constructeur usuel : initialisation de l'attribut data.
        - data : contient la structure de donnees du fichier.vrp.
     */
    PLNE_vrp(Data);

    /*
    * Construction d'un tableau index (i, j) vers un index d'arc.
    */
    void indexIJtoEdge ();
    void print_indexIJtoEdge ();

    /*
    * Création des variables pour le model cvrp non-orienté 3 indices.
    */
    NumVarMatrix  add_variable_Undirected_cvrp_x_ek(IloEnv &);
    NumVarMatrix  add_variable_Undirected_cvrp_y_ik(IloEnv &);

    /*
    * Déclaration des contrainte pour le model cvrp non-orienté 3 indices.
    */
    void add_constraint_directed_cvrp_x_ek(IloEnv &, IloModel &, NumVarMatrix &, NumVarMatrix &);

    /*
    * Creation de la fonction objective du model CVRP non-orienté 3 indices.
    */
    void add_fonction_objectif_x_ek(IloEnv &, IloModel &, NumVarMatrix &);

    /*
    * Resolution du type enumeration.
    */
    Type_oriente resolution_enum_graphe(int);

    /*
     * Methode principale de la classe PLNE_vrp pour resoudre le probleme de tournees de vehicules.
     */
    void run(int);

    /*
     * Inclusion de la best solution de la Métaheuritique dans le PLNE via la fonction "addMIPStart(variables, valeurs)"
     */
    void addMIPStart_CPLEX(IloEnv &, IloCplex &, NumVarMatrix x_ij);

    /*
    * Création des variables pour le model cvrp orienté.
    */
    NumVarMatrix  add_variable_directed_cvrp(IloEnv &);
    NumVar3Matrix add_variable_directed_cvrp_x_ijk(IloEnv &);
    NumVarMatrix  add_variable_directed_cvrp_y_ik(IloEnv &);
    /*
    * Création des variables pour le model cvrp orienté avec mtz.
    */
    IloNumVarArray add_variable_directed_mtz(IloEnv &);

    /*
    * Création des contraintes pour le model cvrp orienté.
    */
    void add_constraint_directed_cvrp(IloEnv &, IloModel &, NumVarMatrix &);
    void add_constraint_directed_cvrp_x_ijk(IloEnv &, IloModel &, NumVarMatrix &, NumVar3Matrix &);

    /*
    * Création des contraintes MTZ pour le model cvrp orienté.
    */
    void add_constraint_directed_mtz(IloModel &, NumVarMatrix &, IloNumVarArray &);

    /*
    * Creation de la fonction objective du model CVRP orienté.
    */
    void add_fonction_objectif(IloEnv &, IloModel &, NumVarMatrix &);
    void add_fonction_objectif_x_ijk(IloEnv &, IloModel &, NumVar3Matrix &);

};

#endif /* PLNE_vrp_h */
