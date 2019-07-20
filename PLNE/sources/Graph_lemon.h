#ifndef GRAPH_LEMON
#define GRAPH_LEMON

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <map>

#include <lemon/list_graph.h>
#include <lemon/concepts/maps.h>
#include <lemon/nagamochi_ibaraki.h>
#include <ilcplex/ilocplex.h>


#include "Data.h"

using namespace std;

typedef IloArray<IloNumVarArray> NumVarMatrix;

/**************************  C_Graph  ******************************/
class Graph_lemon{
private :
  /*
  * Attribut
  */
  Data                                          _data;        /*donnee fichier.vrp*/
  lemon::ListGraph                              _graph;       /*Graph lemon*/
  map<pair<int, int>, lemon::ListGraph::Edge>   _edgeName;     /*Edge  lemon*/
  map<int, lemon::ListGraph::Node>              _nodeName;     /*Node  lemon*/

public:

  /*
  * Constructeur Usuel prenant pour attribut les données du fichier.vrp
  */
  Graph_lemon (Data);

  /*
  * Construction d'un graphe lemon non orienté à partir des données du fichier vrp
  */
  void   construct_Undirected_Lemon_Graph();

  /*
  * Construction d'un coupe minimum du graphe lemon :
  * retourne : la valeur de cette coupe
  *          : l'ensemble des sommets de la coupe
  */
  map<int, bool>  Undirected_MinimumCut(vector<double> &, vector<vector<int>> &);

  /*
  * Accesseur
  */
  //lemon::ListGraph       getGraph();
  lemon::ListGraph::Node getNode (const int &);
  //lemon::ListGraph::Edge getEdge (const int &, const &);
  //double                 getX_ij (const double &, const double &);

};

#endif
