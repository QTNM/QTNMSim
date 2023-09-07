
# QTNMSim
Geant4 simulation of physics processes in the QTNM CRESDA geometry.

Collect requirements in issue tracker and start populating code base.

## Build instruction

At Warwick, SCRTP, use cvmfs as the easiest environment setup (with bash):

source /cvmfs/sft.cern.ch/lcg/views/LCG_103/x86_64-centos7-gcc11-opt/setup.sh

which sets up Geant4 11.1 and GCC11 on a CentOS7 background. ROOT 6.28 and cmake 3.23 will also be available. Just create a 'build' directory, then 

cd build; cmake ..; make

and run in the build directory.

### Status update

Compiles and runs at this stage for the test0/1/2/3.mac test cases. Most required physics features have been implemented.
The flexible primary particle event generator, the GPS, was implemented. Pre-defined primary particle event generators 
remain, i.e a test generator 
(default energy, at origin, 90 degree pitch angle, aka in x-direction), the electron gun generator and the Tritium beta decay generator. The GPS allows creating a primary electron conveniently by macro, very flexible configurations are possible.
A run timer prints to screen at the end of run.

Output to file includes two separate ntuples, stored in a ROOT output file: ntuple 'Score' holds 
data extracted for each electron hit, i.e. a scattering event of the primary electron and any daughter particle on any 
material, for instance a Tritium gas atom.

The ntuple 'Signal' stores all the trajectory data, produced by the primary electron or any daughter as it is transported. 
That data contains event ID and track ID, and the majority of data consists of time points in nanoseconds and 
anntenna response points in Volt, with an antenna ID vector labelling each (time, volt) point in time-order. Added are the 
trajectory starter conditions, i.e. the primary position components in millimetre, pitch angle, 
i.e. emission angle with respect to the z-axis, and the initial kinetic energy of the primary particle from the beginning of 
the trajectory.

The number density (atoms/cm3) of the tritium material is printed to screen for validation (defined in default g/cm3 units). 
Also, the lower energy limit of the tritium decay generator now stands at 1.5 keV, avoiding too low energy electrons with 
cyclotron radii to small to sample by realistic maximum step lengths, say 0.1mm.

While testing for energy conservation which affects the chirp of the signal, the 'Signal' ntuple also contains the
kinetic energy in MeV (G4 default) for each step. Next attempt after checking the textbook Boris algorithm is Tom Goffrey's 
analytic solution of the For, O'Connell equations in 3D.
