#!/usr/bin/env python3
import argparse
import numpy as np
import matplotlib.pyplot as plt
import ROOT


def parse_arguments():
    parser = argparse.ArgumentParser(
        description="""Cross Section analysis tool
        """,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )

    parser.add_argument(
        "--xscale",
        default="log",
        type=str,
        help="Control log/linear scaling of x-axis",
        nargs="?",
    )

    parser.add_argument(
        "--yscale",
        default="log",
        type=str,
        help="Control log/linear scaling of y-axis",
        nargs="?",
    )

    parser.add_argument("--reference_results",
                        help="""Filename to read reference/experimental
                        data from. If not provided, a reference result
                        for elastic scattering from NIST data will be
                        used""",
                        type=str,
                        nargs="?"
                        )

    parser.add_argument("--output",
                        default="ElasticCrossSection.png",
                        help="Filename to save plot to.",
                        type=str,
                        nargs="?"
                        )

    parser.add_argument("--input",
                        default="CrossSection.root",
                        type=str,
                        help="File, to analyse",
                        nargs="?",
                        )

    return parser.parse_args()


def summary(fname_in="CrossSection.root", fname_out="ElasticCrossSection.png",
            xscale="log", yscale="log", reference=None):
    input_file = ROOT.TFile(fname_in)
    tree = input_file.Get("CrossSection")

    energy = []
    sigma = []
    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        energy.append(tree.Energy)
        sigma.append(tree.CrossSection)

    # Plot Geant4
    plt.plot(energy, sigma, label='Geant4')

    if reference is None:
        # Plot NIST Value - accessed 17/02/25
        nist_value = 5.662165e-3 * 2.8002852e-21
        plt.plot(18.6, nist_value, linestyle='', marker='x',
                 label=f'NIST Value = {nist_value:.4E}', markersize=4)
    else:
        # Read from file
        reference_data = np.loadtxt(reference, delimiter=',')
        with open(reference, encoding='utf-8') as f:
            label = f.readline()
        plt.plot(reference_data[:,0]*1e-3, reference_data[:,1], linestyle='',
                 marker='x', label=label[2:], markersize=4)

    plt.xlabel(r'Energy (KeV)')
    plt.ylabel(r'$\sigma (m^2)$')
    plt.legend()
    plt.xscale(xscale)
    plt.yscale(yscale)
    if xscale == 'linear':
        plt.xlim(left=0)
    if yscale == 'linear':
        plt.ylim(bottom=0)
    plt.tight_layout()
    plt.savefig(fname_out)

if __name__ == "__main__":
    options = parse_arguments()

    summary(fname_in=options.input, fname_out=options.output,
            xscale=options.xscale, yscale=options.yscale,
            reference=options.reference_results)
