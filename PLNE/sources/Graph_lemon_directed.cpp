#include "Graph_lemon_directed.h"

#define PREC 1

using namespace std;


Graph_lemon_directed::Graph_lemon_directed (Data data) : _data(data) {
  /*Initialisation du graphe _x_ij à zéro*/
  for (int i(0); i <= _data.getDimension(); i++) {
    this->_x_ij.push_back(vector<double> (_data.getDimension()+1, 0.));
  }
}

void Graph_lemon_directed::construct_Undirected_Lemon_Graph() {
  /*Initialisation des noeuds du graphe lemon*/
  for (int i(1); i <= _data.getDimension(); i++) { /*Depot inclus*/
      /*Initialisation d'une Map de noeud + initialisation des noeuds du graphe*/
      this->_nodeName[i] = this->_graph.addNode();
  }

  //Initialisation des arcs du graphe lemon (orienté)
  /*for (int i(1); i <= _data.getDimension(); i++) { //Depot inclus
    for (int j(1+i); j <= _data.getDimension(); j++) { //Depot inclus
      if (i != j) {
        //Initialisation d'une Map de arc + initialisation des arcs du graphe
        this->_edgeName[make_pair(i,j)] = this->_graph.addEdge(this->_nodeName[i], this->_nodeName[j]);
      }
    }
  }*/

  for (int i(1); i <= _data.getDimension(); i++) {
    for (int j(1); j <= _data.getDimension(); j++) {
      if (i != j) {
        this->_edgeName[make_pair(i,j)] = this->_graph.addEdge(this->_nodeName[i], this->_nodeName[j]);
      }
    }
  }

}

map<int, bool>  Graph_lemon_directed::Undirected_MinimumCut() {
  /*Donnees necessaire pour l'algorithme de NagamochiIbaraki de Lemon :*/
  lemon::ListGraph::EdgeMap<double>  cost_edges(this->_graph);
  lemon::ListGraph::NodeMap<bool>    mincut(this->_graph);
  double mincutvalue = 0.;

  /*Map bool pour savoir si x-ij est dans la coupe*/
  map<int, bool> mincutBool;

  //Mise à jour des coups des arcs :
  /*for (int i(1); i <= _data.getDimension(); i++) { //Depot inclus
    //Graphe unidirectionnnel et non reflexive pour Nagamochi_Ibaraki
    for (int j(1+i) ; j <= _data.getDimension(); j++) { //Depot inclus
      if (i != j) {
        //cout<< "CUT(" << i << "," << j << ") = " << _x_ij[i][j] << " " << this->_edgeName[make_pair(i,j)] << endl;
        cost_edges.set(this->_edgeName[make_pair(i,j)], (this->_x_ij[i][j] + this->_x_ij[j][i])*PREC);
      }
    }
  }*/

  //Mise à jour des coups des arcs :
  for (int i(1); i <= _data.getDimension(); i++) { //Depot inclus
    //Graphe unidirectionnnel et non reflexive pour Nagamochi_Ibaraki
    for (int j(1) ; j <= _data.getDimension(); j++) { /*Depot inclus*/
      if (i != j) {
        //cout<< "CUT(" << i << "," << j << ") = " << _x_ij[i][j] << " " << this->_edgeName[make_pair(i,j)] << endl;
        cost_edges.set(this->_edgeName[make_pair(i,j)], (this->_x_ij[i][j])*PREC);
      }
    }
  }

  /*Initialisation de l'Algorithme NagamochiIbaraki par le graphe et les couts correspondant aux arrêtes :*/
  lemon::NagamochiIbaraki<lemon::ListGraph, lemon::ListGraph::EdgeMap<double> > nagamochi_ibaraki(this->_graph, cost_edges);

  /*Execution de l'algorithme :*/
  //nagamochi_ibaraki.run();

  nagamochi_ibaraki.init();

  for (int i(1); i <= _data.getDimension(); i++) {
    nagamochi_ibaraki.processNextPhase();
  }

  /*Retourne la coupe minimale dans mincut et dans mincutvalue la valeur de la coupe :*/
  mincutvalue = nagamochi_ibaraki.minCutMap(mincut) / (PREC*1.0);

  /*Initialisation de la list_W avec les sommets de la coupe minimum du graphe :*/
  for (int i(1); i <= _data.getDimension(); i++) { /*Depot inclus*/
    if (mincut[this->_nodeName[i]]) {
      mincutBool[i] = true;
    } else  {
      mincutBool[i] = false;
    }
  }

  /*Affichage de la valeur de la coupe et de l'ensemble des sommets composants la coupe :*/
  /*cout<<"MinCut value : "<<mincutvalue<<endl;
  cout<<"MinCut induced by : ";
  for (int i(1); i <= _data.getDimension(); i++) {
    if (mincut[this->_nodeName[i]])
      cout<<i<<" ";
  }
  cout<<endl; */

  /*Retourne la coupe minimum version Lemon :*/
  return mincutBool ;
}

lemon::ListGraph::Node Graph_lemon_directed::getNode (const int &i) {
  return this->_nodeName[i];
}

void Graph_lemon_directed::setXij (const int &i, const int &j, double value) {
  this->_x_ij[i][j] = value;
}
