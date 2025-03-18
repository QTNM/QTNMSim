#!/usr/bin/env python3
import argparse
import numpy as np
import matplotlib.pyplot as plt
import ROOT


def parse_arguments():
    parser = argparse.ArgumentParser(
        description="""Secondary sample analysis tool
        """,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )

    parser.add_argument(
        "--xscale",
        default="linear",
        type=str,
        help="Control log/linear scaling of x-axis",
        nargs="?",
    )

    parser.add_argument(
        "--yscale",
        default="linear",
        type=str,
        help="Control log/linear scaling of y-axis",
        nargs="?",
    )

    parser.add_argument("--output",
                        default="ElectronEnergy.png",
                        help="Filename to save plot to.",
                        type=str,
                        nargs="?"
                        )

    parser.add_argument("--input",
                        default="secondaries.root",
                        type=str,
                        help="File, to analyse",
                        nargs="?",
                        )

    return parser.parse_args()


def summary(fname_in, fname_out, xscale, yscale):
    input_file = ROOT.TFile(fname_in)
    tree = input_file.Get("SecondarySamples")

    energy = []
    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        energy.append(tree.Energy)
    energy = np.array(energy)

    fig, ax1 = plt.subplots()
    ax2 = ax1.twinx()

    # Plot histogram
    bins = np.linspace(0,100,101)
    ax1.hist(energy, bins=bins, color='blue')
    ax1.set_xlabel(r'Energy (eV)', fontsize=16)
    ax1.set_ylabel(r'Counts', color='blue', fontsize=16)
    ax1.tick_params(axis='both', labelsize=16)
    ax1.tick_params(axis='y', labelsize=16, labelcolor='blue')
    ax1.set_xscale(xscale)
    ax1.set_yscale(yscale)
    if xscale == 'linear':
        ax1.set_xlim(left=0)
    if yscale == 'linear':
        ax1.set_ylim(bottom=0)

    ax2.ecdf(energy, color='red')
    ax2.set_ylabel('CDF', color='red', fontsize=16)
    ax2.tick_params(axis='y', labelsize=16, labelcolor='red')

    plt.title('Secondary Electron Energy', fontsize=18)
    plt.tight_layout()
    plt.savefig(fname_out)

if __name__ == "__main__":
    options = parse_arguments()

    summary(fname_in=options.input, fname_out=options.output,
            xscale=options.xscale, yscale=options.yscale)
