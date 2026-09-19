#include "system.h"

#include <iostream>
#include <vector>
#include <random>
#include "lattice.h"

class System {
    public:
        std::mt19937 rng;
        Lattice grid;
        System(double J, double T, int n_columns, int n_rows, int seed){
            this -> rng = std::mt19937(seed);
            this -> grid = Lattice(n_columns, n_rows, J, T, 0.0);
        };

        bool accept_move(){
            std::vector<std::vector<Qubit>> microstate = grid.choose_random_state();
            double new_E = grid.calculate_energy(microstate);
            double current_E = grid.calculate_energy(grid.lattice);
            if (new_E < current_E){
                grid.lattice = microstate;
                return true;
            }
            return false;
        };

        void simulate(int nsteps){
            double E = grid.calculate_energy(grid.lattice);
            for (int i=0; i<nsteps; i++){
                accept_move();
                E += grid.calculate_energy(grid.lattice);
            };
            double E_av = E/nsteps;
        };
};