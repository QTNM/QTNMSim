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
