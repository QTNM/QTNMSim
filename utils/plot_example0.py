import numpy as np
import matplotlib.pyplot as plt
import ROOT


def plot_signal():
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
    plt.savefig('example0.png')


if __name__ == "__main__":
    plot_signal()
