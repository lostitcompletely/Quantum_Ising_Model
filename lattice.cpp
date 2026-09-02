#include <iostream>
#include <vector>
#include <random>
#include "qubit.h"

class Lattice {
    public:
        double J;
        double T;
        double h;
        std::vector<std::vector<Qubit>> lattice;
        std::mt19937 rng;
        Lattice(int n_columns, int n_rows, double J, double T, double h){
            this -> J = J;
            this -> T = T;
            this -> h = h;
            this -> rng = std::mt19937(1234);
            double beta = 1.0 / T;
            //std::vector<std::vector<Qubit>> lattice;
            for (int i = 0; i < n_columns; i++){
                std::vector<Qubit> row;
                for (int j = 0; j < n_rows; j++){
                    row.push_back(Qubit(1.0,1.0));
                };
                lattice.push_back(row);
            };
        };

        std::vector<std::vector<int>> get_neighbours(int i, int j){
            std::vector<std::vector<int>> neighbours;
            int n_columns = lattice.size();
            int n_rows = lattice[0].size();
            neighbours.push_back({(i-1+n_columns)%n_columns, j});
            neighbours.push_back({(i+1)%n_columns, j});
            neighbours.push_back({i, (j-1+n_rows)%n_rows});
            neighbours.push_back({i, (j+1)%n_rows});
            return neighbours;
        };

        void switch_neighbours(int i1, int j1, int i2, int j2){
            Qubit temp = lattice[i1][j1];
            lattice[i1][j1] = lattice[i2][j2];
            lattice[i2][j2] = temp;
        };

        Qubit flip_spin(Qubit& qubit){
            std::vector<std::vector<int>> M = {{0,1}, {1,0}};
            for (int i=0; i<2; i++){
                double new_alpha = M[i][0]*qubit.get_state()[0] + M[i][1]*qubit.get_state()[1];
                double new_beta = M[i][0]*qubit.get_state()[1] + M[i][1]*qubit.get_state()[0];
                qubit.set_state(new_alpha, new_beta);
            };
            return qubit;
        }

        int CNOT(Qubit &control,int i, int j){
            if (control.measure_spin() == 1){
                flip_spin(control);
                return 1;
            } else {
                return -1;
            }
        };

        std::vector<std::vector<int>> get_parities(std::vector<std::vector<Qubit>> qubits){
            Qubit control(1.0, 0.0);
            std::vector<std::vector<int>> parities;
            for (int i=0; i<qubits.size(); i++){
                std::vector<int> row;
                for (int j=0; j<qubits[0].size(); j++){
                    int parity = CNOT(control, i, j);
                    row.push_back(parity);
                };
                parities.push_back(row);
            };
            return parities;
        };

        double hamiltonian_z(std::vector<std::vector<int>> parities){
            std::vector<double> hamiltonian;
            double e_z = 0.0;
            for (int i=0; i<parities.size(); i++){
                for (int j=0; j<parities.size(); j++){
                    std::vector<std::vector<int>> neighbours = get_neighbours(i, j);
                    for (int k=0; k<neighbours.size(); k++){
                        int ni = neighbours[k][0];
                        int nj = neighbours[k][1];

                        e_z = e_z + (-J*parities[i][j]*parities[ni][nj]);
                    };
                };
            };
            return e_z;
        };

        std::vector<std::vector<Qubit>> choose_random_state(){
            std::vector<std::vector<Qubit>> random_state;
            std::uniform_int_distribution<int> dist(0, 1);
            for (int i=0; i<lattice.size(); i++){
                std::vector<Qubit> row;
                for (int j=0; j<lattice[0].size(); j++){
                    int n = dist(rng);
                    int alpha = (0 + n) % 2;
                    int beta = (1 + n) % 2;
                    row.push_back(Qubit(alpha, beta));
                }
                random_state.push_back(row);
            }
            return random_state;
        };

        double psi(std::vector<std::vector<Qubit>> qubits){
            return 1.0;
        };

        double hamiltonian_x(std::vector<std::vector<Qubit>> qubits){
            double e_x = 0.0;
            double N = 0.0;
            std::vector<std::vector<double>> M = {{0,1}, {1,0}};
            for (int i=0; i<qubits.size(); i++){
                for (int j=0; j<qubits.size(); j++){
                    std::vector<std::vector<Qubit>> duplicate = qubits;
                    duplicate[i][j] = flip_spin(duplicate[i][j]);
                    e_x = e_x - (h*psi(duplicate)/psi(qubits));
                };
            };
            return e_x;
        };

        double calculate_energy(){
            std::vector<std::vector<Qubit>> microstate = choose_random_state();
            return hamiltonian_z(get_parities(microstate)) + hamiltonian_x(microstate);
        };
};