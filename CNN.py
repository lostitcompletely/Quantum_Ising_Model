import torch
import torch.nn as nn


class CNNWavefunction(nn.Module):

    def __init__(
        self,
        lattice_size,
        num_filters=8,
        kernel_size=3
    ):
        super().__init__()

        self.lattice_size = lattice_size

        self.conv1 = nn.Conv2d(
            in_channels=1,
            out_channels=num_filters,
            kernel_size=kernel_size,
            padding=0
        )

        self.conv2 = nn.Conv2d(
            in_channels=num_filters,
            out_channels=num_filters,
            kernel_size=kernel_size,
            padding=0
        )

        L1 = lattice_size - kernel_size + 1
        L2 = L1 - kernel_size + 1

        flattened_size = num_filters * L2 * L2

        self.output = nn.Linear(
            flattened_size,
            1
        )

        self.initialize_weights()

    def initialize_weights(self):

        for layer in [self.conv1, self.conv2]:

            nn.init.normal_(
                layer.weight,
                mean=0.0,
                std=0.01
            )

            nn.init.zeros_(layer.bias)

        nn.init.normal_(
            self.output.weight,
            mean=0.0,
            std=0.01
        )

        nn.init.zeros_(
            self.output.bias
        )

    def forward(self, C):

        if C.dim() == 2:

            C = C.unsqueeze(0)
            C = C.unsqueeze(0)

        X = self.conv1(C)

        X = torch.tanh(X)

        X = self.conv2(X)

        X = torch.tanh(X)

        X = torch.flatten(
            X,
            start_dim=1
        )

        f = self.output(X)

        return f.squeeze()

    def psi(self, C):

        f = self.forward(C)

        return torch.exp(f)