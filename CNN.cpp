#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

using namespace std;



// [channel][row][column]
using Tensor = vector<vector<vector<double>>>;


// Utility: create tensor
Tensor make_tensor(
    int channels,
    int height,
    int width,
    double value = 0.0
) {
    return Tensor(
        channels,
        vector<vector<double>>(
            height,
            vector<double>(width, value)
        )
    );
}

// ReLU
Tensor relu(const Tensor& x) {

    int channels = x.size();
    int height = x[0].size();
    int width = x[0][0].size();

    Tensor output =
        make_tensor(channels, height, width);


    for (int c = 0; c < channels; c++) {

        for (int i = 0; i < height; i++) {

            for (int j = 0; j < width; j++) {

                output[c][i][j] =
                    max(0.0, x[c][i][j]);
            }
        }
    }

    return output;
}

// ReLU backward
//
// If x > 0:
//
// dReLU/dx = 1
//
// If x < 0:
//
// dReLU/dx = 0

Tensor relu_backward(
    const Tensor& x,
    const Tensor& grad_output
) {

    int channels = x.size();
    int height = x[0].size();
    int width = x[0][0].size();

    Tensor grad_input =
        make_tensor(channels, height, width);


    for (int c = 0; c < channels; c++) {

        for (int i = 0; i < height; i++) {

            for (int j = 0; j < width; j++) {

                if (x[c][i][j] > 0.0) {

                    grad_input[c][i][j] =
                        grad_output[c][i][j];

                } else {

                    grad_input[c][i][j] = 0.0;
                }
            }
        }
    }

    return grad_input;
}

// Conv2D
class Conv2D {

public:

    int input_channels;
    int output_channels;

    int height;
    int width;

    int kernel_size;
    int padding;


    // W[output_channel]
    //  [input_channel]
    //  [kernel_row]
    //  [kernel_column]

    vector<vector<vector<vector<double>>>> W;

    vector<double> b;


    // Gradients

    vector<vector<vector<vector<double>>>> dW;

    vector<double> db;


    // Saved input for backprop

    Tensor cached_input;


    Conv2D(
        int in_channels,
        int out_channels,
        int h,
        int w,
        int kernel = 3,
        int pad = 1
    )
        : input_channels(in_channels),
          output_channels(out_channels),
          height(h),
          width(w),
          kernel_size(kernel),
          padding(pad)
    {

        // Allocate weights

        W.resize(output_channels);

        dW.resize(output_channels);


        for (int f = 0; f < output_channels; f++) {

            W[f].resize(input_channels);
            dW[f].resize(input_channels);


            for (int c = 0; c < input_channels; c++) {

                W[f][c].resize(kernel_size);

                dW[f][c].resize(kernel_size);


                for (int i = 0; i < kernel_size; i++) {

                    W[f][c][i].resize(kernel_size);
                    dW[f][c][i].resize(kernel_size);


                    for (int j = 0; j < kernel_size; j++) {

                        // Temporary initialization

                        W[f][c][i][j] = 0.01;

                        dW[f][c][i][j] = 0.0;
                    }
                }
            }
        }


        // Biases

        b.resize(output_channels, 0.0);
        db.resize(output_channels, 0.0);
    }

    // Forward
    Tensor forward(const Tensor& input) {

        // Save input because we need it during backprop

        cached_input = input;


        Tensor output =
            make_tensor(
                output_channels,
                height,
                width
            );


        // Loop over output filters

        for (int f = 0; f < output_channels; f++) {


            // Loop over lattice

            for (int i = 0; i < height; i++) {

                for (int j = 0; j < width; j++) {

                    double sum = b[f];


                    // Loop over input channels

                    for (int c = 0;
                         c < input_channels;
                         c++) {


                        // Loop over kernel

                        for (int ki = 0;
                             ki < kernel_size;
                             ki++) {

                            for (int kj = 0;
                                 kj < kernel_size;
                                 kj++) {


                                int input_i =
                                    i + ki - padding;

                                int input_j =
                                    j + kj - padding;


                                // Zero padding

                                if (
                                    input_i >= 0 &&
                                    input_i < height &&
                                    input_j >= 0 &&
                                    input_j < width
                                ) {

                                    sum +=
                                        W[f][c][ki][kj]
                                        * input[c][input_i][input_j];
                                }
                            }
                        }
                    }


                    output[f][i][j] = sum;
                }
            }
        }

        return output;
    }

    // Backward
    //
    // grad_output =
    //
    // dL/d(output)
    //
    // We calculate:
    //
    // dL/dW
    // dL/db
    // dL/dinput
    Tensor backward(
        const Tensor& grad_output
    ) {

        Tensor grad_input =
            make_tensor(
                input_channels,
                height,
                width
            );


        // Reset gradients

        for (int f = 0; f < output_channels; f++) {

            db[f] = 0.0;

            for (int c = 0; c < input_channels; c++) {

                for (int ki = 0;
                     ki < kernel_size;
                     ki++) {

                    for (int kj = 0;
                         kj < kernel_size;
                         kj++) {

                        dW[f][c][ki][kj] = 0.0;
                    }
                }
            }
        }

        // Calculate gradients
        for (int f = 0; f < output_channels; f++) {

            for (int i = 0; i < height; i++) {

                for (int j = 0; j < width; j++) {

                    double upstream =
                        grad_output[f][i][j];

                    // Bias gradient
                    db[f] += upstream;

                    // Weight gradients
                    for (int c = 0;
                         c < input_channels;
                         c++) {

                        for (int ki = 0;
                             ki < kernel_size;
                             ki++) {

                            for (int kj = 0;
                                 kj < kernel_size;
                                 kj++) {


                                int input_i =
                                    i + ki - padding;

                                int input_j =
                                    j + kj - padding;


                                if (
                                    input_i >= 0 &&
                                    input_i < height &&
                                    input_j >= 0 &&
                                    input_j < width
                                ) {

                                    dW[f][c][ki][kj]
                                        +=
                                        upstream
                                        *
                                        cached_input[c]
                                            [input_i]
                                            [input_j];


                                    // Gradient with respect
                                    // to the input

                                    grad_input[c]
                                        [input_i]
                                        [input_j]
                                        +=
                                        upstream
                                        *
                                        W[f][c][ki][kj];
                                }
                            }
                        }
                    }
                }
            }
        }


        return grad_input;
    }

    // Update weights
    void update(double learning_rate) {

        for (int f = 0; f < output_channels; f++) {

            b[f] -=
                learning_rate * db[f];


            for (int c = 0; c < input_channels; c++) {

                for (int ki = 0;
                     ki < kernel_size;
                     ki++) {

                    for (int kj = 0;
                         kj < kernel_size;
                         kj++) {

                        W[f][c][ki][kj]
                            -=
                            learning_rate
                            * dW[f][c][ki][kj];
                    }
                }
            }
        }
    }
};


// Dense layer
class Dense {

public:

    int input_size;
    int output_size;


    vector<vector<double>> W;
    vector<double> b;


    vector<vector<double>> dW;
    vector<double> db;


    vector<double> cached_input;


    Dense(int input, int output)
        : input_size(input),
          output_size(output)
    {

        W.resize(
            output_size,
            vector<double>(input_size)
        );

        dW.resize(
            output_size,
            vector<double>(input_size)
        );

        b.resize(output_size, 0.0);
        db.resize(output_size, 0.0);


        for (int i = 0; i < output_size; i++) {

            for (int j = 0; j < input_size; j++) {

                W[i][j] = 0.01;
            }
        }
    }


    vector<double> forward(
        const vector<double>& input
    ) {

        cached_input = input;

        vector<double> output(output_size);


        for (int i = 0; i < output_size; i++) {

            double sum = b[i];


            for (int j = 0; j < input_size; j++) {

                sum +=
                    W[i][j] * input[j];
            }


            output[i] = sum;
        }


        return output;
    }


    vector<double> backward(
        const vector<double>& grad_output
    ) {

        vector<double> grad_input(input_size, 0.0);


        for (int i = 0; i < output_size; i++) {

            db[i] = grad_output[i];


            for (int j = 0; j < input_size; j++) {

                dW[i][j] =
                    grad_output[i]
                    * cached_input[j];


                grad_input[j] +=
                    grad_output[i]
                    * W[i][j];
            }
        }


        return grad_input;
    }


    void update(double learning_rate) {

        for (int i = 0; i < output_size; i++) {

            b[i] -=
                learning_rate * db[i];


            for (int j = 0; j < input_size; j++) {

                W[i][j] -=
                    learning_rate
                    * dW[i][j];
            }
        }
    }
};


// ============================================================
// Flatten
// ============================================================

vector<double> flatten(
    const Tensor& input
) {

    vector<double> output;


    for (const auto& channel : input) {

        for (const auto& row : channel) {

            for (double value : row) {

                output.push_back(value);
            }
        }
    }


    return output;
}

// CNN
class CNN {

public:

    Conv2D conv1;
    Conv2D conv2;
    Conv2D conv3;

    Dense dense;


    // Cached values needed for backprop

    Tensor z1;
    Tensor z2;
    Tensor z3;


    CNN()

        // input channels
        // output channels
        // height
        // width

        : conv1(1, 8, 4, 4),

          conv2(8, 16, 4, 4),

          conv3(16, 16, 4, 4),

          // 16 × 4 × 4 = 256

          dense(256, 1)
    {}

    // Forward
    double forward(
        const Tensor& configuration
    ) {

        // Convolution 1
        z1 = conv1.forward(configuration);

        Tensor a1 = relu(z1);


        // Convolution 2
        z2 = conv2.forward(a1);

        Tensor a2 = relu(z2);


        // Convolution 3
        z3 = conv3.forward(a2);

        Tensor a3 = relu(z3);


        // Flatten
        vector<double> flattened =
            flatten(a3);


        // Dense
        vector<double> output =
            dense.forward(flattened);


        // f_theta(s)
        double f_theta =
            output[0];


        // Wavefunction
        double psi =
            exp(f_theta);


        return psi;
    }
};



// Main
int main() {

    // 4 × 4 spin configuration
    Tensor configuration =
        make_tensor(1, 4, 4);


    configuration[0] = {

        {0, 1, 1, 0},

        {1, 0, 0, 1},

        {1, 1, 0, 0},

        {0, 1, 0, 1}
    };


    // Create network
    CNN cnn;

    // Forward pass
    double psi =
        cnn.forward(configuration);


    cout << "psi(s) = "
         << psi
         << endl;


    return 0;
}