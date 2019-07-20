#include "DetecteCycle_directed.h"

DetecteCycle_directed::DetecteCycle_directed(const int &dim, map<int, vector<int> > matrice) {
    this->dimension = dim;
    this->mAdj = matrice;
    for (int i(2); i <= dim; i++ ) {
        this->client_a_traite.push_back(i);
    }
}

void DetecteCycle_directed::run() {
    this->_find_cycleAvecDepot();
    this->_find_cycleSansDepot();
}

void DetecteCycle_directed::_find_cycleAvecDepot() {
    int idDepot = 1; /*1 est le depot*/

    this->MapClientBool[idDepot] = -1;

    vector<int> cycleDepot;
    int current, next_current;

    /*Recuperer la ligne Depot du tableau Map :*/
    vector<int> vectorDepot = this->mAdj[idDepot];

    /*Recupere tout les cycles avec le depot :*/
    for (int i(0); i < vectorDepot.size(); i++) {
        cycleDepot.push_back(idDepot);

        current = vectorDepot[i];
        cycleDepot.push_back(current);
        this->client_a_traite.remove(current);
        this->MapClientBool[current] = -1;
        do {
            next_current = this->mAdj[current][0];
            cycleDepot.push_back(next_current);
            current = next_current;
            this->client_a_traite.remove(current);
            this->MapClientBool[current] = -1;
        } while (current != idDepot);

        this->cycleD.push_back(cycleDepot);
        cycleDepot.clear();
    }
}

void DetecteCycle_directed::_find_cycleSansDepot() {
    int idStart;
    int current, next_current;

    /*Recuperer la ligne Depot du tableau Map :*/
    vector<int> vectorSansDepot;

    for (std::list<int>::iterator it = this->client_a_traite.begin(); it != this->client_a_traite.end(); it++) {
        idStart = current = *it;
        vectorSansDepot.push_back(idStart);
        do {
            next_current = this->mAdj[current][0];
            vectorSansDepot.push_back(next_current);
            current = next_current;
            this->client_a_traite.remove(current);
            this->MapClientBool[current] = this->cycleS.size();
            it = this->client_a_traite.begin();
        } while (current != idStart);

        this->cycleS.push_back(vectorSansDepot);
        vectorSansDepot.clear();
    }
}

void DetecteCycle_directed::print_cycleAvecDepot() {
    cout << "Affichage des cycles avec Depot :" << endl;
    for (int i(0); i < this->cycleD.size(); i++) {
        cout << "Cycle " << i+1 << " : ";
        for (int j(0); j < this->cycleD[i].size(); j++) {
            cout << cycleD[i][j] << " ";
        }
        cout << endl;
    }
}

void DetecteCycle_directed::print_cycleSansDepot() {
    cout << "Affichage des cycles sans Depot :" << endl;
    for (int i(0); i < this->cycleS.size(); i++) {
        cout << "Cycle " << i+1 << " : ";
        for (int j(0); j < this->cycleS[i].size(); j++) {
            cout << cycleS[i][j] << " ";
        }
        cout << endl;
    }
}

void DetecteCycle_directed::print_clientATraite() {
    cout << "Affichage de la liste de client a traiter : " << endl;
    for (std::list<int>::iterator it = this->client_a_traite.begin(); it != this->client_a_traite.end(); it++) {
        std::cout << *it << ' ';
    }
    cout << endl;
}

void DetecteCycle_directed::print_MapClientBool() {
    cout << "Affichage de la Map des clients" << endl;
    for (int i(1); i <= this->dimension; i++) {
        cout << i << " = " << this->MapClientBool[i] << endl;
    }
}

vector<vector<int> > DetecteCycle_directed::getCycleD() {
  return this->cycleD;
}

vector<vector<int> > DetecteCycle_directed::getCycleS() {
  return this->cycleS;
}

map<int, int> DetecteCycle_directed::getMapClientBool() {
  return this->MapClientBool;
}
