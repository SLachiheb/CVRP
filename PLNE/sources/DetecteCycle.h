#ifndef DETECTECYCLE_H
#define DETECTECYCLE_H

#include <iostream>
#include <map>
#include <vector>
#include <list>

using namespace std;

class DetecteCycle {

    private :

    int                         dimension;
    map<int, vector<int> >      mAdj;
    vector<int>                 cycleD; /*Cycle avec Depot*/
    vector<vector<int> >        cycleS; /*Cycle sans Depot*/
    list<int>                   client_a_traite;
    map<int, int>              MapClientBool;

    void _find_cycleAvecDepot();

    void _find_cycleSansDepot();

    public :

    DetecteCycle(const int &, map<int, vector<int> > , list<int> );

    void run();

    void print_cycleAvecDepot();

    void print_cycleSansDepot();

    void print_clientATraite();

    void print_MapClientBool();

    /*Getteur*/
    vector<int>           getCycleD();
    vector<vector<int> >  getCycleS();
    map<int, int>         getMapClientBool();

};

#endif // DETECTECYCLE_H
