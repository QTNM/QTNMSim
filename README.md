
# QTNMSim
Geant4 simulation of physics processes in the QTNM CRESDA geometry.

Collect requirements in issue tracker and start populating code base.

## Build instruction

At Warwick, SCRTP, use cvmfs as the easiest environment setup (with bash):

```
source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-el9-gcc13-opt/setup.sh
```

Anyone using CentOS7 can source the following environment:

```
source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-centos7-gcc12-opt/setup.sh
```

which sets up Geant4 11.2 and GCC13(12) on a CentOS9(7) background. ROOT 6.30 and cmake 3.26 will also be available. Just create a 'build' directory, then 

```
cd build; cmake ..; make
```

and run in the build directory.

## Geometry

QTNMSim specifies geometry via a GDML file, passed as a command line argument. New geometry files can be generated using the [pyg4ometry package](https://www.pp.rhul.ac.uk/bdsim/pyg4ometry/index.html#). This can be installed using pip:

```
pip install pyg4ometry
```

or HomeBrew:

```
brew install vtk cgal antlr4-cpp-runtime pybind11 cython opencascade mpfr
```

or built from source. New geometry files can then be generated:

```
cd geometry/
python testscript.py
```

## Test Results

The code is automatically tested upon new pushes. Plots from recent tests can be downloaded from:
https://github.com/QTNM/QTNMSim/actions?query=workflow%3ABuildandTest
