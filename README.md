# Quantum Ising Model with a Transverse Magnetic Field

## Explanation of Project

### Lattice of Qubits and the Hamiltonian

$$
H = -J\sum_{\langle i,j\rangle}\sigma_i^z\sigma_j^z
-h\sum_i\sigma_i^x
$$

We use a classical thermodynamic approach to calculate the energy between neighbouring spins in the first sum. $\sigma_z$ represents a measurement of spin in the $z$-basis.


The second sum accounts for the energy from the magnetic field applied in the $x$ direction (perpendicular). the spin in the $x$-basis is calculated using $\sigma_x$.

Applying a magnetic field causes an uncertainty in the $z$-spin, so we represent the spins as qubits, all of them are initialized in the state:

$$
|\psi\rangle = \frac{1}{\sqrt{2}}(|0\rangle + |1\rangle)
$$


### Finding the Energy of the Ground State of the System Using a Monte-Carlo Simulation and a CNN

If we use a 3D lattice of 10x10x10 spins/dipoles then our quantum state would live in a $2^{1000}$ dimensional Hilbert Space and manually calculating the energy of each full microstate would be too inefficient. As a workaround we consider a possible configuration of spins e.g. $|00101110\rangle$ and an initial wavefunction. We then compute

$$
E = \frac{\langle\psi|H|\psi\rangle}{\langle\psi|\psi\rangle}
$$

where $|\psi\rangle = \sum_{C}\psi(C)$ where C is a possible microstate. 

If we sample possible states C one at a time:
$$
E_{loc}(C) = \frac{\langle\psi(C)|H|\psi(C')\rangle}{|\psi(C)|^2}
$$

and rearrange:
$$
E_{loc}(C) = \langle\psi(C)|H|\psi(C')\rangle\frac{\psi(C')}{\psi(C)}
$$

then use a monte carlo simulation:
$$
\frac{1}{M}\sum^ME_{loc}(C)
$$

States are chosen if they have a lower energy than the previous state, $\langle\psi(C)|H|\psi(C')\rangle$ is calculated by using the energy in the classical ising model along with the spin flip operator $\sigma_x$ for the magnetic field term.

$\psi(C)$ is determined using a CNN and the energy found is used to train it so the wavefunction found minimises E.