#include "Graph_lemon.h"

#define PREC 1

using namespace std;


Graph_lemon::Graph_lemon (Data data) : _data(data) {
}

void Graph_lemon::construct_Undirected_Lemon_Graph() {
  cout << "He suis la1" << endl;

  /*Initialisation des noeuds du graphe lemon*/
  for (int i(1); i <= _data.getDimension(); i++) { /*Depot inclus*/
      /*Initialisation d'une Map de noeud + initialisation des noeuds du graphe*/
      this->_nodeName[i] = this->_graph.addNode();
  }
  cout << "He suis la2" << endl;

  //Initialisation des arcs du graphe lemon (orienté)
  for (int i(1); i <= _data.getDimension(); i++) { //Depot inclus
    for (int j(i+1); j <= _data.getDimension(); j++) { //Depot inclus
        //Initialisation d'une Map de arc + initialisation des arcs du graphe
        cout << i << " " << j << endl;
        this->_edgeName[make_pair(i,j)] = this->_graph.addEdge(this->_nodeName[i], this->_nodeName[j]);
    }
  }
  cout << "He suis la3" << endl;

}

map<int, bool>  Graph_lemon::Undirected_MinimumCut(vector<double> &xe, vector<vector<int>> &indexEdge) {
  /*Donnees necessaire pour l'algorithme de NagamochiIbaraki de Lemon :*/
  lemon::ListGraph::EdgeMap<double>  cost_edges(this->_graph);
  lemon::ListGraph::NodeMap<bool>    mincut(this->_graph);
  double mincutvalue = 0.;
  int e;

  /*Map bool pour savoir si x-ij est dans la coupe*/
  map<int, bool> mincutBool;

  //Mise à jour des coups des arcs :
  for (int i(1); i <= _data.getDimension(); i++) { //Depot inclus
    //Graphe unidirectionnnel et non reflexive pour Nagamochi_Ibaraki
    for (int j(i+1) ; j <= _data.getDimension(); j++) { //Depot inclus
        //cout<< "CUT(" << i << "," << j << ") = " << _x_ij[i][j] << " " << this->_edgeName[make_pair(i,j)] << endl;
        e = indexEdge[i][j];
        cost_edges.set(this->_edgeName[make_pair(i,j)], xe[e] * PREC);
    }
  }

  /*Initialisation de l'Algorithme NagamochiIbaraki par le graphe et les couts correspondant aux arrêtes :*/
  lemon::NagamochiIbaraki<lemon::ListGraph, lemon::ListGraph::EdgeMap<double> > nagamochi_ibaraki(this->_graph, cost_edges);

  /*Execution de l'algorithme :*/
  nagamochi_ibaraki.run();

  /*nagamochi_ibaraki.init();
  for (int i(1); i <= _data.getDimension(); i++) {
    nagamochi_ibaraki.processNextPhase();
  }*/

  /*Retourne la coupe minimale dans mincut et dans mincutvalue la valeur de la coupe :*/
  mincutvalue = nagamochi_ibaraki.minCutMap(mincut) / (PREC*1.0);

  bool test = false;
  /*Initialisation de la list_W avec les sommets de la coupe minimum du graphe :*/
  for (int i(1); i <= _data.getDimension(); i++) { /*Depot inclus*/
    if (mincut[this->_nodeName[i]]) {
      mincutBool[i] = true;
      test = true;
    } else  {
      mincutBool[i] = false;
    }
  }
  mincutBool[0] = test;

  /*Affichage de la valeur de la coupe et de l'ensemble des sommets composants la coupe :*/
  cout<<"MinCut value : "<<mincutvalue<<endl;
  cout<<"MinCut induced by : ";
  for (int i(1); i <= _data.getDimension(); i++) {
    if (mincut[this->_nodeName[i]])
      cout<<i<<" ";
  }
  cout<<endl;

  /*Retourne la coupe minimum version Lemon :*/
  return mincutBool ;
}

lemon::ListGraph::Node Graph_lemon::getNode (const int &i) {
  return this->_nodeName[i];
}
