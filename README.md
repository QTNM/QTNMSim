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
A run timer is the latest update, printing to screen at the end of run.

Additional output to file is the latest update. Two separate ntuples are stored in a ROOT output file: ntuple 'Score' holds 
data extracted for each electron hit, i.e. a scattering event of the primary electron and any daughter particle on any 
material, for instance a Tritium gas atom.

The ntuple 'Signal' stores all the trajectory data, produced by the primary electron or any daughter as it is transported. 
That data is ordered according to track ID and antenna ID per event, with the majority of data consisting of time points and 
anntenna response points in time-order. Added in the latest update are the starter conditions, i.e. the location and 
momentum direction vector of the primary particle from the begin of the event.
