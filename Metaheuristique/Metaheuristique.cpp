#include "Metaheuristique.h"

/************** Constructeur usuel **************/
Metaheuristique::Metaheuristique(Data data, int k, int nb_enfant, int it_grasp, int it_els) : heuristique_main(data), best_heuristique(data)
{
    this->k_proche_voisin   = k;
    this->nb_enfant_par_mut = nb_enfant;
    this->iteration_Grasp   = it_grasp;
    this->iteration_ELS     = it_els;
    this->index_iterations  = 1;
}

/************** Coeur de la classe **************/
void Metaheuristique::run()
{
    double temps;
    clock_t t1, t2;

    for (int i(0); i < this->iteration_Grasp; i++) {
        //std::cout << "----------------GRASP-------------------------" << std::endl;
        /*Phase 1 du Grasp : Generation d'une solution par une heuristique gloutonne randomis»e + rechercheLocale*/

        t1 = clock();

        heuristique_main.route_first_cluster_second(this->k_proche_voisin); // 1 er C and W
        // Rajouter une methode de Clark and Wight.
        //std::cout << "Je suis ici" << std::endl;
        //std::cout << "-----------------ELS-----------------------" << std::endl;
        for (int j(0); j < this->iteration_ELS; j++) {
            /*Phase 2 du Grasp : Amelioration par recherche locale - ici remplac»e par l'ELS.
             *
             **Phase 1 de l'ELS : Concatenation des tournees principales de l'heuristique principale.
             */
            heuristique_main.concatenation_tournee_S();

            //std::cout << "----------------ENFANTS----------------------" << std::endl;

            /*Phase 2 de l'ELS : Mutation pour obtenir des enfants.*/
            this->mutation_enfants();

            /*Phase 3 de l'ELS : Recherche Locale sur les enfants.
             **Transformation en solution complete par Split.
             */
            this->split_rechercheLocale_enfants();

            /*Selection du meilleur enfant.*/
            this->selection_meilleur_enfant();

            //std::cout << "----------------FIN ELS----------------------" << std::endl;
        }

        t2 = clock();
        temps = (float)(t2-t1)/CLOCKS_PER_SEC;
        printf("temps = %f\n", temps);

        std::cout << "Cout : " << heuristique_main.getCout_() << std::endl; /*A SUPPRIMER*/

        /*Ajout dans la liste des solutions*/
        bool verif = this->_add_rcl_GRASP(temps);
        if (verif == false)
            i--;

        std::cout << "----------------------------" << std::endl; /*A SUPPRIMER*/

        /*Reset de l'heuristique_main :*/
        this->_reset_HeuristiqueMain();
    }
    this->_initBestRclGrasp();

    /*VÈrification de la faisabilitÈ de la solution */
    if (faisabilite(this->best_heuristique)) {
        std::cout << "Solution : " << getBestCout() << std::endl;
        this->print_Cost_Time();
    } else
        std::cout << "Solution non faisable" << std::endl;
    std::cout << "Je suis ici" << std::endl;
    /*Debbug*/
    debbug_tournee(this->best_heuristique);


}

/************** Partie Mutation de l'ELS **************/
void Metaheuristique::mutation_enfants() /*Dois-je faire une mise a jour sur le cout*/
{
    /*Creation de nb_enfant_par mut*/
    for (int i(0); i < this->nb_enfant_par_mut; i++) {
        this->enfants.push_back(Heuristique (this->heuristique_main));
        /*Mutation de l'enfant nouvellement cree :*/
        this->enfants[i].mutation();
    }
}

/************** Partie Recherche Locale sur enfant de l'ELS **************/
void Metaheuristique::split_rechercheLocale_enfants()
{
    for (size_t i(0); i < this->enfants.size(); i++) {
        //std::cout << "Enfant " << i+1 << " :" << std::endl;
        /*Effacement de donnee anterieur de l'heuristique_main*/
        this->enfants[i].setTournee_S_clear();
        /*Application recherche locale*/
        enfants[i].split();
        enfants[i].rechercheLocal();
    }
}

/************** Partie selection du meilleurs enfant ELS **************/
void Metaheuristique::selection_meilleur_enfant()
{
    int     index_best = -1;
    double  cout_enfant = std::numeric_limits<double>::infinity();
    for (int i(0); i < this->enfants.size(); i++) {
        if (this->enfants[i].getCout_() < cout_enfant) {
            index_best = i;
            cout_enfant  = this->enfants[i].getCout_();
        }
    }
    /*Mise a jour de la meilleur heuristique*/
    if (heuristique_main.getCout_() > cout_enfant) {
        this->heuristique_main = this->enfants[index_best];
    }
    this->enfants.clear();
}

/************** Annexe **************/
void Metaheuristique::_reset_HeuristiqueMain () {
    heuristique_main.setTour_geant_clear();
    heuristique_main.setTournee_S_clear();
    heuristique_main.setCout(0.);
}

bool Metaheuristique::_add_rcl_GRASP(double time) {
    if (this->faisabilite(this->heuristique_main)) {
        this->rcl_grasp.push_back(Heuristique(this->heuristique_main));
        /*Recuperer tous les coups des heuristiques faisaible :*/
        this->list_cost.push_back(this->heuristique_main.getCout_());
        /*Recuperer tous les temps des heuristique faisaibles :*/
        this->list_time.push_back(time);
        /*Recuperer toutes les iterations :*/
        this->list_iterations.push_back(this->index_iterations++);
        //debbug_tournee(this->heuristique_main);
        return true;
    }
    return false;
}

void Metaheuristique::print_Cost_Time() {
    std::list<double>::iterator it;
    std::cout << "list_cost = " << std::endl;
    std::cout << "[";
    for(it = this->list_cost.begin(); it!=this->list_cost.end(); ++it)
    {
        std::cout << *it << "," << std::endl;    //On accËde ‡ l'ÈlÈment pointÈ via l'Ètoile
    }
    std::cout << "]" << std::endl;

    std::cout << "list_time = " << std::endl;
    std::cout << "[";
    for(it = this->list_time.begin(); it!=this->list_time.end(); ++it)
    {
        std::cout << *it << "," << std::endl;    //On accËde ‡ l'ÈlÈment pointÈ via l'Ètoile
    }
    std::cout << "]" << std::endl;

    std::list<int>::iterator its;
    std::cout << "List iteration : " << std::endl;
    std::cout << "[";
    for(its = this->list_iterations.begin(); its!=this->list_iterations.end(); ++its)
    {
        std::cout << *its << "," << std::endl;    //On accËde ‡ l'ÈlÈment pointÈ via l'Ètoile
    }
    std::cout << "]" << std::endl;

    std::cout << "list_cost = " ;
    std::cout << "[";
    for(it = this->list_cost.begin(); it!=this->list_cost.end(); ++it)
    {
        std::cout << *it << ",";    //On accËde ‡ l'ÈlÈment pointÈ via l'Ètoile
    }
    std::cout << "]" << std::endl;

    std::cout << "list_time = " ;
    std::cout << "[";
    for(it = this->list_time.begin(); it!=this->list_time.end(); ++it)
    {
        std::cout << *it << ",";    //On accËde ‡ l'ÈlÈment pointÈ via l'Ètoile
    }
    std::cout << "]" << std::endl;

    std::cout << "List iteration : " << std::endl;
    std::cout << "[";
    for(its = this->list_iterations.begin(); its!=this->list_iterations.end(); ++its)
    {
        std::cout << *its << ",";    //On accËde ‡ l'ÈlÈment pointÈ via l'Ètoile
    }
    std::cout << "]" << std::endl;
}

void Metaheuristique::_initBestRclGrasp() {
    double solutionBest = std::numeric_limits<double>::infinity();
    int indexbest = -1;
    for (int i(0); i < this->rcl_grasp.size(); i++) {
        if (this->rcl_grasp[i].getCout_() < solutionBest) {
            solutionBest    = this->rcl_grasp[i].getCout_();
            indexbest = i;
        }
    }
    this->best_heuristique = this->rcl_grasp[indexbest];
}

double  Metaheuristique::getBestCout() {
    return this->best_heuristique.getCout_();
}

std::vector<std::vector<int>>   Metaheuristique::getBestSolution() {
    return this->best_heuristique.getTournee_S();
}

bool Metaheuristique::faisabilite(Heuristique &h) {
    Data data = h.getData();
    if (data.getNb_vehicules() < h.getTournee_S().size()) {
        std::cout << "----------------------------" << std::endl;
        std::cout << "Erreur du nombre de vehicule : " << h.getTournee_S().size() << std::endl;
        return false;
    }
    if (data.getOPT() > h.getCout_()) {
        std::cout << "----------------------------" << std::endl;
        std::cout << "Erreur de optimum : " <<  h.getCout_() << std::endl;
        return false;
    }
    if (not this->check_capacity(h)) {
        std::cout << "----------------------------" << std::endl;
        std::cout << "Erreur de capacitÈ" << std::endl;
        return false;
    }
    return true;
}

bool Metaheuristique::check_capacity(Heuristique &h) {
    std::vector<std::vector<int>> tour = h.getTournee_S();
    Data data = h.getData();
    int demande;

    for (int i(0); i < tour.size(); i++) {
        demande = 0;
        for (int j(0); j < tour[i].size(); j++) {
            demande += data.getDemande_client(tour[i][j]);
        }
        if (demande > data.getCapacite()) {
            return false;
        }
    }
    return true;
}

void Metaheuristique::debbug_tournee(Heuristique &h) {
    /*Test de la solution*/
    std::vector<std::vector<int>> solution = h.getTournee_S();
    Data data = this->best_heuristique.getData();

    double sommeT = 0.;
    /*Affichage de la tournee :*/
    for (int i(0); i < solution.size(); i++) {
        //int demande = 0;
        double somme = 0.;
        if (solution[i].size() == 1) {
            somme +=  data.getDistance_ij(1, solution[i][0]);
            //std::cout << 1 << ", " << solution[i][0] << " => " << data.getDistance_ij(1, solution[i][0]) << std::endl;
            somme +=  data.getDistance_ij(solution[i][0], 1);
            //std::cout << solution[i][0] << ", " << 1 << " => " << data.getDistance_ij(solution[i][0], 1) << std::endl;
        } else {
            for (int j(0); j < solution[i].size()-1; j++) {
                if (j == 0) {
                    somme += data.getDistance_ij(1, solution[i][j]);
                    //std::cout << 1 << ", " << solution[i][j] << " => " << data.getDistance_ij(1, solution[i][j]) << std::endl;
                }
                somme += data.getDistance_ij(solution[i][j], solution[i][j+1]);
                //std::cout << solution[i][j] << ", " << solution[i][j+1] << " => " << data.getDistance_ij(solution[i][j], solution[i][j+1]) <<std::endl;
                if (j == solution[i].size()-2) {
                    somme += data.getDistance_ij(solution[i][j+1], 1);
                    //std::cout << solution[i][j+1] << ", " << 1 <<" => " <<  data.getDistance_ij(solution[i][j+1], 1) << std::endl;
                }
            }
        }
        sommeT += somme;
    }
    std::cout << "Cout Total : " << sommeT <<std::endl<<std::endl;

    for (int i(0); i < solution.size(); i++) {
        std::cout << "( ";
        for (int j(0); j < solution[i].size(); j++) {
            std::cout << solution[i][j] << " ";
        }
        std::cout << ")" << std::endl;
    }
}
