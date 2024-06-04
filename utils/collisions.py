#!/usr/bin/env python3
import argparse
import matplotlib.pyplot as plt
import numpy as np
import scipy.constants as const
from scipy.optimize import curve_fit
import ROOT


def parse_arguments():
    parser = argparse.ArgumentParser(
        description="""Electron gun analysis tool
        """,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )

    parser.add_argument(
        "--density",
        default=5e-9,
        type=float,
        help="Background density (g/cm^3)",
        nargs="?",
    )

    parser.add_argument("--filename",
                        default="qtnm.root",
                        type=str,
                        help="File, or file containing list of files to \
                        analyse",
                        nargs="?",
                        )

    return parser.parse_args()


def beta(T):
    return np.sqrt(1 - (1 / gamma(T)**2))


def gamma(T):
    mc2 = const.electron_mass * const.c**2 / const.elementary_charge / 1e3
    return (T + mc2) / mc2


def summary(density=5e-9, fname="qtnm.root"):
    inputFile = ROOT.TFile(fname)
    score = inputFile.Get('ntuple').Get('Score')

    times = []
    n = score.GetEntries()

    id_last = -1
    for i in range(n):
        score.GetEntry(i)
        # Only store first collision
        if score.EventID == id_last:
            continue
        times.append(score.TimeStamp)
        id_last = score.EventID

    n = len(times)
    times = np.array(times) * 1e-9 # Seconds
    v0 = beta(18.575) * const.c
    paths = times * v0

    (counts, bins, _) = plt.hist(paths, bins=1000, cumulative=-1)
    d_g4 = density #g/cm^3
    n_SI = d_g4 * 1e3 / const.atomic_mass / 3.014 # Assume atomic Tritium

    def decay(x, mfp):
        return n * np.exp(-x/mfp)

    bin_c = 0.5 * (bins[1:] + bins[:-1])
    popt, pcov = curve_fit(decay, bin_c, counts)

    # Calculate sigma from mfp
    sigma = 1.0 / n_SI / popt[0]
    dsigma = np.sqrt(np.diag(pcov))[0] * sigma / popt[0]
    # Calculate error in sigma from mfp

    sigma_cm2 = sigma * 1e4
    print('Observed cross section = %e, +/- %e cm^2' % (sigma_cm2, dsigma * 1e4))
    plt.plot(bin_c, decay(bin_c, popt[0]), label=r'$\sigma_\mathrm{obs} = $%.3e' % (sigma_cm2) + r'$\;\mathrm{cm}^2$')

    plt.xlabel('x(m)')
    plt.ylabel('Intensity')
    plt.xlim(left=0)
    plt.legend()
    plt.savefig('test.png')

if __name__ == "__main__":
    options = parse_arguments()
    summary(options.density, options.filename)
