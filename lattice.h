#include <vector>
#include <random>
#include "qubit.h"

class Lattice {
    public:
        Lattice(int n_columns, int n_rows, double J, double T);
        double J;
        double T;
        double beta;
        double h;
        std::vector<std::vector<int>> get_neighbours(int i, int j);
        void switch_neighbours(int i1, int j1, int i2, int j2);
        std::vector<Qubit> lattice;
        void flip_spin(Qubit& qubit);
        std::vector<int> parity_check();
        std::vector<double> hamiltonian_z(std::vector<int> parities);
        std::vector<double> hamiltonian_x(std::vector<int> parities);
        double calculate_energy();
        double calculate_weight();
};