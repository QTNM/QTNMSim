
# Test0

A simple test which allows the user to output a ROOT file containing cross-sections, as calculated by QTNMSim, as a function of energy.

## Running the Test

By default the cross section values for elastic scattering are output. As a secondary option, inelastic (impact ionisation) cross sections can be selected via the Inelastic scattering physics list:

```
$ ./TestEm0 -p InelasticScatteringList
```

Further options are available via checking:

```
$ ./TestEm0 --help
```

## Processing the Results

The test also provides a python script for reading and visualising results. Options can be listed:

```
$ python3 test0.py --help
```

Results may be compared to a set of reference or experimental values. An example file is provided with the test, `shah1987.txt`, which contains a set of experimental results for the impact ionisation of Hydrogen (DOI 10.1088/0022-3700/20/14/022). If a set of reference results are provided to the python routine they will be plotted alongside the obtained results from the ROOT file. If no reference results are provided (the default) a single value for elastic scattering obtained from the NIST database (https://srdata.nist.gov/srd64/Elastic) at 18.6KeV will be plotted for reference.

Further reference data sets may be added. The format is a CSV file, with two header lines. The first header line is used for the plot legend. After the header lines the file contains pairs of energy and cross section values in units of eV and m^2:

```
$ head -n 4 shah1987.txt
# Shah et al. 1987 J. Phys. B: Atom. Mol. Phys. 20 3501
# Energy(eV), CrossSection (m^2)
14.6,0.544e-21
14.8,0.661e-21
```