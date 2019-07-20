#include "PLNE_vrp.h"

#include <lemon/list_graph.h>
#include <lemon/concepts/maps.h>
#include <lemon/nagamochi_ibaraki.h>
#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

#include <cstring>
#include <string>
#include <cmath>

#define OUTPUT
#define epsilon 0.001

ILOUSERCUTCALLBACK3(UserMengerCutSeparation, Data &, _data, Graph_lemon_directed &, _graph_undirected, NumVarMatrix &, x_ij) {
  #ifdef OUTPUT
      cout<<"*********** USER Min Cut separation Callback Fractionnaire 3 Parametres*************"<<endl;
  #endif

  //Mise à jour de la valeur des arcs du graph_lemon :
  for (int i(1); i <= _data.getDimension(); i++) { //depot_inclus
    for (int j(1); j <= _data.getDimension(); j++) { //_depot_inclus
        if (i != j) {
          _graph_undirected.setXij(i, j, getValue(x_ij[i][j]));
        }
    }
  }

  //Appel de la fonction de l'algorithme de coupe minimum :
  map<int, bool>  mincutBool = _graph_undirected.Undirected_MinimumCut();

  //Nombre d'élèments composants la sous tournée W :
  int nb_clients_coupe = 0;

  //Declaration d'une contrainte :
  IloExpr constrainte(_env);

  //Constructeur de contrainte de sous tournée potentiellement violée:
  //Parcours l'ensemble des sommets du graphe :
  for (int i(1); i <= _data.getDimension(); i++) { /*_depot_inclus*/
      //Si un sommet fait parti de la coupe minimum W, on met à jour la constrainte de sous tournée :
      if (mincutBool[i]) {
        nb_clients_coupe += 1;
        //Recherche des sommets ne faisant pas parti de W :
        for (int j(1); j <= _data.getDimension(); j++) {
          if ((i != j) and (not mincutBool[j]))
            constrainte += x_ij[i][j];
        }
     }
  }

  //Le nombre client dans l'ensemble W doit etre >= 2 :
  if (nb_clients_coupe >= 2) {
    for (int i(1); i <= _data.getDimension(); i++) {
        if (mincutBool[i]) {
          for (int j(1); j <= _data.getDimension(); j++) {
            if (i != j and not mincutBool[j])
              cout << "x_ij[" << i << "][" << j <<"]" << endl;
          }
       }
    }
    add(constrainte >= 1, IloCplex::UseCutForce); // UseCutForce UseCutPurge UseCutFilter
  }
}

ILOLAZYCONSTRAINTCALLBACK3(LazyMengerCutSeparation, Data &, _data, Graph_lemon_directed &, _graph_undirected, NumVarMatrix &, x_ij) {

  #ifdef OUTPUT
      cout<<"*********** Lazy Min Cut separation Callback Integer 3 Parametres*************"<<endl;
  #endif

  //Mise à jour de la valeur des arcs du graph_lemon :
  for (int i(1); i <= _data.getDimension(); i++) {
    for (int j(1); j <= _data.getDimension(); j++) {
        if (i != j) {
          #ifdef OUTPUT
              //cout<< "ILO(" << i<< "," << j << ") = " << getValue(x_ij[i][j])<<endl;
          #endif
          _graph_undirected.setXij(i, j, getValue(x_ij[i][j]));
        }
    }
  }

  //Appel de la fonction de l'algorithme de coupe minimum :
  map<int, bool>  mincutBool = _graph_undirected.Undirected_MinimumCut();

  //Nombre d'élèments composants la sous tournée W :
  int nb_clients_coupe = 0;

  IloExpr constrainte(_env);

  //Constructeur d'une contrainte de sous tournée potentiellement violée:
  //Parcours l'ensemble des sommets du graphe :
  for (int i(1); i <= _data.getDimension(); i++) {
      //Si un sommet fait parti de la coupe minimum W, on met à jour la constrainte de sous tournée :
      if (mincutBool[i]) {
        nb_clients_coupe = nb_clients_coupe + 1;
        //Recherche des sommets ne faisant pas parti de W :
        for (int j(1); j <= _data.getDimension(); j++) {
          if (i != j and not mincutBool[j])
            constrainte = constrainte + x_ij[i][j];
        }
     }
  }

  //Le nombre de clients dans l'ensemble W doit etre >= 2 :
  if (nb_clients_coupe >= 2) {
    add(constrainte >= 1, IloCplex::UseCutForce);
  }
}

ILOLAZYCONSTRAINTCALLBACK3(LazyCycleCutSeparation, Data &, _data, Graph_lemon_directed &, _graph_undirected, NumVarMatrix &, x_ij) {

  #ifdef OUTPUT
      cout<<"*********** Lazy Cycle separation Callback Integer 3 Parametres*************"<<endl;
  #endif
    //Construire une matrice adjacente du graphe CVRP à partir de x_ij entier
    //Déclaration de la matrice adjacente :
    map<int, vector<int>> mapMatriceAdj;
    //Initialisation de la matrice adjacente :
    for (int i(1); i <= _data.getDimension(); i++) {
      vector<int> cycle_client;
      for (int j(1); j <= _data.getDimension(); j++) {
        if (i != j) {
            if (getValue(x_ij[i][j]) > 1-epsilon){
              cycle_client.push_back(j);
            }
        }
      }
      mapMatriceAdj[i] = cycle_client;
    }

    //A supprimer dans le code final : Affichage.
    for (int i(1); i < mapMatriceAdj.size(); i++) {
      cout << i << " : ";
      for (int j(0); j < mapMatriceAdj[i].size(); j++) {
        #ifdef OUTPUT
            cout << mapMatriceAdj[i][j] << " ";
        #endif
      }
      cout << endl;
    }

    //Appel de la fonction de detection de cycle :
    DetecteCycle_directed findCycle(_data.getDimension(), mapMatriceAdj);

    findCycle.run();

    //A supprimer dans le code final
    findCycle.print_cycleSansDepot();
    findCycle.print_cycleAvecDepot();

    //Recupere la liste des cycles :
    vector<vector<int> >  cycle_sans_depot = findCycle.getCycleS();
    map<int, int>         map_list_SD     = findCycle.getMapClientBool();

    //Affichage de la map_list_SD :
    //findCycle.print_MapClientBool();

    for (int i(1); i <= _data.getDimension(); i++) {
      cout << i << " : " << map_list_SD[i] << endl;
    }

    //Supprime les Cycles sans depot
    //Parcours chaque cycle
    for (int i(0); i < cycle_sans_depot.size(); i++) {
      IloExpr  contrainte(_env);
      int     somme_demande = 0;
      //Parcours le cycle
      for (int j(0); j < cycle_sans_depot[i].size() - 1; j++) {
        somme_demande += _data.getDemande_client(cycle_sans_depot[i][j]);
        cout << i << ", " << j << " : ";
        //Parcours chaque noeud du graphe
        for (int k(1); k <= _data.getDimension(); k++) {
          //Si le noeud fait pas partie du sommet du cycle i en question :
          if (map_list_SD[k] != i ) {
            cout << "(" << cycle_sans_depot[i][j] << ", " << k<< "), ";
            cout << "(" << k << ", " << cycle_sans_depot[i][j] << "), ";
            contrainte += x_ij[cycle_sans_depot[i][j]][k];
            contrainte += x_ij[k][cycle_sans_depot[i][j]];
          }
        }
        cout << endl;
      }
      add(contrainte >= ceil(somme_demande*1. / _data.getCapacite()), IloCplex::UseCutForce);
    }


    //Supprime les Cycles avec depot respectant par la capacite
    //Recupere la liste des cycles :
    vector<vector<int> >  cycle_avec_depot = findCycle.getCycleD();

    //Parcours chaque cycle comportant le depot :
    for (int i(0); i < cycle_avec_depot.size(); i++) {
      IloExpr  contrainte(_env);
      //IloExpr  contrainte2(_env);
      int     somme_demande = 0;
      for (int j(0); j < cycle_avec_depot[i].size() - 1; j++) {
        somme_demande += _data.getDemande_client(cycle_avec_depot[i][j]);
        contrainte += x_ij[cycle_avec_depot[i][j]][cycle_avec_depot[i][j+1]];
        //contrainte2 += x_ij[cycle_avec_depot[i][j+1]][cycle_avec_depot[i][j]];
      }
      if (somme_demande > _data.getCapacite()) {
        cout << somme_demande <<endl;
        add(contrainte >= ceil(somme_demande*1. / _data.getCapacite()), IloCplex::UseCutForce);
        //add(contrainte2 >= ceil(somme_demande*1. / _data.getCapacite()), IloCplex::UseCutForce);
      }
    }
}

PLNE_vrp::PLNE_vrp(Data data) : _data(data), _graph_undirected(data)
{
  _id_depot = _data.getIdDepot();
  _depot_inclus = _id_depot;
  _depot_exclus = _id_depot + 1;
  for (int i(0); i <= _data.getDimension(); i++) {
    _delta_i.push_back(vector<int> ());
  }

}

Type_oriente PLNE_vrp::resolution_enum_graphe(int input) {
  if (input == 1) return CVRP_UNDIRECTED;
  if (input == 2) return CVRP_ORIENTE_WITH_MTZ;
  if (input == 3) return CVRP_ORIENTE_WITHOUT_MTZ;
  if (input == 4) return CVRP_DIRECTED_WITHOUT_MTZ_3_INDEX;
  return DEFAULTS;
}

/*void PLNE_vrp::addMIPStart_CPLEX(IloEnv &env, IloCplex &cplex_, NumVarMatrix x_ij) {
    //Résolution par métaheuristique
    //this->_meta_init.run();
    //vector<vector<int>> solution = this->_meta_init.getBestSolution();

    //Point de départ pour résoudre le MIP
    IloNumVarArray x_ij_start (env); //Ensemble de variables
    IloNumArray    v_ij_start (env); //Ensemble de valeurs associés aux variables

    cout << "addMIPStart" << endl;
    //Initialisation de la solution de depart
    for (int i(0); i < solution.size(); i++) {

        for (int j(0); j < solution[i].size()-1; j++) {
            if (j == 0) {
                //Initialisation du depot pour chaque tournée
                x_ij_start.add(x_ij[1][solution[i][j]]);
                v_ij_start.add(1);
            }

            x_ij_start.add(x_ij[solution[i][j]][solution[i][j+1]]);
            v_ij_start.add(1);

            if (j == solution[i].size()-2) {
                //Initialisation du depot pour chaque fin de tournée
                x_ij_start.add(x_ij[solution[i][j+1]][1]);
                v_ij_start.add(1);
            }
        }
    }

    cplex_.addMIPStart(x_ij_start, v_ij_start);
    x_ij_start.end();
    v_ij_start.end();
}*/

NumVarMatrix PLNE_vrp::add_variable_directed_cvrp(IloEnv &_env){
  /*Declaration de la variable de decision x_ij :*/
  /** x_ij : Les client i visités immédiatement avant les client j => voir pour supprimer des contraintes non necessaire !!!**/
  NumVarMatrix x_ij(_env, _data.getDimension()+1);
  for (int i(0); i <= _data.getDimension(); i++) {
      x_ij[i] =  IloNumVarArray(_env, _data.getDimension()+1);
      for (int j(0); j <= _data.getDimension(); j++) {
          stringstream            myname;
          myname << "x_(" << i << "," << j << ")";
          x_ij[i][j] = IloNumVar(_env, 0, 1, ILOINT, myname.str().c_str());
      }
  }
  cout << x_ij << endl << endl;

  return x_ij;
}

NumVar3Matrix PLNE_vrp::add_variable_directed_cvrp_x_ijk(IloEnv &_env){
  /*Declaration de la variable de decision x_ijk :*/
  NumVar3Matrix x_ijk(_env, _data.getDimension()+1);
  for (int i(0); i <= _data.getDimension(); i++) {
      x_ijk[i] = NumVarMatrix(_env, _data.getDimension()+1);
      for (int j(0); j <= _data.getDimension(); j++) {
          x_ijk[i][j] =   IloNumVarArray(_env, _data.getNb_vehicules()+1);
          for (int k(0); k <= _data.getNb_vehicules(); k++) {
              stringstream            myname;
              myname << "x_(" << i << "," << j << "," << k << ")";
              x_ijk[i][j][k] = IloNumVar(_env, 0, 1, ILOINT, myname.str().c_str()); /*Attention il ne vaut pas prendre compte les reflexifs*/
          }
      }
  }
  return x_ijk;
}

NumVarMatrix PLNE_vrp::add_variable_directed_cvrp_y_ik(IloEnv &_env){
  /*Declaration de la variable de decision y_ik :*/
  NumVarMatrix y_ik(_env, _data.getDimension()+1);
  for (int i(0); i <= _data.getDimension(); i++) {
      y_ik[i] =  IloNumVarArray(_env, _data.getDimension()+1);
      for (int k(0); k <= _data.getNb_vehicules(); k++) {
          stringstream            myname;
          myname << "x_(" << i << "," << k << ")";
          y_ik[i][k] = IloNumVar(_env, 0, 1, ILOINT, myname.str().c_str());
      }
  }
  cout << y_ik << endl << endl;

  return y_ik;
}

IloNumVarArray PLNE_vrp::add_variable_directed_mtz(IloEnv &_env){
  /*Declaration de la variable de decision wi : variable representant la charge du vehicule apres la visite du client i*/
  IloNumVarArray w_(_env);
  for (int i(0); i <= _data.getDimension(); i++) { //w_(0) ne sert a rien, juste pratique pour gerer les tableaux
      stringstream            myname;
      myname << "w_(" << i << ")";
      //w_.add(IloNumVar(_env, 0.0, _data.getCapacite() - _data.getDemande_client(i), ILOFLOAT, myname.str().c_str()));
      w_.add(IloNumVar(_env, _data.getDemande_client(i), _data.getCapacite() , ILOFLOAT, myname.str().c_str()));
  }

  return w_;
}

void PLNE_vrp::add_constraint_directed_cvrp(IloEnv &env, IloModel &model, NumVarMatrix &x_ij) {
  /*Declaration de la constrainte 1 et 2 : Le depot peut avoir jusqu'à m arc sortant et m arc entrant */
  IloExpr constraintDepOut(env);
  IloExpr constraintDepIn(env);
  for (int j(_depot_exclus); j <= _data.getDimension(); j++) {
    constraintDepOut += x_ij[_id_depot][j];
    constraintDepIn  += x_ij[j][_id_depot];
  }
  model.add(constraintDepOut == _data.getNb_vehicules());
  model.add(constraintDepIn == _data.getNb_vehicules());


  /*Declaration de la constrainte 3 : La somme du nombre d'arcs sortant du sommet client est de 1*/
  for (int i(_depot_exclus); i <= _data.getDimension(); i++) {
      IloExpr    expr_contraint3(env);
      for (int j(_depot_inclus); j <= _data.getDimension(); j++) {
          if (i != j)
              expr_contraint3 += x_ij[i][j];
      }
      model.add( expr_contraint3 == 1 );
  }

  /*Declaration de la constrainte 4 :  La somme du nombre d'arcs entrant du sommet client est de 1*/
  for (int j(_depot_exclus); j <= _data.getDimension(); j++) {
      IloExpr     expr_contraint4(env);
      for (int i(_depot_inclus); i <= _data.getDimension(); i++) {
          if (i != j)
              expr_contraint4 += x_ij[i][j];
      }
      model.add( expr_contraint4 == 1 );
  }
}

void PLNE_vrp::add_constraint_directed_cvrp_x_ijk(IloEnv &env, IloModel &model, NumVarMatrix &y_ik, NumVar3Matrix &x_ijk) {
  /*Contrainte (1.29)*/
  for (int i(_depot_exclus); i <= _data.getDimension(); i++) {
    IloExpr contraint_1_29(env);
    for (int k(1); k <= _data.getNb_vehicules(); k++) {
      contraint_1_29 += y_ik[i][k];
    }
    model.add( contraint_1_29 == 1 );
  }

  /*Contrainte (1.30)*/
  IloExpr contraint_1_30(env);
  for (int k(1); k <= _data.getNb_vehicules(); k++) {
    contraint_1_30 += y_ik[_id_depot][k];
  }
  model.add( contraint_1_30 == _data.getNb_vehicules() );

  /*Contrainte (1.31)*/
  for (int i(_depot_inclus); i <= _data.getDimension(); i++) {
    for (int k(1); k <= _data.getNb_vehicules(); k++) {
      IloExpr contraint_1_31_1(env);
      IloExpr contraint_1_31_2(env);
      for (int j(_depot_inclus); j <= _data.getDimension(); j++) {
        contraint_1_31_1 += x_ijk[i][j][k];
      }
      for (int j(_depot_inclus); j <= _data.getDimension(); j++) {
        contraint_1_31_2 += x_ijk[j][i][k];
      }
      model.add ( contraint_1_31_1 == contraint_1_31_2 == y_ik[i][k] );
    }
  }

  /*Contrainte (1.32)*/
  for (int k(1); k <= _data.getNb_vehicules(); k++) {
    IloExpr  contraint_1_32(env);
    for (int i(_depot_inclus); i < _data.getDimension(); i++) {
      contraint_1_32 += _data.getDemande_client(i) * y_ik[i][k];
    }
    model.add( contraint_1_32 <= _data.getCapacite() );
  }
}

void PLNE_vrp::add_constraint_directed_mtz(IloModel &model, NumVarMatrix &x_ij, IloNumVarArray &w_) {
  /*Declaration de la contrainte 5 : La contrainte permettant d’éliminer les cycles du graphe */
  for (int i(_depot_exclus); i <= _data.getDimension(); i++) {
      for (int j(_depot_exclus); j <= _data.getDimension(); j++) {
          if (i != j)
              model.add(w_[i] - w_[j] >= _data.getDemande_client(i) - (_data.getCapacite() + _data.getDemande_client(i)) * (1 - x_ij[i][j]));
              //model.add(w_[i] - w_[j] >= _data.getDemande_client(j) - (_data.getCapacite() * (1 - x_ij[i][j])));
      }
  }
}

void PLNE_vrp::add_fonction_objectif(IloEnv &env, IloModel &model, NumVarMatrix &x_ij) {
  /*Declaration d'une fonction objective à 2 indices:*/
  IloExpr            expr(env);
  for (int i(_depot_inclus); i <= _data.getDimension(); i++) {
      for (int j(_depot_inclus); j <= _data.getDimension(); j++) {
          if (i != j) {
            //obj.setLinearCoef(x_ij[i][j], round(_data.getDistance_ij(i,j)));
            expr += round(_data.getDistance_ij(i,j)) * x_ij[i][j];
          }

      }
  }
  model.add(IloMinimize(env, expr));
  /*Affichage du model*/
  cout << model << endl;
}

void PLNE_vrp::add_fonction_objectif_x_ijk(IloEnv &env, IloModel &model, NumVar3Matrix &x_ijk) {
  /*Declaration d'une fonction objective à 3 indices:*/
  IloExpr            expr(env);
  for (int i(_depot_inclus); i <= _data.getDimension(); i++) {
      for (int j(_depot_inclus); j <= _data.getDimension(); j++) {
          if (i != j) {
            IloExpr            expr_xijk(env);
            for (int k(1); k <= _data.getNb_vehicules(); k++) {
                expr_xijk += x_ijk[i][j][k];
            }
            expr += round(_data.getDistance_ij(i, j)) * expr_xijk;
          }
      }
  }
  model.add(IloMinimize(env, expr));
  /*Affichage du model*/
  cout << model << endl;
}

/*********************************************************************************************************/
/***************************************Partie 3 Indices Undirected***************************************/
/*********************************************************************************************************/

void PLNE_vrp::indexIJtoEdge() {
    /*Declaration :*/
    int                   init_num = 0;

    for (int i(0); i <= _data.getDimension(); i++) {
        this->_indexEdge.push_back(vector<int> (_data.getDimension()+1, -1));
    }
    /*Initialisation :*/
    for (int i(0); i <= _data.getDimension(); i++) {
        if (i == 0)
            continue;
        for (int j(i+1); j <= _data.getDimension(); j++) {
            if (i < j) {
                this->_indexEdge[i][j] = init_num;
                this->_indexEdge[j][i] = init_num;
                this->_delta_i[i].push_back(init_num) ;
                this->_delta_i[j].push_back(init_num) ;
                this->_ce[init_num] = _data.getDistance_ij(i, j);
                this->_indexNode[init_num] = make_pair(i, j);
                init_num++;
            }
        }
    }
}

void PLNE_vrp::print_indexIJtoEdge () {
  /*Affichage :*/
  cout << "coord (i, j) => numero d'arc : " << endl;
  for (int i(0); i < this->_indexEdge.size(); i++) {
      cout << i << " : ";
      for (int j(0); j < this->_indexEdge[i].size(); j++) {
          cout << this->_indexEdge[i][j] << " ";
      }
      cout << endl;
  }
  cout << endl;
  cout << "Pour chaque sommet => sommet voisin : " << endl;
  for (int i(0); i < this->_delta_i.size(); i++) {
      cout << i << " : ";
      for (int j(0); j < this->_delta_i[i].size(); j++) {
          cout << this->_delta_i[i][j] << " ";
      }
      cout << endl;
  }
  cout << endl;
  cout << "Pour chaque arc la distance entre les points : " << endl;
  for (int i(0); i < this->_ce.size(); i++) {
      cout << i << " : " << this->_ce[i] << endl;
  }
}

NumVarMatrix  PLNE_vrp::add_variable_Undirected_cvrp_x_ek(IloEnv &_env) {
  /*Numero d'arc:*/
  int e;

  /*Nombre d'arcs dans le graphe :*/
  this->_taille_xe = (((_data.getDimension() *  (_data.getDimension() - 1)) / 2))- 1;
  cout << _taille_xe << endl;

  /*Declaration de la variable de decision x_ek :*/
  NumVarMatrix x_ek(_env, this->_taille_xe  + 1);
  for (int i(_depot_inclus); i <= _data.getDimension(); i++) {
      for (int j(i + 1); j <= _data.getDimension(); j++) {
          e = _indexEdge[i][j];
          x_ek[e] =  IloNumVarArray(_env, _data.getNb_vehicules()+1);
          for (int k(0); k <= _data.getNb_vehicules(); k++) {
            stringstream            myname;
            myname << "x_(" << e << "," << k << ")";
            if (i ==_id_depot)
              x_ek[e][k] = IloNumVar(_env, 0, 2, ILOINT, myname.str().c_str()); /*contraint (1.47)*/
            else
              x_ek[e][k] = IloNumVar(_env, 0, 1, ILOINT, myname.str().c_str()); /*contraint (1.46)*/
          }
      }
  }
  cout << x_ek << endl << endl;
  return x_ek;
}

NumVarMatrix  PLNE_vrp::add_variable_Undirected_cvrp_y_ik(IloEnv &_env) {
  /*Declaration de la variable de decision y_ik :*/

  NumVarMatrix y_ik(_env, _data.getDimension()+1);

  for (int i(0); i <= _data.getDimension(); i++) {
    y_ik[i] =  IloNumVarArray(_env, _data.getNb_vehicules()+1);
    for (int k(0); k <= _data.getNb_vehicules(); k++) {
      stringstream            myname;
      myname << "y_(" << i << "," << k << ")";
      y_ik[i][k] = IloNumVar(_env, 0, 1, ILOINT, myname.str().c_str()); /*contraint (1.45)*/
    }
  }
  cout << y_ik << endl << endl;
  return y_ik;
}

void PLNE_vrp::add_constraint_directed_cvrp_x_ek(IloEnv &env, IloModel &model, NumVarMatrix &x_ek, NumVarMatrix &y_ik) {
  /*Contraint (1.40)*/
  for (int i(_depot_exclus); i <= _data.getDimension(); i++) {
    IloExpr contraint_1_40(env);
    for (int k(1); k <= _data.getNb_vehicules(); k++) {
      contraint_1_40 += y_ik[i][k];
    }
    model.add ( contraint_1_40 == 1 );
  }

  /*Contraint (1.41)*/
  IloExpr contraint_1_41(env);
  for (int k(1); k <= _data.getNb_vehicules(); k++) {
    contraint_1_41 += y_ik[_id_depot][k];
  }
  model.add ( contraint_1_41 == _data.getNb_vehicules() );

  /*Contraint (1.42)*/
  for (int i(_depot_inclus); i <= _data.getDimension(); i++) {
    for (int k(1); k <= _data.getNb_vehicules(); k++) {
      IloExpr contraint_1_42(env);
      for (int t(0); t < this->_delta_i[i].size(); t++) {
        contraint_1_42 += x_ek[this->_delta_i[i][t]][k];
      }
      model.add ( contraint_1_42 == 2 * y_ik[i][k]  );
    }
  }

  /*Contraint (1.43)*/
  for (int k(1); k <= _data.getNb_vehicules(); k++) {
    IloExpr contraint_1_43(env);
    for (int i(_depot_inclus); i <= _data.getDimension(); i++) {
      contraint_1_43 += _data.getDemande_client(i) * y_ik[i][k];
    }
    model.add( contraint_1_43 <= _data.getCapacite() );
  }

  /*Contraint (1.44)*/
  // Contrainte ajouté par méthode de coupe.
}

void PLNE_vrp::add_fonction_objectif_x_ek(IloEnv &env, IloModel &model, NumVarMatrix &x_ek) {
  IloExpr contraint_obj(env);
  for (int e(0); e <= this->_taille_xe; e++) {
    IloExpr            expr_x_ek(env);
    for (int k(1); k <= _data.getNb_vehicules(); k++) {
      expr_x_ek += x_ek[e][k];
    }
    contraint_obj += round(this->_ce[e]) * expr_x_ek;
  }
  model.add(IloMinimize(env, contraint_obj));
  /*Affichage du model*/
  cout << model << endl;
}

ILOUSERCUTCALLBACK4(UserMinCutUndirectedSeparation, Data &, _data, NumVarMatrix &, x_ek, vector<vector<int>> &, indexEdge, int &, taille_xe) {
  #ifdef OUTPUT
      cout<<"*********** USER Min Cut Undirected separation Callback Fractionnaire 3 Parametres*************"<<endl;
  #endif

  int               e;
  vector<double>    xe(taille_xe+1, 0); /*A voir*/
  Graph_lemon       graph_undirected(_data);
  double            value;

  // Pour chaque vehicule trouver la mincut :
  for (int k(1); k <= _data.getNb_vehicules(); k++) {

    //Initialisation des valeurs des arcs xe pour le grapg_lemon :
    for (int i(1); i <= _data.getDimension(); i++) { //depot_inclus
      for (int j(i+1); j <= _data.getDimension(); j++) { //_depot_inclus
        /*Calcul l'index (i, j) correspondant à l'arc xe :*/
        e = indexEdge[i][j];
        /*Initialisation des arcs xe Lemon :*/
        //xe[e] = getValue(x_ek[e][k]);

        if (getValue(x_ek[e][k]) > 2 -epsilon)
          xe[e] = 2.;
        if (getValue(x_ek[e][k]) > 1 -epsilon)
          xe[e] = 1.;
        else
          xe[e] = getValue(x_ek[e][k]);
      }
    }

    //Contruction du graph undirected de lemon :
    graph_undirected.construct_Undirected_Lemon_Graph();

    /*Appel de la fonction de l'algorithme de coupe minimum :*/
    map<int, bool>  mincutBool = graph_undirected.Undirected_MinimumCut(xe, indexEdge);

    /*Nombre d'élèments composants la sous tournée W :*/
    int nb_clients_coupe = 0;

    /*Declaration d'une contrainte :*/
    IloExpr constraint(_env);

    if (!mincutBool[1]) {
      /*Constructeur de contrainte de sous tournée potentiellement violée:*/
      /*Parcours l'ensemble des sommets du graphe :*/
      for (int i(1); i <= _data.getDimension(); i++) { /*_depot_inclus*/
          /*Si un sommet fait parti de la coupe minimum W, on met à jour la constrainte de sous tournée :*/
          if (mincutBool[i]) {
            nb_clients_coupe += 1;
            /*Recherche des sommets ne faisant pas parti de W :*/
            for (int j(1); j <= _data.getDimension(); j++) { /*_depot_inclus*/
              if (! mincutBool[j]) {
                if (j < i) {
                  e = indexEdge[j][i];
                } else {
                  e = indexEdge[i][j];
                }
                constraint += x_ek[e][k];
              }
            }
          }
      }

      /*Le nombre client dans l'ensemble W doit etre >= 2 :*/
      if (nb_clients_coupe >= 2) {
        add(constraint >= 2, IloCplex::UseCutForce); // UseCutForce UseCutPurge UseCutFilter
      }
    }
  }
}

ILOLAZYCONSTRAINTCALLBACK4(LazyCycleUndirectedCutSeparation,  Data &, _data, NumVarMatrix &, x_ek, vector<vector<int>> &, indexEdge, int &, taille_xe) {

  #ifdef OUTPUT
      cout<<"*********** Lazy Cycle Undirected separation Callback Integer 3 Parametres*************"<<endl;
  #endif
    int e;

    for (int k(1); k <= _data.getNb_vehicules(); k++) {

          map<int, bool> isPresent;
          list<int>    is_present;
          for (int i(1); i <= _data.getDimension(); i++) {
            isPresent[i] = false;
          }

          //Construire une matrice adjacente du graphe CVRP à partir de x_ek entier
          map<int, vector<int>> mapMatriceAdj;
          //Initialisation de la matrice adjacente :
          for (int i(1); i <= _data.getDimension(); i++) {
            vector<int> cycle_client;
            for (int j(1); j <= _data.getDimension(); j++) {
              if (i == j) continue;
              if (i < j) {
                e = indexEdge[i][j];
                if (getValue(x_ek[e][k]) > 1 - epsilon || getValue(x_ek[e][k]) > 2 - epsilon){ // (getValue(x_ek[e][k]) == 1 || getValue(x_ek[e][k]) == 2){
                    cycle_client.push_back(j);
                    isPresent[i] = true;
                }
              }
              if (j < i) {
                e = indexEdge[j][i];
                if (getValue(x_ek[e][k]) > 1 - epsilon || getValue(x_ek[e][k]) > 2 - epsilon){
                    cycle_client.push_back(j);
                    isPresent[i] = true;
                }
              }

            }
            mapMatriceAdj[i] = cycle_client;
          }

          for (int i(1); i <= _data.getDimension(); i++) {
            if (isPresent[i])
              is_present.push_back(i);
          }

          //Appel de la fonction de detection de cycle :
          DetecteCycle findCycle(_data.getDimension(), mapMatriceAdj, is_present);
          findCycle.run();

          //A supprimer dans le code final
          findCycle.print_cycleSansDepot();
          //findCycle.print_cycleAvecDepot();

          //Recupere la liste des cycles :
          vector<vector<int> >  cycle_sans_depot = findCycle.getCycleS();
          map<int, int>         map_list_SD     = findCycle.getMapClientBool();

          //Affichage de la map_list_SD :
          //findCycle.print_MapClientBool();

          /*for (int i(1); i <= _data.getDimension(); i++) {
            cout << i << " : " << map_list_SD[i] << endl;
          }*/

          //Supprime les Cycles sans depot / Parcours chaque cycle


          for (int i(0); i < cycle_sans_depot.size(); i++) {
            IloExpr  contrainte(_env);
            int     somme_demande = 0;
            //Parcours le cycle
            for (int j(0); j < cycle_sans_depot[i].size() -1; j++) {
              int num_node = cycle_sans_depot[i][j];
              somme_demande += _data.getDemande_client(num_node);
              //cout << i << ", " << j << " : "
              //Parcours chaque noeud du graphe
              for (int t(1); t <= _data.getDimension(); t++) {
                //Si le noeud fait pas partie du sommet du cycle i en question :
                if (map_list_SD[t] != i) {
                  //cout << "(" << cycle_sans_depot[i][j] << ", " << t<< "), ";
                  if (num_node < t)
                    e = indexEdge[num_node][t];
                  else
                    e = indexEdge[t][num_node];
                  //cout << e << " ";
                  contrainte += x_ek[e][k];
                }
              }
            }
            add(contrainte - (2*ceil(somme_demande*1. / _data.getCapacite())) >= 0.001 , IloCplex::UseCutForce);
          }
    }
}


void PLNE_vrp::run(int choix)
{
    IloEnv env;                 //Integration de l'environnement Cplex au programme.

    try {
        IloModel model(env);    // Regroupe les objets suivants contraintes, variables, fonction objectif.
        IloCplex cplex(model);  // Regroupe les fonctions solveurs de Cplex.
        //IloObjective obj=IloAdd(model, IloMinimize(env));

        NumVarMatrix    x_ij, y_ik, x_ek;
        NumVar3Matrix   x_ijk;
        IloNumVarArray  w_;

        switch (resolution_enum_graphe(choix)) {
            case CVRP_UNDIRECTED:
                /*Pré-traitement*/
                this->indexIJtoEdge();
                this->print_indexIJtoEdge();
                /*Creation des variables*/
                x_ek = this->add_variable_Undirected_cvrp_x_ek(env);
                y_ik = this->add_variable_Undirected_cvrp_y_ik(env);
                /*Creation des contraintes*/
                this->add_constraint_directed_cvrp_x_ek(env, model, x_ek, y_ik);
                /*Creation de la fonction objectif*/
                this->add_fonction_objectif_x_ek(env, model, x_ek);
                /*Creation de coupes*/
                //cplex.use(UserMinCutUndirectedSeparation(env, this->_data, x_ek, this->_indexEdge, this->_taille_xe));
                cplex.use(LazyCycleUndirectedCutSeparation(env, this->_data, x_ek, this->_indexEdge, this->_taille_xe));
                break;
            case CVRP_ORIENTE_WITHOUT_MTZ:
                /*Creation des variables*/
                x_ij = this->add_variable_directed_cvrp(env);
                /*Creation de constrainte du cvrp*/
                this->add_constraint_directed_cvrp(env, model, x_ij);
                /*Creation de la fonction objective*/
                this->add_fonction_objectif(env, model, x_ij);
                /*Utilisation de coupe*/
                cplex.use(LazyCycleCutSeparation(env, _data, _graph_undirected, x_ij));
                break;
            case CVRP_ORIENTE_WITH_MTZ:
                /*Creation des variables*/
                x_ij = this->add_variable_directed_cvrp(env);
                w_ = this->add_variable_directed_mtz(env);
                /*Creation de constrainte du cvrp*/
                this->add_constraint_directed_cvrp(env, model, x_ij);
                /*Creation de constainte de la mtz pour la cvrp*/
                this->add_constraint_directed_mtz(model, x_ij, w_);
                /*Creation de la fonction objective*/
                this->add_fonction_objectif(env, model, x_ij);
                /*Utilisation de coupe*/
                cplex.use(UserMengerCutSeparation(env, _data, _graph_undirected, x_ij));
                cplex.use(LazyMengerCutSeparation(env, _data, _graph_undirected, x_ij));
                break;
            case CVRP_DIRECTED_WITHOUT_MTZ_3_INDEX:
                /*Creation des variables*/
                x_ijk = this->add_variable_directed_cvrp_x_ijk(env);
                y_ik  = this->add_variable_directed_cvrp_y_ik(env);
                /*Creation de constrainte du cvrp*/
                this->add_constraint_directed_cvrp_x_ijk(env, model, y_ik, x_ijk);
                /*Creation de la fonction objective*/
                this->add_fonction_objectif_x_ijk(env, model, x_ijk);
                break;
            case DEFAULTS:
                break;
        }

        /*Insertion de la solution d'une Métaheuristique*/
        //this->addMIPStart_CPLEX(env, cplex, x_ij);

        cplex.exportModel("sortie.lp");

        /*Resolution du PLNE*/
        cplex.solve();

        cout << "Solution status = " << cplex.getStatus() << endl;
        cout << "Solution value  = " << cplex.getObjValue() << endl;


        if (resolution_enum_graphe(choix) == CVRP_UNDIRECTED) {
          for (int k(1); k <= _data.getNb_vehicules(); k++) {
            cout << "Voiture " << k << " : " << endl;
            for (int e(0); e <= _taille_xe; e++) {
                int val = cplex.getValue(x_ek[e][k]);
                if (val == 1 || val == 2) {
                  cout << _indexNode[e].first << " " << _indexNode[e].second << endl;
                }
            }
          }
        } else {
          for (int i(_depot_inclus); i <= _data.getDimension(); i++) {
            for (int j(_depot_inclus); j <= _data.getDimension(); j++) {
              if (i != j) {
                int vals = cplex.getValue(x_ij[i][j]);
                if (vals == 1)
                  cout << i << " " << j << endl;
              }
            }
          }
        }
    }
    catch (IloException& ex) {
        cerr << "Error: Exception de la Concert Technology " << ex << endl;
    }
    catch (...) {
        cerr << "Error : Exception inconnu " << endl;
    }

    env.end(); //liberation de l'environnement Cplex en appelant la méthode end() sur l'objet IloEnv.
}
