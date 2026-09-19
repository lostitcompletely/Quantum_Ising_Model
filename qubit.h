#pragma once

#include <vector>
#include <random>

class Qubit {
    public:
        Qubit(double alpha, double beta);
        std::vector<double> get_state();
        void set_state(double alpha, double beta);
        int measure_spin();

    private:
        std::vector<double> state;

};