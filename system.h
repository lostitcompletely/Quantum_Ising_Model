#include <vector>
#include <random>
#include "lattice.h"

class System {
    public:
        System(double J, double T, int n_columns, int n_rows, int seed);
        std::mt19937 rng;
        Lattice grid;
        void create_initial_state();
        void fluctuate();
        std::vector<int> choose_config(int n);
        bool accept_move();
        void apply_hamiltonian();
        void simulate(int nsteps);
};