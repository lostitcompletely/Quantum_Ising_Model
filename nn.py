import numpy as np


# ============================================================
# ACTIVATION
# ============================================================

def silu(x):
    return x / (1.0 + np.exp(-x))


# ============================================================
# 3D PERIODIC CONVOLUTION
# ============================================================

def conv3d(X, K, b):
    """
    X:
        (C_in, D, H, W)

    K:
        (C_out, C_in, Kd, Kh, Kw)

    b:
        (C_out,)

    Output:
        (C_out, D, H, W)
    """

    C_in, D, H, W = X.shape
    C_out, _, Kd, Kh, Kw = K.shape

    output = np.zeros(
        (C_out, D, H, W)
    )

    # Centre of kernel
    kd = Kd // 2
    kh = Kh // 2
    kw = Kw // 2

    for out_c in range(C_out):

        for z in range(D):
            for y in range(H):
                for x in range(W):

                    value = b[out_c]

                    for in_c in range(C_in):

                        for dz in range(Kd):
                            for dy in range(Kh):
                                for dx in range(Kw):

                                    zz = (z + dz - kd) % D
                                    yy = (y + dy - kh) % H
                                    xx = (x + dx - kw) % W

                                    value += (
                                        K[
                                            out_c,
                                            in_c,
                                            dz,
                                            dy,
                                            dx
                                        ]
                                        *
                                        X[
                                            in_c,
                                            zz,
                                            yy,
                                            xx
                                        ]
                                    )

                    output[
                        out_c,
                        z,
                        y,
                        x
                    ] = value

    return output


# ============================================================
# CNN
# ============================================================

class CNN3D:

    def __init__(
        self,
        lattice_size,
        channels=(8, 16, 32),
        kernel_size=3
    ):

        self.L = lattice_size

        # ----------------------------------------------------
        # Store all convolutional kernels and biases
        # ----------------------------------------------------

        self.K = []
        self.b = []

        channel_sizes = [1] + list(channels)

        for i in range(len(channel_sizes) - 1):

            C_in = channel_sizes[i]
            C_out = channel_sizes[i + 1]

            K = np.random.randn(
                C_out,
                C_in,
                kernel_size,
                kernel_size,
                kernel_size
            ) * 0.01

            b = np.zeros(C_out)

            self.K.append(K)
            self.b.append(b)

        # ----------------------------------------------------
        # Final linear layer
        # ----------------------------------------------------

        final_channels = channels[-1]

        n_features = (
            final_channels
            * lattice_size
            * lattice_size
            * lattice_size
        )

        self.W = np.random.randn(
            1,
            n_features
        ) * 0.01

        self.final_b = np.zeros(1)


    # ========================================================
    # FORWARD PASS
    # ========================================================

    def forward(self, C):

        # C has shape:
        #
        # (L, L, L)
        #
        # Add channel dimension

        X = C[np.newaxis, :, :, :]

        # ----------------------------------------------------
        # Convolutional hidden layers
        # ----------------------------------------------------

        for i in range(len(self.K)):

            X = conv3d(
                X,
                self.K[i],
                self.b[i]
            )

            X = silu(X)

        # ----------------------------------------------------
        # Flatten
        # ----------------------------------------------------

        X = X.reshape(-1)

        # ----------------------------------------------------
        # Final scalar
        # ----------------------------------------------------

        f = self.W @ X + self.final_b

        return f[0]