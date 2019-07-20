#include "Heuristique.h"

/********************* Constructeur ********************/
Heuristique::Heuristique(Data d) : data(d)
{
}

/********************* Constructeur de recopie *********************/
Heuristique::Heuristique(const Heuristique &h) : data(h.data){
    this->cout_ = h.cout_;
    this->tour_geant_T = h.tour_geant_T;
    this->tournee_S = h.tournee_S;
}

/******************** Heuristique de construction : route-First and cluster-Seconde ********************/

void Heuristique::route_first_cluster_second(const int &k) {
    this->k_plus_proche_voisin(k);
    this->split();
    this->rechercheLocal();
}

void Heuristique::k_plus_proche_voisin(const int &k) {
    // Liste de client non traite :
    std::list<int> clients;
    const int NBSELECTIONS= k;

    // Remplir la liste avec les identifiants des clients :
    for (int i(1); i <= data.getDimension(); i++) {
        clients.push_back(i);
    }

    //Produire une liste ordonn»e de client du plus proche au plus loin du depot :
    std::vector<std::pair<int, double>> proche_depot;
    std::vector<std::pair<int, double>> proche_voisin;

    //Initiliase le vector de plus proche depot (le depot est toujours represente par l'index de sommet 1) :
    proche_depot = distance_sommet_pivot(1, clients); // mettre le depot de maniere automatique a l'aide tour_geant_T[0]

    // Choisir le client le plus proche du depot parmi 5 au hasard : (Hypothese : toujours plus de 5 sommets au depart)
    int pioche_aleatoire = ((rand() % (NBSELECTIONS))); /*Entre 0 et 4 donc 5 choix possibles*/

    //std::cout << "Je choisis le chiffre : " << pioche_aleatoire << std::endl;
    std::pair<int,double> pioche_parmi_cinq = proche_depot[pioche_aleatoire];
    //std::cout << "Le sommet selectionne est le sommet client : " << pioche_parmi_cinq.first << " "  << pioche_parmi_cinq.second << std::endl;

    // Generalisation de l'algorithme sur tout les clients :
    while (true) {
        proche_voisin = distance_sommet_pivot(pioche_parmi_cinq.first, clients);
        if (clients.size() == 0) {
            break;
        }
        if (clients.size() < NBSELECTIONS) {
            pioche_aleatoire = ((rand() % (clients.size())));
        } else {
            pioche_aleatoire = ((rand() % (NBSELECTIONS)));
        }
        //std::cout << "Je choisis le chiffre : " << pioche_aleatoire << std::endl;
        pioche_parmi_cinq = proche_voisin[pioche_aleatoire];
        //std::cout << "Le sommet selectionne est le sommet client : " << pioche_parmi_cinq.first << " "  << pioche_parmi_cinq.second << std::endl;
    }
}

void Heuristique::split () {
    //Donnee :
    std::map<std::pair<int, int>, double>       c_ij;                     // c_ij cout des arcs (i,j).
    std::vector<std::pair<int, double>>         v_i;                      // V_i < identifiant sommet i, distance en plus court chemin entre le depot et le client i >.
    std::vector<std::pair<int, int>>            p_i;                      // P_i predecesseur de i sur le plus court chemin menant de 0 á j.
    double                                      cost;

    // Initialisation du tableau de cout c_ij :
    std::vector<int> tour_geant(std::begin(this->tour_geant_T), std::end(this->tour_geant_T));   // transformation de la liste en vector pour plus de facilited d'acces.
    for (int i(1); i < tour_geant.size(); i++) {                                              // i = 1 car on ne compte pas l'arc depot-client1 dans un premier temps.
        if (i != tour_geant.size() - 1) {                                                        // evite la sortie de tableau.
            c_ij[std::make_pair(tour_geant[i], tour_geant[i+1])] = data.getDistance_ij(tour_geant[i],tour_geant[i+1]);//calcul_distance(data.getCoordonnee()[tour_geant[i]], data.getCoordonnee()[tour_geant[i+1]]);
        }
        c_ij[std::make_pair(tour_geant[0], tour_geant[i])] = data.getDistance_ij(tour_geant[0],tour_geant[i]); //calcul_distance(data.getCoordonnee()[tour_geant[0]], data.getCoordonnee()[tour_geant[i]]);
    }

    //Algorithme de split :
    v_i.push_back(std::make_pair(tour_geant.front(), 0.));
    p_i.push_back(std::make_pair(tour_geant.front(), 0.));

    for (int i(1); i < tour_geant.size(); i++) {
        v_i.push_back(std::make_pair(tour_geant[i], std::numeric_limits<double>::infinity()));
        p_i.push_back(std::make_pair(tour_geant[i], 0));
    }

    for (int i(1); i < tour_geant.size(); i++) {
        int j(i);                                                                               // etude de chaque chemin possible a partir d'un sommet i.
        int charge(0);                                                                          // taux de remplissage d'un vehicule.
        do {
            charge = charge + data.getDemande_client()[tour_geant[j]];
            if (i == j) {
                cost = c_ij[std::make_pair(tour_geant[0], tour_geant[i])] * 2;                       // valeur arc symetrique.
            } else {
                cost = cost - c_ij[std::make_pair(tour_geant[0], tour_geant[j-1])] + c_ij[std::make_pair(tour_geant[j-1], tour_geant[j])] + c_ij[std::make_pair(tour_geant[0], tour_geant[j])];
            }
            if ((charge <= data.getCapacite()) && ((v_i[i-1].second + cost) < v_i[j].second)) {
                v_i[j].second = v_i[i-1].second + cost;     // voir si modification ????
                p_i[j].second = i - 1;
            }
            j = j + 1;
        } while ((j < tour_geant.size()) && (charge <= data.getCapacite())) ;   // Attention sur l'indice de j
    }
    //debbug_v_i(v_i);
    //debbug_p_i(p_i);

    extraction_split(p_i);
    //this->cout_ = v_i[v_i.size()-1].second;
    this->setCout(v_i[v_i.size()-1].second);
    //debbug_s_i(this->tournee_S);
}

void Heuristique::extraction_split(const std::vector<std::pair<int, int>> &p_i) {
    // Initialisation de la solution complete a vide :
    tournee_S.clear();
    int j(p_i.size() - 1);
    do {
        std::vector<int> tournee;
        for (int k(p_i[j].second + 1); k <=j; k++) {
            tournee.push_back(p_i[k].first);
        }
        this->tournee_S.push_back(tournee);
        j = p_i[j].second;
    } while (j > 0); // Attention indice
}


/******************** Recherche Locale ********************/

void Heuristique::rechercheLocal() {
    rechercheLocale_2_opt_intra_tournee();
    rechercheLocale_2_opt_inter_tournee();
    /*Supprimer les tourn»es vide :*/
    removeTourneeVide();
    //rechercheLocale_swap_externe();
    /*Supprimer les tourn»es vide :*/
    //removeTourneeVide();
}

void Heuristique::removeTourneeVide()
{
    for (int i(0); i < this->getTournee_S().size(); i++) {
        if (this->getTournee_S().size() == 0) {
            this->tournee_S.erase(this->tournee_S.begin()+i);
            i = 0;
        }
    }
}

/*Recherche Locale Swap externe*/

void Heuristique::rechercheLocale_swap_externe()
{
    bool ameliore, ameliore_total = true;
    std::deque<std::deque<int> > tournee_S1(this->tournee_S.size());

    /*Construction tournee_S deque*/
    for (int i(0); i < this->tournee_S.size(); i++) {
        if (this->tournee_S[i].size()==0) {
            tournee_S1[i].push_back(this->tour_geant_T.front());
            tournee_S1[i].push_back(this->tour_geant_T.front());
        }
        for (int j(0); j < this->tournee_S[i].size(); j++) {
            if (j==0) {
                tournee_S1[i].push_back(this->tour_geant_T.front());
            }
            tournee_S1[i].push_back(this->tournee_S[i][j]);
            if (j==this->tournee_S[i].size()-1) {
                tournee_S1[i].push_back(this->tour_geant_T.front());
            }
        }
    }

    while (ameliore_total) {
        ameliore_total = false;
        /*Transfomer tournee_S en deque + cout version 1 = > T1 vers T2*/
        for (int index_tournee(0); index_tournee < tournee_S1.size(); index_tournee++) {
            ameliore = true;
            while (ameliore == true) {
                ameliore = false;
                if (voisinage_swap_externe(tournee_S1[index_tournee], tournee_S1[(index_tournee+1)%tournee_S1.size()] ) ) {
                    ameliore = true;
                    ameliore_total = true;
                }
            }
        }
        /*Transfomer tournee_S en deque + cout version 1 = > T2 vers T1*/
        for (int index_tournee(0); index_tournee < tournee_S1.size(); index_tournee++) {
            ameliore = true;
            while (ameliore == true) {
                ameliore = false;
                if (voisinage_swap_externe(tournee_S1[(index_tournee+1)%tournee_S1.size()], tournee_S1[index_tournee] ) ) {
                    ameliore = true;
                    ameliore_total = true;
                }
            }
        }

    }

    /*Modification de tournee :*/
    for (int i(0); i < tournee_S1.size(); i++) {
        this->tournee_S[i].clear();
        for (int j(0); j < tournee_S1[i].size(); j++) {
            if (j==0 || j==tournee_S1[i].size()-1) {
                continue;
            }
            this->tournee_S[i].push_back(tournee_S1[i][j]);
        }
    }

}

bool Heuristique::voisinage_swap_externe(std::deque<int> &tournee_1, std::deque<int> &tournee_2){
    bool ameliore = false;
    double solution_init = 0.;

    int demande = 0;
    int demande_i = 0;

    double delta = 0.;
    double solution_best = this->getCout_();
    //std::cout << "COUT = " << solution_best <<std::endl;
    std::pair<int,int> solution = std::make_pair(-1, -1);

    /*Calculer le cout de la demande en tournee_2 :*/
    for (int i(0); i < tournee_2.size(); i++) {
        demande += this->data.getDemande_client(tournee_2[i]);
    }

    for (int i(1); i < tournee_1.size()-1; i++) {
        demande_i = this->data.getDemande_client(tournee_1[i]);
        for (int j(1); j <= tournee_2.size()-1; j++) {
            delta = data.getDistance_ij(tournee_2[j-1], tournee_1[i]) + data.getDistance_ij(tournee_1[i], tournee_2[j]) \
            + data.getDistance_ij(tournee_1[i-1], tournee_1[i+1]) \
            - data.getDistance_ij(tournee_1[i-1], tournee_1[i]) - data.getDistance_ij(tournee_1[i], tournee_1[i+1]) \
            - data.getDistance_ij(tournee_2[j-1], tournee_2[j]);

            solution_init = this->getCout_() + delta;

            if (delta < 0 && solution_init < solution_best && demande_i + demande <= data.getCapacite()) { // Controler le cout ????
                solution_best = solution_init;
                solution = std::make_pair(i, j);
                ameliore = true;
            }
        }
    }
    if (ameliore) {
        tournee_2.insert(tournee_2.begin()+solution.second, tournee_1[solution.first]);
        tournee_1.erase(tournee_1.begin()+solution.first);
        //std::cout << "Amelioration Swap de " << this->getCout_() << " en " << solution_best << std::endl;
        this->setCout(solution_best);
    }
    return ameliore;
}

/*Recherche Locale 2opt_Intra*/

void Heuristique::rechercheLocale_2_opt_intra_tournee() {
    bool ameliore;

    for (int index_tournee(0); index_tournee < this->tournee_S.size(); index_tournee++) {
        ameliore = true;
        while (ameliore == true) {
            ameliore = false;
            if (voisinage_2_opt_intra(this->tournee_S[index_tournee])) {
                ameliore = true;
            }
        }
    }
}

int Heuristique::mod ( int x , int y )
{
    return x >= 0 ? x % y : y - 1 - ((-x-1) % y) ;
}

bool Heuristique::voisinage_2_opt_intra(std::vector<int> &tournee) {
    /*Si pas assez de tournee pour faire des permuatation d'arc :*/
    bool ameliore = false;

    if (tournee.size() < 4)
        return ameliore;

    /*Variable correspondant aux arcs:*/
    double c_ux, c_vy, c_uv, c_xy;
    double delta =  std::numeric_limits<double>::infinity();
    double solution_init = std::numeric_limits<double>::infinity();
    double solution_best = this->cout_;
    std::pair<std::pair<int,int>, std::pair<int, int> > solution = std::make_pair(std::make_pair(-1,-1),std::make_pair(-1,-1)) ;

    /*Premier arrive, premier sorti*/
    std::deque<int> list_courant(tournee.begin(), tournee.end());
    list_courant.push_front(this->tour_geant_T.front());

    /*Voisinage de solution en 2_opt*/
    std::vector<int> tour_geant(list_courant.begin(), list_courant.end());

    /*Affichage Tour geant*/
    /*std::cout << "Tour geant : ";
     for (size_t i(0); i < tour_geant.size(); i++) {
     std::cout << tour_geant[i] << " ";
     }
     std::cout << std::endl;*/

    /*Affichage liste courant*/
    /*std::cout << "Liste courant : ";
     for (size_t i(0); i < list_courant.size(); i++) {
     std::cout << list_courant[i] << " ";
     }
     std::cout << std::endl;*/

    /*Affichage du cout*/
    //std::cout << this->cout_ << std::endl;

    /*Iteration sur les arcs*/
    for (int arc1(0); arc1 < tour_geant.size(); arc1++) {
        /*delta = C_uv + C_xy - C_ux - C_vy (Remplace l'arc (u,x) et (v,y) par (u,v) et (x,y) :*/
        c_ux = data.getDistance_ij(tour_geant[arc1], tour_geant[mod((arc1+1), int(tour_geant.size()))]);
        /*Affichage des arcs selectionnees*/
        //std::cout << "Selection (" << tour_geant[arc1] << "," << tour_geant[mod((arc1+1), tour_geant.size())] << ")" << std::endl;
        for (int arc2(0); arc2 < tour_geant.size(); arc2++) {

            /*Pour eviter de faire des changement arcs non utilie :*/
            if (arc2 != mod((arc1-1), int(tour_geant.size())) && arc2 != arc1 && arc2 != mod((arc1+1), int(tour_geant.size()))) {
                /*Affichage des arcs selectionnees*/
                //std::cout << "(" << tour_geant[arc2] << "," << tour_geant[mod((arc2+1), tour_geant.size())] << ")" << std::endl;
                c_vy = data.getDistance_ij(tour_geant[arc2], tour_geant[mod((arc2+1), int(tour_geant.size()))]);
                /*Operation de changement*/
                c_uv = data.getDistance_ij(tour_geant[arc1], tour_geant[arc2]);
                c_xy = data.getDistance_ij(tour_geant[mod((arc1+1), int(tour_geant.size()))], tour_geant[mod((arc2+1), int(tour_geant.size()))]);
                delta = c_uv + c_xy - c_ux - c_vy;
                /*Gain en inter-change des deux arcs :*/
                solution_init = this->cout_ + delta;
                /*Selection :*/
                if (solution_init < solution_best) {
                    solution_best = solution_init;
                    solution = std::make_pair(std::make_pair(tour_geant[arc1],tour_geant[mod((arc1+1), int(tour_geant.size()))]), std::make_pair(tour_geant[arc2],tour_geant[mod((arc2+1), int(tour_geant.size()))]));
                    //std::cout << "solutionInit : " << solution_init << ", delta : " << delta << ", cout_ : " << cout_ << std::endl;
                    ameliore = true;
                }
            }
        }
    }
    /*Affichage*/
    //std::cout << "Resultat : " << solution_best << ", (" << solution.first.first << "," << solution.first.second \
    << "), (" << solution.second.first << "," << solution.second.second << ")" << std::endl;

    if (ameliore) {
        /*Permet de rajouter un sommet en debut de liste ce que vector ne permet pas :*/
        std::deque<int> tournee_tmp(tournee.begin(), tournee.end());
        tournee_tmp.push_front(this->tour_geant_T.front());

        /*Insertion dans le tour geant : (x, v) + Mise a jour du tour_geant_T*/
        swap_arc(tournee_tmp, solution.first.second, solution.second.first);
        /*Copy*/
        tournee.clear();
        for (int i(1); i < tournee_tmp.size(); i++) {
            tournee.push_back(tournee_tmp[i]);
        }
        /*Mise a jour solution_vrp / cout*/
        this->setCout(solution_best);
    }
    return ameliore;
}
/*Recherche Locale Inter*/

void Heuristique::rechercheLocale_2_opt_inter_tournee() {
    bool ameliore;
    //A voir si on met pas le while a l'exterieur !!!
    //Tournee 1:
    for (int indexTournee_i(0); indexTournee_i < this->tournee_S.size() - 1; indexTournee_i++) {
        //Tournee 2:
        for (int indexTournee_j(indexTournee_i+1); indexTournee_j < this->tournee_S.size(); indexTournee_j++) {
            ameliore = true;
            while (ameliore == true) {
                ameliore = false;
                if (voisinage_2_opt_inter(indexTournee_i, indexTournee_j)) {
                    ameliore = true;
                }
            }
        }
    }
}

bool Heuristique::voisinage_2_opt_inter(int indexTournee_i, int indexTournee_j) {
    /*Modification des vectors en Deque pour ajouter en tete et queue le depot :*/
    std::deque<int> tournee_i(this->tournee_S[indexTournee_i].begin(), this->tournee_S[indexTournee_i].end());
    tournee_i.push_back(this->tour_geant_T.front());
    tournee_i.push_front(this->tour_geant_T.front());
    std::deque<int> tournee_j(this->tournee_S[indexTournee_j].begin(), this->tournee_S[indexTournee_j].end());
    tournee_j.push_back(this->tour_geant_T.front());
    tournee_j.push_front(this->tour_geant_T.front());

    int charge_tournee_i = 0;
    int charge_tournee_j = 0;
    int somme_d_i = 0, somme_d_j;
    int charge_i, charge_j;
    double dist_tournee;
    std::deque<int> best_tournee_i, best_tournee_j;
    double solution_best = this->getCout_();
    bool ameliore = false;

    /*Calcul la charge de chaque tournee*/
    for (int i(0); i < tournee_i.size(); i++) {
        charge_tournee_i += data.getDemande_client(tournee_i[i]);
    }
    for (int j(0); j < tournee_j.size(); j++) {
        charge_tournee_j += data.getDemande_client(tournee_j[j]);
    }
    //std::cout << "Charge tournee 1 = " << charge_tournee_i << std::endl;
    //std::cout << "Charge tournee 2 = " << charge_tournee_j << std::endl << std::endl;

    std::deque<int>::iterator il;
    std::deque<int>::iterator jl;
    int i,j;
    for (il=tournee_i.begin()+1, i=0; il!=tournee_i.end(); il++, i++) {
        //std::cout << "Interchange sommet " << *il << " -----------------" << std::endl;
        somme_d_i += data.getDemande_client(tournee_i[i]);
        somme_d_j = 0;
        for (jl=tournee_j.begin()+1, j=0; jl!=tournee_j.end(); jl++, j++) {
            somme_d_j += data.getDemande_client(tournee_j[j]);
            /*Creation des deux vectors :*/
            std::deque<int> hybride_i;
            hybride_i.insert(hybride_i.begin(), tournee_i.begin(), il);
            hybride_i.insert(hybride_i.end(), jl, tournee_j.end());

            std::deque<int> hybride_j;
            hybride_j.insert(hybride_j.begin(), tournee_j.begin(), jl);
            hybride_j.insert(hybride_j.end(), il, tournee_i.end());

            charge_i = somme_d_i + (charge_tournee_j - somme_d_j);
            charge_j = somme_d_j + (charge_tournee_i - somme_d_i);

            //debbug_deque(1, hybride_i);
            //std::cout << "Charge tourne i = " << charge_i << std::endl;
            //debbug_deque(2, hybride_j);
            //std::cout << "Charge tourne j = " << charge_j << std::endl;

            /*Verification des contraintes :*/
            /*Contrainte de charge*/
            if (charge_i > data.getCapacite() || charge_j > data.getCapacite()) {
                //std::cout << std::endl;
                break;
            } else {
                //std::cout << "Capacite OK" << std::endl;
                /*Contrainte de distance*/
                dist_tournee = data.getDistance_ij(tournee_i[i], tournee_j[j+1]) + data.getDistance_ij(tournee_i[i+1], tournee_j[j]) \
                - data.getDistance_ij(tournee_i[i], tournee_i[i+1]) - data.getDistance_ij(tournee_j[j], tournee_j[j+1]);
                //std::cout << "Delta : " << dist_tournee << std::endl;
                //std::cout << data.getDistance_ij(tournee_i[i], tournee_j[j+1]) << std::endl;
                //std::cout << data.getDistance_ij(tournee_i[i+1], tournee_j[j]) << std::endl;
                //std::cout << data.getDistance_ij(tournee_i[i], tournee_i[i+1]) << std::endl;
                //std::cout << data.getDistance_ij(tournee_j[j], tournee_j[j+1]) << std::endl;
                /*Distance diminue avec la permuation d'arc*/
                if (dist_tournee < -0.00001 && this->getCout_() + dist_tournee < solution_best) {
                    //std::cout << "---->Distance ameliore de " << solution_best << " en " << this->cout_+dist_tournee << std::endl;
                    solution_best = this->getCout_() + dist_tournee;
                    best_tournee_i = hybride_i;
                    best_tournee_j = hybride_j;
                    ameliore = true;
                }
            }
            //std::cout << std::endl;
        }
    }

    /*Si ameliore = true faire la Mise a jour des tournee_i et tournee_j avec le best*/
    if (ameliore == true) {
        /*De Deque en Vector :*/
        this->tournee_S[indexTournee_i].clear();
        for (int i(0); i < best_tournee_i.size(); i++) {
            if (i == 0 || i == best_tournee_i.size()-1) {
                continue;
            }
            this->tournee_S[indexTournee_i].push_back(best_tournee_i[i]);
        }
        this->tournee_S[indexTournee_j].clear();
        for (int i(0); i < best_tournee_j.size(); i++) {
            if (i == 0 || i == best_tournee_j.size()-1) {
                continue;
            }
            this->tournee_S[indexTournee_j].push_back(best_tournee_j[i]);
        }
        //std::cout << "2_opt : " << this->getCout_() << std::endl;
        this->setCout(solution_best);
    }
    return ameliore;
}

/******************** Concatenation ********************/
void Heuristique::concatenation_tournee_S() {
    int id_depot = this->tour_geant_T.front();
    /*Re-initialisation a l'aide des tournee_S par concatenation :*/
    this->tour_geant_T.clear();
    this->tour_geant_T.push_back(id_depot);
    for (int index_t(0); index_t < this->tournee_S.size(); index_t++) {
        for (int index_ti(0); index_ti < this->tournee_S[index_t].size(); index_ti++) {
            this->tour_geant_T.push_back(this->tournee_S[index_t][index_ti]);
        }
    }
}

/******************** Mutation ********************/
void Heuristique::mutation ()
{
    /*Selection de nombre aleatoire entre 1 et tour_geant_T.size() - 1*/
    int index_1 = rand()%(this->tour_geant_T.size() - 1) + 1;   /*rand()%(borne_maximale - borne_minimale ) + borne_minimale;*/
    int index_2;
    do {
        index_2 = rand()%(this->tour_geant_T.size() - 1) + 1;
    } while (index_1 == index_2);

    /*Swap de l'index1 et 2 randomise*/
    int tmp = this->tour_geant_T[index_1];
    this->tour_geant_T[index_1] = this->tour_geant_T[index_2];
    this->tour_geant_T[index_2] = tmp;

    /*Mettre a jour le cout_*/ // Faire attention la derniere case.
    double tmp_cout = this->getCout_() \
    - data.getDistance_ij(this->tour_geant_T[index_1-1], this->tour_geant_T[index_1]) \
    - data.getDistance_ij(this->tour_geant_T[index_1], this->tour_geant_T[(index_1+1)%this->tour_geant_T.size()]) \
    - data.getDistance_ij(this->tour_geant_T[index_2-1], this->tour_geant_T[index_2]) \
    - data.getDistance_ij(this->tour_geant_T[index_2], this->tour_geant_T[(index_2+1)%this->tour_geant_T.size()]) \
    + data.getDistance_ij(this->tour_geant_T[index_1-1], this->tour_geant_T[index_2]) \
    + data.getDistance_ij(this->tour_geant_T[index_2], this->tour_geant_T[(index_1+1)%this->tour_geant_T.size()]) \
    + data.getDistance_ij(this->tour_geant_T[index_2-1], this->tour_geant_T[index_1]) \
    + data.getDistance_ij(this->tour_geant_T[index_1], this->tour_geant_T[(index_2+1)%this->tour_geant_T.size()]);
    this->setCout(tmp_cout);
}

/******************** Tools ********************/
void Heuristique::swap_arc(std::deque<int> &tournee, int &x, int &y) {
    int index_1 = -1;
    int index_2 = -1;
    int index_depot;
    std::vector<int> resultat;

    if (x == y)
        return;

    for (int i(0); i < tournee.size(); i++) {
        if (tournee[i] == x)
            index_1 = i;
    }
    for (int j(0); j < tournee.size(); j++) {
        if (tournee[j] == y)
            index_2 = j;
    }

    //for (size_t i(0); i < tournee.size(); i++) {
    //    std::cout << tournee[i] << " ";
    //}
    //std::cout << std::endl;
    //std::cout << " i : " << index_1 << ", j : " << index_2 << std::endl;

    if (index_1 < index_2) {
        //std::cout << "-----------B-----------" << std::endl;
        //std::cout << index_2 << " " << index_1 << std::endl;
        for (int i(0); i < index_1; i++) {
            resultat.push_back(tournee[i]);
            if (i == 0)
                index_depot = resultat.size()-1;
        }
        //std::cout << index_2 << " " << index_1 << std::endl;
        for (int i(index_2); i>=index_1; i--) {
            //std::cout << i << std::endl;
            resultat.push_back(tournee[i]);
        }
        for (int i(index_2+1); i < tournee.size(); i++) {
            //std::cout << i << std::endl;
            resultat.push_back(tournee[i]);
        }
        //std::cout << "----------B------------" << std::endl;
    }else{
        for (int i(index_2+1); i < index_1; i++) {
            resultat.push_back(tournee[i]);
        }
        for (int i(index_2); i!=index_1-1; i--) {
            if (i == -1)
                i = tournee.size()-1;
            resultat.push_back(tournee[i]);
            if (i == 0)
                index_depot = resultat.size()-1;
        }
    }
    /*Vider le tour_geant_T pour la mise a jour*/
    tournee.clear();
    /*Remettre le depot au debut du tableau:*/
    for (int i(index_depot); i < resultat.size(); i++) {
        tournee.push_back(resultat[i]);
    }
    for (int i(0); i < index_depot; i++) {
        tournee.push_back(resultat[i]);
    }
    /*Affichage du resultat*/
    //std::cout << "Resultat swap arc : ";
    //for (size_t i(0); i < resultat.size(); i++) {
    //    std::cout <<tournee[i] << " ";
    //}
    //std::cout << std::endl;
}

std::vector<std::pair<int, double>> Heuristique::distance_sommet_pivot(const int &index_sommet_pivot, std::list<int> &sommet_traite) {
    std::vector<std::pair<int, double>> distance_depot;
    double distance = 0;
    /*Parcourir la Map de coordonnee de sommet */
    std::list<int>::const_iterator il;
    for (il=sommet_traite.begin(); il!=sommet_traite.end(); il++) {
        // condition pour eviter de calculer la distance entre deux mÕmes points.
        // condition pour ne pas calculer des points deja present dans le tour geant.
        if (*il != index_sommet_pivot) {
            distance = data.getDistance_ij(index_sommet_pivot,*il);
            distance_depot.push_back(std::make_pair(*il, distance));
        }
    }
    /*Trier selon le deuxieme argument*/
    std::sort(distance_depot.begin(), distance_depot.end(), [] (const std::pair<int, double> &pair1, const std::pair<int, double> &pair2) {
        return pair1.second < pair2.second;
    });
    /*Mettre a jour les sommets traite :*/
    sommet_traite.remove(index_sommet_pivot);
    /*Mettre a jour le tour geant :*/
    this->tour_geant_T.push_back(index_sommet_pivot);
    //std::cout << "Le client " << index_sommet_pivot << " a ete traite" << std::endl;
    //debbug_liste(sommet_traite);

    return distance_depot;
}

/********************** getter et setter sur COUT_ ********************/

std::vector<std::vector<int>>  Heuristique::getTournee_S () {
    return this->tournee_S;
}

std::vector<int> Heuristique::getTour_geant_T() {
    return this->tour_geant_T;
}

Data Heuristique::getData() {
    return data;
}

double Heuristique::getCout_(){
    return this->cout_;
}

void Heuristique::setCout(double newCout) {
    this->cout_ = newCout;
}

void Heuristique::setTournee_S_clear() {
    this->tournee_S.clear();
}

void Heuristique::setTour_geant_clear() {
    this->tour_geant_T.clear();
}
/******************** FONCTION DE DEBBUG ********************/

void Heuristique::debbug_s_i(const std::vector<std::vector<int>> &s_i) {
    for (int i(0); i < s_i.size(); i++) {
        std::cout << "Tournee numero " << i+1 << " : ";
        for (int j(0); j < s_i[i].size(); j++) {
            std::cout << s_i[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Heuristique::debbug_v_i(const std::vector<std::pair<int, double>> &v_i) {
    std::vector<std::pair<int, double>>::const_iterator il;
    for (il=v_i.begin(); il!=v_i.end(); il++) {
        std::cout << "v_i sommet : " << (*il).first << ", cout : " << (*il).second << std::endl;
    }
    std::cout << std::endl;
}

void Heuristique::debbug_p_i(const std::vector<std::pair<int, int>> &p_i) {
    std::vector<std::pair<int, int>>::const_iterator il;
    for (il=p_i.begin(); il!=p_i.end(); il++) {
        std::cout << "p_i sommet : " << (*il).first << ", tournee : " << (*il).second << std::endl;
    }
    std::cout << std::endl;
}

void Heuristique::debbug_cij(const std::map<std::pair<int, int>, double> &c_ij) {
    std::map<std::pair<int, int>, double> ::const_iterator il;
    for (il=c_ij.begin(); il!=c_ij.end(); il++) {
        std::cout << "c_ij(" << (*il).first.first << ", " << (*il).first.second << ") = " << (*il).second << std::endl;
    }
    std::cout << std::endl;
}

void Heuristique::debbug_tour_geant_T(const std::vector<int> &liste) {
    std::vector<int>::const_iterator il;
    std::cout << "Tour_geant_T : ";
    for (il=liste.begin(); il!=liste.end(); il++) {
        std::cout << *il << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

void Heuristique::debbug_tournee_S(const std::vector<std::vector<int>> &tab) {
    for (int i(0); i < tab.size(); i++) {
        std::cout << "Tournee " << i << " : ";
        for (int j(0); j < tab[i].size(); j++) {
            std::cout << tab[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void Heuristique::debbug_distance() {
    std::map<int, std::vector<std::pair<int, double>>> proche_voisin;
    double distance = 0;
    /*Parcourir la Map de coordonnee de sommet */
    for (int index_sommet_pivot(1); index_sommet_pivot <= data.getDimension(); index_sommet_pivot++) {
        std::vector<std::pair<int, double>> distance_depot;
        for (int i(0); i<=data.getDimension(); i++) {
            // condition pour eviter de calculer la distance entre deux mÕmes points.
            // condition pour ne pas calculer des points deja present dans le tour geant.
            if (i != index_sommet_pivot) {
                distance = data.getDistance_ij(index_sommet_pivot, i); //calcul_distance(data.getCoordonnee()[index_sommet_pivot], data.getCoordonnee()[i]);
                distance_depot.push_back(std::make_pair(i, distance));
            }
        }
        /*Trier selon le deuxieme argument*/
        std::sort(distance_depot.begin(), distance_depot.end(), [] (const std::pair<int, double> &pair1, const std::pair<int, double> &pair2) {
            return pair1.second < pair2.second;
        });
        proche_voisin[index_sommet_pivot] = distance_depot;
    }

    /*Debbug affichage liste_plus_proche_voisin*/
    for (int i(1); i <= data.getDimension(); i++) {
        std::cout << "Les sommets voisin de " << i << " : " << std::endl;
        for (int j(0); j < proche_voisin[i].size(); j++) {
            std::cout << "sommet\t" << proche_voisin[i][j].first << "\test a une distance du depot " << i << "\tde  " << proche_voisin[i][j].second << " KM" << std::endl;
        }
    }
    std::cout << std::endl;
}

void Heuristique::debbug_heuristique ()
{
    this->debbug_tour_geant_T(this->tour_geant_T);
    this->debbug_tournee_S(this->tournee_S);
}

void Heuristique::debbug_deque(const int &i, std::deque<int> &tab) {
    std::cout << "Tournee " << i << " : ";
    for (int i(0); i < tab.size(); i++) {
        std::cout << tab[i] << " ";
    }
    std::cout << std::endl;
}
