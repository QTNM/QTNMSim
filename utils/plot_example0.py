#!/usr/bin/env python3
import argparse
import numpy as np
import matplotlib.pyplot as plt
import ROOT


def parse_arguments():
    parser = argparse.ArgumentParser(
        description="""Plotting script for example0
        """,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )

    parser.add_argument("--output",
                        default="example0.png",
                        help="Filename to save plot to.",
                        type=str,
                        nargs="?"
                        )

    return parser.parse_args()

def plot_signal(output_file="example0.png"):
    f = ROOT.TFile('qtnm.root')
    t = f.Get('ntuple/Signal')
    t.GetEntry(0)
    TimeArr = np.array(t.TimeVec)
    AntennaIDArr = np.array(t.AntennaID)
    VoltageArr = np.array(t.VoltageVec)
    for i in set(AntennaIDArr):
        plt.plot(TimeArr[AntennaIDArr == i], VoltageArr[AntennaIDArr == i], label=f"Antenna {i}")
    plt.legend()
    plt.xlabel('Time (ns)')
    plt.ylabel('Voltage')
    plt.xlim(left=0)
    plt.tight_layout()
    plt.savefig(output_file)


if __name__ == "__main__":
    options = parse_arguments()
    plot_signal(options.output)
