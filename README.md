# QTNMSim
Geant4 simulation of physics processes in the QTNM CRESDA geometry.

Collect requirements in issue tracker and start populating code base.

## Build instruction (REQUIREMENTS - no build yet)

At Warwick, SCRTP, use cvmfs as the easiest environment setup (with bash):

source /cvmfs/sft.cern.ch/lcg/views/LCG_102/x86_64-centos7-gcc10-opt/setup.sh

which sets up Geant4 11p02 and GCC11 on a CentOS7 background. ROOT 6.26 and cmake 3.20 will also be available. Just create a 'build' 
directory, then 

cd build; cmake ..; make

and run in the build directory.
