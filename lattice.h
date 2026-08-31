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
        std::mt19937 rng;
        std::vector<std::vector<int>> get_neighbours(int i, int j);
        std::vector<std::vector<Qubit>> lattice;
        void switch_neighbours(int i1, int j1, int i2, int j2);
        Qubit flip_spin(Qubit& contol, int i, int j);
        int CNOT(Qubit& control, Qubit& qubit);
        std::vector<int> parity_check();
        std::vector<std::vector<int>> choose_random_state();
        double hamiltonian_z(std::vector<std::vector<int>> parities);
        double hamiltonian_x(std::vector<std::vector<Qubit>> qubits);
        double calculate_energy();
        double calculate_weight();
};