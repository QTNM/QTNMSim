
# QTNMSim
Geant4 simulation of physics processes in the QTNM CRESDA geometry.

Collect requirements in issue tracker and start populating code base.

## Build instruction

At Warwick, SCRTP, use cvmfs as the easiest environment setup (with bash):

source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-el9-gcc13-opt/setup.sh

Anyone using CentOS7 can source the following environment:

source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-centos7-gcc12-opt/setup.sh

which sets up Geant4 11.2 and GCC13(12) on a CentOS9(7) background. ROOT 6.30 and cmake 3.26 will also be available. Just create a 'build' directory, then 

cd build; cmake ..; make

and run in the build directory.

## Test Results

The code is automatically tested upon new pushes. Plots from recent tests can be downloaded from:
https://github.com/QTNM/QTNMSim/actions?query=workflow%3ABuildandTest

### Status update

Added storage of omega values and kinetic energy values. The former permits to determine the truth value
for the beat frequency (side-bands), the latter yields the truth value for the chirp-rate.

Version v0.4-beta: repaired hit scoring bug. The standard filtering for deposited energy misses a large number of
Coulomb scatter hits. Two new hit filters were imlemented in QTGasSD: first on process name, score all
non-transportation process steps; second, require a change of momentum pitch angle (theta to z-axis) of more than
1.e-3 radians or 0.057 degrees. Likewise, the unnecessary scoring from the maximum time limit process, i.e. at the
final step without any scattering process involved, has been removed.

Observation was that no hits were produced even in artificially increased Tritum gas densities. Inspection of tracking
log output shows that scattering does occur as expected but the predominant scattering outcome at relevant energies
is dominated by momentum changes rather than deposited energy. The momentum vector changes direction post-step and
that leads quite often to electrons leaving the trap hence is significant and should be stored as a hit.

The tagged version v0.3-beta is feature complete and tested. It remains a pre-release version since important
input data like a realistic geometry and corresponding field map are missing. Examples provided only permit
tests of functionality.

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
the trajectory. For testing, the kinetic energy for each step is also stored.

The number density (atoms/cm3) of the tritium material is printed to screen for validation (defined in default g/cm3 units). 
Also, the lower energy limit of the tritium decay generator now stands at 1.5 keV, avoiding too low energy electrons with 
cyclotron radii too small to sample by realistic maximum step lengths, say 0.3mm.

