#include <iostream>
#include <vector>

class Qubit {
    public:
        Qubit(double alpha, double beta){
            double norm = std::sqrt(alpha*alpha + beta*beta);
            std::vector<double> state = {alpha/norm, beta/norm};
        };

        std::vector<double> get_state(){
            return state;
        };

        void set_state(double alpha, double beta){
            double norm = std::sqrt(alpha*alpha + beta*beta);
            state[0] = alpha/norm;
            state[1] = beta/norm;
        };
        int measure_spin(){
            double prob_0 = state[0]*state[0];
            double prob_1 = state[1]*state[1];
            double rand_num = static_cast<double>(rand()) / RAND_MAX;
            if(rand_num < prob_0){
                return 1;
            } else {
                return -1;
            }
        };

    private:
        std::vector<double> state;
};