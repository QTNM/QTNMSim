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
    plt.plot(TimeArr[AntennaIDArr == 0], VoltageArr[AntennaIDArr == 0], label='Antenna 0')
    plt.plot(TimeArr[AntennaIDArr == 1], VoltageArr[AntennaIDArr == 1], label='Antenna 1', linestyle='--')
    plt.legend()
    plt.xlabel('Time (ns)')
    plt.ylabel('Voltage')
    plt.tight_layout()
    plt.savefig('test0.png')


if __name__ == "__main__":
    plot_signal()
