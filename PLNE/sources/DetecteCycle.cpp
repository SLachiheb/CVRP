#include "DetecteCycle.h"

DetecteCycle::DetecteCycle (const int &dim, map<int, vector<int> > matrice, list<int> client) {
    this->dimension = dim;
    this->mAdj = matrice;
    this->client_a_traite = client;
    for (int i(1); i <= dimension; i++) {
      this->MapClientBool[i] = -1;
    }
}

void DetecteCycle::run() {
    this->_find_cycleAvecDepot();
    this->_find_cycleSansDepot();
}

void DetecteCycle::_find_cycleAvecDepot() {
  int idDepot;
  int next_current = idDepot = 1; /*1 est le depot*/
  int current;
  map<int, bool> isVisited;

  for (int i(1); i <= dimension; i++) {
    if (i == 1)
      isVisited[i] = true;
    else
      isVisited[i] = false;
  }

  do {
      cycleD.push_back(next_current);
      current = mAdj[next_current][0];
      this->client_a_traite.remove(next_current);
      if (!isVisited[current]) {
        isVisited[current] = true;
      } else {
        if (mAdj[next_current].size() <= 1)
          current = idDepot;
        else {
          current = mAdj[next_current][1];
          if (!isVisited[current]) {
            isVisited[current] = true;
          } else {
            current = idDepot;
          }
        }
      }
      this->MapClientBool[current] = -1;
      next_current = current;
  } while (next_current!= idDepot);

  cycleD.push_back(idDepot);
}

void DetecteCycle::_find_cycleSansDepot() {

  int current, idStart;
  map<int, bool> isVisited;
  vector<int> vectorSansDepot;



  for (std::list<int>::iterator it = this->client_a_traite.begin(); it != this->client_a_traite.end(); it++) {
    int next_current = idStart = *it; /* isStart */

    for (int i(1); i <= dimension; i++) {
      if (i == idStart)
        isVisited[i] =  true;
      else
        isVisited[i] = false;
    }

    do {
        vectorSansDepot.push_back(next_current);
        current = mAdj[next_current][0];
        this->client_a_traite.remove(next_current);
        if (!isVisited[current]) {
          isVisited[current] = true;
        } else {
          current = mAdj[next_current][1];
          if (!isVisited[current]) {
            isVisited[current] = true;
          } else {
            current = idStart;
          }
        }
        this->MapClientBool[current] = this->cycleS.size();
        next_current = current;
    } while (next_current!= idStart);

    vectorSansDepot.push_back(idStart);

    it = this->client_a_traite.begin();
    this->cycleS.push_back(vectorSansDepot);
    vectorSansDepot.clear();
  }
}

void DetecteCycle::print_cycleAvecDepot() {
    cout << "Affichage des cycles avec Depot :" << endl;
    cout << "Cycle 1 : ";
    for (int i(0); i < this->cycleD.size(); i++) {
        cout << cycleD[i] << " ";
    }
    cout << endl;
}

void DetecteCycle::print_cycleSansDepot() {
    cout << "Affichage des cycles sans Depot :" << endl;
    for (int i(0); i < this->cycleS.size(); i++) {
        cout << "Cycle " << i+1 << " : ";
        for (int j(0); j < this->cycleS[i].size(); j++) {
            cout << cycleS[i][j] << " ";
        }
        cout << endl;
    }
}

void DetecteCycle::print_clientATraite() {
    cout << "Affichage de la liste de client a traiter : " << endl;
    for (std::list<int>::iterator it = this->client_a_traite.begin(); it != this->client_a_traite.end(); it++) {
        std::cout << *it << ' ';
    }
    cout << endl;
}

void DetecteCycle::print_MapClientBool() {
    cout << "Affichage de la Map des clients" << endl;
    for (int i(1); i <= this->dimension; i++) {
        cout << i << " = " << this->MapClientBool[i] << endl;
    }
}

vector<int>  DetecteCycle::getCycleD() {
  return this->cycleD;
}

vector<vector<int> > DetecteCycle::getCycleS() {
  return this->cycleS;
}

map<int, int> DetecteCycle::getMapClientBool() {
  return this->MapClientBool;
}
